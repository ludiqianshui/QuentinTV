/************************************************************************
*                                                                       *
*   NuiImpl.cpp -- Implementation of CSkeletalViewerApp methods dealing *
*                  with NUI processing                                  *
*                                                                       *
* Copyright (c) Microsoft Corp. All rights reserved.                    *
*                                                                       *
* This code is licensed under the terms of the                          *
* Microsoft Kinect for Windows SDK (Beta) from Microsoft Research       *
* License Agreement: http://research.microsoft.com/KinectSDK-ToU        *
*                                                                       *
************************************************************************/

#include "stdafx.h"
#include "VideoWallControl.h"
#include "resource.h"
#include <mmsystem.h>
#include <math.h>

static const COLORREF g_JointColorTable[NUI_SKELETON_POSITION_COUNT] = 
{
    RGB(169, 176, 155), // NUI_SKELETON_POSITION_HIP_CENTER
    RGB(169, 176, 155), // NUI_SKELETON_POSITION_SPINE
    RGB(168, 230, 29), // NUI_SKELETON_POSITION_SHOULDER_CENTER
    RGB(200, 0,   0), // NUI_SKELETON_POSITION_HEAD
    RGB(79,  84,  33), // NUI_SKELETON_POSITION_SHOULDER_LEFT
    RGB(84,  33,  42), // NUI_SKELETON_POSITION_ELBOW_LEFT
    RGB(255, 126, 0), // NUI_SKELETON_POSITION_WRIST_LEFT
    RGB(215,  86, 0), // NUI_SKELETON_POSITION_HAND_LEFT
    RGB(33,  79,  84), // NUI_SKELETON_POSITION_SHOULDER_RIGHT
    RGB(33,  33,  84), // NUI_SKELETON_POSITION_ELBOW_RIGHT
    RGB(77,  109, 243), // NUI_SKELETON_POSITION_WRIST_RIGHT
    RGB(37,   69, 243), // NUI_SKELETON_POSITION_HAND_RIGHT
    RGB(77,  109, 243), // NUI_SKELETON_POSITION_HIP_LEFT
    RGB(69,  33,  84), // NUI_SKELETON_POSITION_KNEE_LEFT
    RGB(229, 170, 122), // NUI_SKELETON_POSITION_ANKLE_LEFT
    RGB(255, 126, 0), // NUI_SKELETON_POSITION_FOOT_LEFT
    RGB(181, 165, 213), // NUI_SKELETON_POSITION_HIP_RIGHT
    RGB(71, 222,  76), // NUI_SKELETON_POSITION_KNEE_RIGHT
    RGB(245, 228, 156), // NUI_SKELETON_POSITION_ANKLE_RIGHT
    RGB(77,  109, 243) // NUI_SKELETON_POSITION_FOOT_RIGHT
};




void CSkeletalViewerApp::Nui_Zero()
{
    m_hNextDepthFrameEvent = NULL;
    m_hNextVideoFrameEvent = NULL;
    m_hNextSkeletonEvent = NULL;
    m_pDepthStreamHandle = NULL;
    m_pVideoStreamHandle = NULL;
    m_hThNuiProcess=NULL;
    m_hEvNuiProcessStop=NULL;
    ZeroMemory(m_Pen,sizeof(m_Pen));
    m_SkeletonDC = NULL;
    m_SkeletonBMP = NULL;
    m_SkeletonOldObj = NULL;
    m_PensTotal = 6;
    ZeroMemory(m_Points,sizeof(m_Points));
    m_LastSkeletonFoundTime = -1;
    m_bScreenBlanked = false;
    m_FramesTotal = 0;
    m_LastFPStime = -1;
    m_LastFramesTotal = 0;
}



HRESULT CSkeletalViewerApp::Nui_Init()
{
    HRESULT                hr;
    RECT                rc;

    m_hNextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hNextVideoFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hNextSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    GetWindowRect(GetDlgItem( m_hWnd, IDC_SKELETALVIEW ), &rc );
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    HDC hdc = GetDC(GetDlgItem( m_hWnd, IDC_SKELETALVIEW));
    m_SkeletonBMP = CreateCompatibleBitmap( hdc, width, height );
    m_SkeletonDC = CreateCompatibleDC( hdc );
    ::ReleaseDC(GetDlgItem(m_hWnd,IDC_SKELETALVIEW), hdc );
    m_SkeletonOldObj = SelectObject( m_SkeletonDC, m_SkeletonBMP );

    hr = m_DrawDepth.CreateDevice( GetDlgItem( m_hWnd, IDC_DEPTHVIEWER ) );
    if( FAILED( hr ) )
    {
        MessageBoxResource( m_hWnd,IDS_ERROR_D3DCREATE,MB_OK | MB_ICONHAND);
        return hr;
    }

    hr = m_DrawDepth.SetVideoType( 320, 240, 320 * 4 );
    if( FAILED( hr ) )
    {
        MessageBoxResource( m_hWnd,IDS_ERROR_D3DVIDEOTYPE,MB_OK | MB_ICONHAND);
        return hr;
    }

    hr = m_DrawVideo.CreateDevice( GetDlgItem( m_hWnd, IDC_VIDEOVIEW ) );
    if( FAILED( hr ) )
    {
        MessageBoxResource( m_hWnd,IDS_ERROR_D3DCREATE,MB_OK | MB_ICONHAND);
        return hr;
    }

    hr = m_DrawVideo.SetVideoType( 640, 480, 640 * 4 );
    if( FAILED( hr ) )
    {
        MessageBoxResource( m_hWnd,IDS_ERROR_D3DVIDEOTYPE,MB_OK | MB_ICONHAND);
        return hr;
    }
    
    hr = NuiInitialize( 
        NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_COLOR);
    if( FAILED( hr ) )
    {
        MessageBoxResource(m_hWnd,IDS_ERROR_NUIINIT,MB_OK | MB_ICONHAND);
        return hr;
    }

    hr = NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, 0 );
    if( FAILED( hr ) )
    {
        MessageBoxResource(m_hWnd,IDS_ERROR_SKELETONTRACKING,MB_OK | MB_ICONHAND);
        return hr;
    }

    hr = NuiImageStreamOpen(
        NUI_IMAGE_TYPE_COLOR,
        NUI_IMAGE_RESOLUTION_640x480,
        0,
        2,
        m_hNextVideoFrameEvent,
        &m_pVideoStreamHandle );
    if( FAILED( hr ) )
    {
        MessageBoxResource(m_hWnd,IDS_ERROR_VIDEOSTREAM,MB_OK | MB_ICONHAND);
        return hr;
    }

    hr = NuiImageStreamOpen(
        NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
        NUI_IMAGE_RESOLUTION_320x240,
        0,
        2,
        m_hNextDepthFrameEvent,
        &m_pDepthStreamHandle );
    if( FAILED( hr ) )
    {
        MessageBoxResource(m_hWnd,IDS_ERROR_DEPTHSTREAM,MB_OK | MB_ICONHAND);
        return hr;
    }

    // Start the Nui processing thread
    m_hEvNuiProcessStop=CreateEvent(NULL,FALSE,FALSE,NULL);
    m_hThNuiProcess=CreateThread(NULL,0,Nui_ProcessThread,this,0,NULL);


	// added by U. Marti, for gesture recognition
	
	// initialize the gesture recognition
	//
	numberOfUsedRecognizers = 0;
	rightHandLengthThreshold = 17;
	sendWindowToBackground = TRUE;
	//wcscpy(cmdDestApp, convertCharToLongchar("Video Wall - Mozilla Firefox"));
	SetDlgItemInt( m_hWnd, IDC_EDITHandLengthThreshold, rightHandLengthThreshold, 0);
	for (int i=0; i<maxNumberOfSkeletons; i++) {
		skeleton2recognizer[i] = -1;
	}

	for (int i=0; i<numberOfRecognizers; i++) {
		gestureRec[i].setRecognizerID(i);
		SetDlgItemInt( m_hWnd, IDC_EDITTriggerThresholdHigh, (int)gestureRec[i].getTriggerThresholdHigh(), 0);
		SetDlgItemInt( m_hWnd, IDC_EDITTriggerThresholdLow, (int)gestureRec[i].getTriggerThresholdLow(), 0);
		SetDlgItemInt( m_hWnd, IDC_EDITMinNumberOfSamples, (int)gestureRec[i].getMinNumberOfSamples(), 0);
		SetDlgItemInt( m_hWnd, IDC_EDITMedianFilterWidth, (int)gestureRec[i].getMedianFilterWidth(), 0);
		SetDlgItemInt( m_hWnd, IDC_EDITTimeoutThreshold, (int)gestureRec[i].getTimeoutAfterGesture(), 0);
		SetDlgItemInt( m_hWnd, IDC_EDITRejectThreshold, (int)gestureRec[i].getRejectThreshold()*100, 0);

		//SetDlgItemInt( m_hWnd, IDC_CHECKSendToBackground, 1, 0);
		//SetDlgItemText( m_hWnd, IDC_EDITCmdDestApp, cmdDestApp);
		gestureRec[i].setOutputWindow(m_hWnd, IDC_DEBUG);
		gestureRec[i].loadPatterns("gesturePatterns.txt");
	}
	oldPosition.x = 0;
	oldPosition.y = 0;
	oldPosition.z = 0;
	wgCommander.readConfiguration("gesture2window.txt");
	sendCommandsToApp = 1;
	if (sendCommandsToApp == 1) 
		SendDlgItemMessage(m_hWnd, IDC_RADIOFirefox, BM_SETCHECK, BST_CHECKED, 0);
	else if (sendCommandsToApp==2)
		SendDlgItemMessage(m_hWnd, IDC_RADIOChrome, BM_SETCHECK, BST_CHECKED, 0);
	else if (sendCommandsToApp==3)
		SendDlgItemMessage(m_hWnd, IDC_RADIOFromFile, BM_SETCHECK, BST_CHECKED, 0);

	// end of added code by U. Marti
    return hr;
}



void CSkeletalViewerApp::Nui_UnInit( )
{
    ::SelectObject( m_SkeletonDC, m_SkeletonOldObj );
    DeleteDC( m_SkeletonDC );
    DeleteObject( m_SkeletonBMP );

    if( m_Pen[0] != NULL )
    {
        DeleteObject(m_Pen[0]);
        DeleteObject(m_Pen[1]);
        DeleteObject(m_Pen[2]);
        DeleteObject(m_Pen[3]);
        DeleteObject(m_Pen[4]);
        DeleteObject(m_Pen[5]);
        ZeroMemory(m_Pen,sizeof(m_Pen));
    }

    // Stop the Nui processing thread
    if(m_hEvNuiProcessStop!=NULL)
    {
        // Signal the thread
        SetEvent(m_hEvNuiProcessStop);

        // Wait for thread to stop
        if(m_hThNuiProcess!=NULL)
        {
            WaitForSingleObject(m_hThNuiProcess,INFINITE);
            CloseHandle(m_hThNuiProcess);
        }
        CloseHandle(m_hEvNuiProcessStop);
    }

    NuiShutdown( );
    if( m_hNextSkeletonEvent && ( m_hNextSkeletonEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextSkeletonEvent );
        m_hNextSkeletonEvent = NULL;
    }
    if( m_hNextDepthFrameEvent && ( m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextDepthFrameEvent );
        m_hNextDepthFrameEvent = NULL;
    }
    if( m_hNextVideoFrameEvent && ( m_hNextVideoFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextVideoFrameEvent );
        m_hNextVideoFrameEvent = NULL;
    }
    m_DrawDepth.DestroyDevice( );
    m_DrawVideo.DestroyDevice( );
}



DWORD WINAPI CSkeletalViewerApp::Nui_ProcessThread(LPVOID pParam)
{
    CSkeletalViewerApp *pthis=(CSkeletalViewerApp *) pParam;
    HANDLE                hEvents[4];
    int                    nEventIdx,t,dt;

    // Configure events to be listened on
    hEvents[0]=pthis->m_hEvNuiProcessStop;
    hEvents[1]=pthis->m_hNextDepthFrameEvent;
    hEvents[2]=pthis->m_hNextVideoFrameEvent;
    hEvents[3]=pthis->m_hNextSkeletonEvent;

    // Main thread loop
    while(1)
    {
        // Wait for an event to be signalled
        nEventIdx=WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]),hEvents,FALSE,100);

        // If the stop event, stop looping and exit
        if(nEventIdx==0)
            break;            

        // Perform FPS processing
        t = timeGetTime( );
        if( pthis->m_LastFPStime == -1 )
        {
            pthis->m_LastFPStime = t;
            pthis->m_LastFramesTotal = pthis->m_FramesTotal;
        }
        dt = t - pthis->m_LastFPStime;
        if( dt > 1000 )
        {
            pthis->m_LastFPStime = t;
            int FrameDelta = pthis->m_FramesTotal - pthis->m_LastFramesTotal;
            pthis->m_LastFramesTotal = pthis->m_FramesTotal;
            SetDlgItemInt( pthis->m_hWnd, IDC_FPS, FrameDelta,FALSE );
        }

        // Perform skeletal panel blanking
        if( pthis->m_LastSkeletonFoundTime == -1 )
            pthis->m_LastSkeletonFoundTime = t;
        dt = t - pthis->m_LastSkeletonFoundTime;
        if( dt > 250 )
        {
            if( !pthis->m_bScreenBlanked )
            {
                pthis->Nui_BlankSkeletonScreen( GetDlgItem( pthis->m_hWnd, IDC_SKELETALVIEW ) );
                pthis->m_bScreenBlanked = true;
            }
        }

        // Process signal events
        switch(nEventIdx)
        {
            case 1:
                pthis->Nui_GotDepthAlert();
                pthis->m_FramesTotal++;
                break;

            case 2:
                pthis->Nui_GotVideoAlert();
                break;

            case 3:
                pthis->Nui_GotSkeletonAlert( );
                break;
        }
    }

    return (0);
}

void CSkeletalViewerApp::Nui_GotVideoAlert( )
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;

    HRESULT hr = NuiImageStreamGetNextFrame(
        m_pVideoStreamHandle,
        0,
        &pImageFrame );
    if( FAILED( hr ) )
    {
        return;
    }

    NuiImageBuffer * pTexture = pImageFrame->pFrameTexture;
    KINECT_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
        BYTE * pBuffer = (BYTE*) LockedRect.pBits;

        m_DrawVideo.DrawFrame( (BYTE*) pBuffer );

    }
    else
    {
        OutputDebugString( L"Buffer length of received texture is bogus\r\n" );
    }

    NuiImageStreamReleaseFrame( m_pVideoStreamHandle, pImageFrame );
}


void CSkeletalViewerApp::Nui_GotDepthAlert( )
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;

    HRESULT hr = NuiImageStreamGetNextFrame(
        m_pDepthStreamHandle,
        0,
        &pImageFrame );

    if( FAILED( hr ) )
    {
        return;
    }

    NuiImageBuffer * pTexture = pImageFrame->pFrameTexture;
    KINECT_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
        BYTE * pBuffer = (BYTE*) LockedRect.pBits;

        // draw the bits to the bitmap
        RGBQUAD * rgbrun = m_rgbWk;
        USHORT * pBufferRun = (USHORT*) pBuffer;
        for( int y = 0 ; y < 240 ; y++ )
        {
            for( int x = 0 ; x < 320 ; x++ )
            {
                RGBQUAD quad = Nui_ShortToQuad_Depth( *pBufferRun );
                pBufferRun++;
                *rgbrun = quad;
                rgbrun++;
            }
        }

        m_DrawDepth.DrawFrame( (BYTE*) m_rgbWk );
    }
    else
    {
        OutputDebugString( L"Buffer length of received texture is bogus\r\n" );
    }

    NuiImageStreamReleaseFrame( m_pDepthStreamHandle, pImageFrame );
}



void CSkeletalViewerApp::Nui_BlankSkeletonScreen(HWND hWnd)
{
    HDC hdc = GetDC( hWnd );
    RECT rct;
    GetClientRect(hWnd, &rct);
    int width = rct.right;
    int height = rct.bottom;
    PatBlt( hdc, 0, 0, width, height, BLACKNESS );
    ReleaseDC( hWnd, hdc );
}

void CSkeletalViewerApp::Nui_DrawSkeletonSegment( NUI_SKELETON_DATA * pSkel, int numJoints, ... )
{
    va_list vl;
    va_start(vl,numJoints);
    POINT segmentPositions[NUI_SKELETON_POSITION_COUNT];

    for (int iJoint = 0; iJoint < numJoints; iJoint++)
    {
        NUI_SKELETON_POSITION_INDEX jointIndex = va_arg(vl,NUI_SKELETON_POSITION_INDEX);
        segmentPositions[iJoint].x = m_Points[jointIndex].x;
        segmentPositions[iJoint].y = m_Points[jointIndex].y;
    }

    Polyline(m_SkeletonDC, segmentPositions, numJoints);

    va_end(vl);
}

void CSkeletalViewerApp::Nui_DrawSkeleton( bool bBlank, NUI_SKELETON_DATA * pSkel, HWND hWnd, int WhichSkeletonColor )
{
    HGDIOBJ hOldObj = SelectObject(m_SkeletonDC,m_Pen[WhichSkeletonColor % m_PensTotal]);
    
    RECT rct;
    GetClientRect(hWnd, &rct);
    int width = rct.right;
    int height = rct.bottom;

    if( m_Pen[0] == NULL )
    {
        m_Pen[0] = CreatePen( PS_SOLID, width / 80, RGB(255, 0, 0) );
        m_Pen[1] = CreatePen( PS_SOLID, width / 80, RGB( 0, 255, 0 ) );
        m_Pen[2] = CreatePen( PS_SOLID, width / 80, RGB( 64, 255, 255 ) );
        m_Pen[3] = CreatePen( PS_SOLID, width / 80, RGB(255, 255, 64 ) );
        m_Pen[4] = CreatePen( PS_SOLID, width / 80, RGB( 255, 64, 255 ) );
        m_Pen[5] = CreatePen( PS_SOLID, width / 80, RGB( 128, 128, 255 ) );
    }

    if( bBlank )
    {
        PatBlt( m_SkeletonDC, 0, 0, width, height, BLACKNESS );
    }

    int scaleX = width; //scaling up to image coordinates
    int scaleY = height;
    float fx=0,fy=0;
    int i;
    for (i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
    {
        NuiTransformSkeletonToDepthImageF( pSkel->SkeletonPositions[i], &fx, &fy );
        m_Points[i].x = (int) ( fx * scaleX + 0.5f );
        m_Points[i].y = (int) ( fy * scaleY + 0.5f );
		m_Points[i].z = (LONG) pSkel->SkeletonPositions[i].z*100.0; // distance to the sensor in cm
    }

    SelectObject(m_SkeletonDC,m_Pen[WhichSkeletonColor%m_PensTotal]);
    
    Nui_DrawSkeletonSegment(pSkel,4,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_HEAD);
    Nui_DrawSkeletonSegment(pSkel,5,NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);
    Nui_DrawSkeletonSegment(pSkel,5,NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
    Nui_DrawSkeletonSegment(pSkel,5,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
    Nui_DrawSkeletonSegment(pSkel,5,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);
    
    // Draw the joints in a different color
    for (i = 0; i < NUI_SKELETON_POSITION_COUNT ; i++)
    {
        HPEN hJointPen;
        
        hJointPen=CreatePen(PS_SOLID,9, g_JointColorTable[i]);
        hOldObj=SelectObject(m_SkeletonDC,hJointPen);

        MoveToEx( m_SkeletonDC, m_Points[i].x, m_Points[i].y, NULL );
        LineTo( m_SkeletonDC, m_Points[i].x, m_Points[i].y );

        SelectObject( m_SkeletonDC, hOldObj );
        DeleteObject(hJointPen);
    }


	// added by U. Marti, for gesture recognition

	// set the mouse to the appropriate position
	//MouseMoveXY(m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].x*4,m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].y*4);
	//SetDlgItemInt( m_hWnd, IDC_DEBUG, WhichSkeletonColor, TRUE);
	

	
	/*
	double rightHandHandLength = sqrt((double)
		(m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT].x-m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].x)*(m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT].x-m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].x)+
		(m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT].y-m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].y)*(m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT].y-m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].y)+
		(m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT].z-m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].z)*(m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT].z-m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].z)
		);

	if (rightHandHandLength>rightHandLengthThreshold) {
		MouseMoveXY(m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].x*4,m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].y*4);
	}*/

	userAngle = 0;
	double l1= sqrt((double)m_Points[NUI_SKELETON_POSITION_SHOULDER_CENTER].x*m_Points[NUI_SKELETON_POSITION_SHOULDER_CENTER].x+m_Points[NUI_SKELETON_POSITION_SHOULDER_CENTER].y*m_Points[NUI_SKELETON_POSITION_SHOULDER_CENTER].y+m_Points[NUI_SKELETON_POSITION_SHOULDER_CENTER].z*m_Points[NUI_SKELETON_POSITION_SHOULDER_CENTER].z);
	double l2= sqrt((double)
		(m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].x-m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x)*(m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].x-m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x)+
		(m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].y-m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y)*(m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].y-m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y)+
		(m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].z-m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT].z)*(m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].z-m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT].z)
		);
	double cross = 
		m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].x*m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].x+
		m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].y*m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].y+
		m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].z*m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT].z;
	userAngle = acos(cross/l1/l2) * 180.0 / 3.14159265;
	SetDlgItemInt( m_hWnd, IDC_EDITUserAngle, (int)userAngle, TRUE);
	SetDlgItemInt( m_hWnd, IDC_EDITRightHandDistance, m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].z, TRUE);

	// get the features and input them to the recognizer
	if (skeleton2recognizer[WhichSkeletonColor] != -1) {

		// input the right hand's position
		gestureRec[skeleton2recognizer[WhichSkeletonColor]].inputFeatures(m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].x, m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].y, m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].z, m_FramesTotal);
		// single user	-->	gestureRec[0].inputFeatures(m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].x, m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].y, m_Points[NUI_SKELETON_POSITION_HAND_RIGHT].z, m_FramesTotal);
		
		// check if a gesture was triggered!
		if (gestureRec[skeleton2recognizer[WhichSkeletonColor]].getRecognitionResult() != NULL) {
		// signle user	-->	if (gestureRec[0].getRecognitionResult() != NULL) {
		
		//
		// get the gesture to window command translation
		//
		char *gesture = new char[strlen(gestureRec[skeleton2recognizer[WhichSkeletonColor]].getRecognitionResult())+1];
		// single user	-->	char *gesture = new char[strlen(gestureRec[0].getRecognitionResult())+1];
		strcpy(gesture, gestureRec[skeleton2recognizer[WhichSkeletonColor]].getRecognitionResult());
		// single user	-->			strcpy(gesture, gestureRec[0].getRecognitionResult());
		char *window = wgCommander.getWindow(gesture);
		char *command = wgCommander.getCommand(gesture);
		//
		// get the window handle of the appropriate window
		//

		
		HWND winHandle = NULL;
		// for Firefox 3.6.34 and 7.0.1
		if (sendCommandsToApp==1) {
			//HWND winHandle2 = FindWindow(NULL, cmdDestApp);
			HWND winHandle2 = FindWindow(NULL, L"Video Wall - Mozilla Firefox");
			winHandle  = FindWindowEx(winHandle2, NULL, L"MozillaWindowClass", NULL);
			//SetDlgItemText( m_hWnd, IDC_DEBUG, L"handle on Firefox");
		} else 
		// for Google Chrome 
		if (sendCommandsToApp==2) {
			HWND winHandle2 = FindWindow(NULL, L"Video Wall - Google Chrome");
			HWND winHandle1  = FindWindowEx(winHandle2, NULL, NULL, L"Video Wall");
			winHandle  = FindWindowEx(winHandle1, NULL, L"Chrome_RenderWidgetHostHWND", NULL);
			//SetDlgItemText( m_hWnd, IDC_DEBUG, L"handle on Chrome");
		} else 
		// take from File
		if (sendCommandsToApp==3) {
			winHandle  = FindWindow(NULL, convertCharToLongchar(window));
		}
		
		

		if (winHandle == NULL) {
			SetDlgItemText( m_hWnd, IDC_DEBUG, L"handle not found");
		} else {
			//
			// set the window to foreground and send the command (first character!)
			//
			if (sendWindowToBackground)
				SetForegroundWindow(winHandle);

			if (strcmp(command, "RightArrow")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_RIGHT, 0);
			} else 
			if (strcmp(command, "LeftArrow")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_LEFT, 0);
			} else 
			if (strcmp(command, "DownArrow")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_DOWN, 0);
			} else 
			if (strcmp(command, "UpArrow")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_UP, 0);
			} else 
			if (strcmp(command, "ENTER")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_RETURN, 0);
			} else 
			if (strcmp(command, "ESC")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_ESCAPE, 0);
			} else 
			if (strcmp(command, "PageUp")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_PRIOR, 0);
			} else 
			if (strcmp(command, "PageDown")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_NEXT, 0);
			} else 
			if (strcmp(command, "F1")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F1, 0);
			} else 
			if (strcmp(command, "F2")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F2, 0);
			} else 
			if (strcmp(command, "F3")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F3, 0);
			} else 
			if (strcmp(command, "F4")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F4, 0);
			} else 
			if (strcmp(command, "F5")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F5, 0);
			} else 
			if (strcmp(command, "F6")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F6, 0);
			} else 
			if (strcmp(command, "F7")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F7, 0);
			} else 
			if (strcmp(command, "F8")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F8, 0);
			} else 
			if (strcmp(command, "F9")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F9, 0);
			} else 
			if (strcmp(command, "F10")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F10, 0);
			} else 
			if (strcmp(command, "F11")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F11, 0);
			} else 
			if (strcmp(command, "F12")==0 ) {
				PostMessage(winHandle, WM_KEYDOWN, VK_F12, 0);
			} else {
				PostMessage(winHandle, WM_CHAR, command[0], 0);
			}
		}
		// single user	-->	gestureRec[0].resetRecognitionResult();
		gestureRec[skeleton2recognizer[WhichSkeletonColor]].resetRecognitionResult();
	}

}
    return;

}




void CSkeletalViewerApp::Nui_DoDoubleBuffer(HWND hWnd,HDC hDC)
{
    RECT rct;
    GetClientRect(hWnd, &rct);
    int width = rct.right;
    int height = rct.bottom;

    HDC hdc = GetDC( hWnd );

    BitBlt( hdc, 0, 0, width, height, hDC, 0, 0, SRCCOPY );

    ReleaseDC( hWnd, hdc );

}

void CSkeletalViewerApp::Nui_GotSkeletonAlert( )
{
    NUI_SKELETON_FRAME SkeletonFrame;

    HRESULT hr = NuiSkeletonGetNextFrame( 0, &SkeletonFrame );

    bool bFoundSkeleton = true;
    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
    {
        if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
        {
			bFoundSkeleton = false;
			if ((skeleton2recognizer[i]==-1)&&(numberOfUsedRecognizers<numberOfRecognizers)) {
				skeleton2recognizer[i] = numberOfUsedRecognizers;
				SetDlgItemInt( m_hWnd, IDC_Skeleton2RecognizerI, i, TRUE);
				SetDlgItemInt( m_hWnd, IDC_Skeleton2RecognizerVal, skeleton2recognizer[i], TRUE);
				numberOfUsedRecognizers++;
				SetDlgItemInt( m_hWnd, IDC_NumOfUsedRecognizers, numberOfUsedRecognizers, TRUE);
			}
        } else {
			if ((skeleton2recognizer[i]!=-1)&&(skeleton2recognizer[i] != -1)) {
				skeleton2recognizer[i] = -1;
				SetDlgItemInt( m_hWnd, IDC_Skeleton2RecognizerI, i, TRUE);
				SetDlgItemInt( m_hWnd, IDC_Skeleton2RecognizerVal, skeleton2recognizer[i], TRUE);
				numberOfUsedRecognizers--;
				SetDlgItemInt( m_hWnd, IDC_NumOfUsedRecognizers, numberOfUsedRecognizers, TRUE);
			}
		}
    }

    // no skeletons!
    //
    if( bFoundSkeleton )
    {
		for (int i=0; i<numberOfRecognizers; i++)
			skeleton2recognizer[i] = -1;
		//numberOfUsedRecognizers = 0;
		SetDlgItemInt( m_hWnd, IDC_NumOfUsedRecognizers, numberOfUsedRecognizers, TRUE);
        return;
    }

    // smooth out the skeleton data
    NuiTransformSmooth(&SkeletonFrame,NULL);

    // we found a skeleton, re-start the timer
    m_bScreenBlanked = false;
    m_LastSkeletonFoundTime = -1;

    // draw each skeleton color according to the slot within they are found.
    //
    bool bBlank = true;
    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
    {
        if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
        {
            Nui_DrawSkeleton( bBlank, &SkeletonFrame.SkeletonData[i], GetDlgItem( m_hWnd, IDC_SKELETALVIEW ), i );
            bBlank = false;
        }
    }

    Nui_DoDoubleBuffer(GetDlgItem(m_hWnd,IDC_SKELETALVIEW), m_SkeletonDC);
}



RGBQUAD CSkeletalViewerApp::Nui_ShortToQuad_Depth( USHORT s )
{
    USHORT RealDepth = (s & 0xfff8) >> 3;
    USHORT Player = s & 7;

    // transform 13-bit depth information into an 8-bit intensity appropriate
    // for display (we disregard information in most significant bit)
    BYTE l = 255 - (BYTE)(256*RealDepth/0x0fff);

    RGBQUAD q;
    q.rgbRed = q.rgbBlue = q.rgbGreen = 0;

    switch( Player )
    {
    case 0:
        q.rgbRed = l / 2;
        q.rgbBlue = l / 2;
        q.rgbGreen = l / 2;
        break;
    case 1:
        q.rgbRed = l;
        break;
    case 2:
        q.rgbGreen = l;
        break;
    case 3:
        q.rgbRed = l / 4;
        q.rgbGreen = l;
        q.rgbBlue = l;
        break;
    case 4:
        q.rgbRed = l;
        q.rgbGreen = l;
        q.rgbBlue = l / 4;
        break;
    case 5:
        q.rgbRed = l;
        q.rgbGreen = l / 4;
        q.rgbBlue = l;
        break;
    case 6:
        q.rgbRed = l / 2;
        q.rgbGreen = l / 2;
        q.rgbBlue = l;
        break;
    case 7:
        q.rgbRed = 255 - ( l / 2 );
        q.rgbGreen = 255 - ( l / 2 );
        q.rgbBlue = 255 - ( l / 2 );
    }

    return q;
}

// added by U. Marti, for gesture recognition

void CSkeletalViewerApp::Nui_ParameterAdjustment( HWND hWnd )
{
	for (int i=0; i<numberOfRecognizers; i++) {
		gestureRec[i].setTriggerThresholdHigh(GetDlgItemInt( hWnd, IDC_EDITTriggerThresholdHigh, NULL, TRUE));
		gestureRec[i].setTriggerThresholdLow(GetDlgItemInt( hWnd, IDC_EDITTriggerThresholdLow, NULL, TRUE));
		gestureRec[i].setMinNumberOfSamples(GetDlgItemInt( hWnd, IDC_EDITMinNumberOfSamples, NULL, TRUE));
		gestureRec[i].setMedianFilterWidth(GetDlgItemInt( hWnd, IDC_EDITMedianFilterWidth, NULL, TRUE));
		gestureRec[i].setTimeoutAfterGesture(GetDlgItemInt( hWnd, IDC_EDITTimeoutThreshold, NULL, TRUE));
		gestureRec[i].setRejectThreshold(GetDlgItemInt( hWnd, IDC_EDITRejectThreshold, NULL, TRUE)/100);
	}
	rightHandLengthThreshold = GetDlgItemInt(hWnd, IDC_EDITHandLengthThreshold, NULL, TRUE);
	//sendCommandsToApp = CWnd::GetCheckedRadioButton( IDC_RADIOFirefox, IDC_RADIOFromFile );
	//CheckDlgButton(hWnd, IDC_CHECKSendToBackground, sendWindowToBackground);
	//GetDlgItemText(hWnd, IDC_EDITCmdDestApp, cmdDestApp, 255);
	/*
	SetDlgItemInt( hWnd, IDC_EDITTriggerThresholdHigh, (int)gestureRec.getTriggerThresholdHigh(), 0);
	SetDlgItemInt( hWnd, IDC_EDITTriggerThresholdLow, (int)gestureRec.getTriggerThresholdLow(), 0);
	SetDlgItemInt( hWnd, IDC_EDITMinNumberOfSamples, (int)gestureRec.getMinNumberOfSamples(), 0);
	SetDlgItemInt( m_hWnd, IDC_EDITMedianFilterWidth, (int)gestureRec.getMedianFilterWidth(), 0);
	SetDlgItemInt( m_hWnd, IDC_EDITTimeoutThreshold, (int)gestureRec.getTimeoutAfterGesture(), 0);
	SetDlgItemInt( m_hWnd, IDC_EDITRejectThreshold, (int)gestureRec.getRejectThreshold()*100, 0);
	*/
}

void CSkeletalViewerApp::Nui_SetCommandSentTo( int i )
{
	sendCommandsToApp = i;
}

void CSkeletalViewerApp::MouseMoveXY (int x, int y )
{  
  double fScreenWidth    = ::GetSystemMetrics( SM_CXSCREEN )-1; 
  double fScreenHeight  = ::GetSystemMetrics( SM_CYSCREEN )-1; 
  //double fx = x*(65535.0f/fScreenWidth);
  //double fy = y*(65535.0f/fScreenHeight);
  double fx = x*(65535.0f/fScreenWidth);
  double fy = y*(65535.0f/fScreenHeight);
  INPUT  Input={0};
  Input.type      = INPUT_MOUSE;
  Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
  Input.mi.dx = (LONG) fx;
  Input.mi.dy = (LONG)fy;
  ::SendInput(1,&Input,sizeof(INPUT));
}


void CSkeletalViewerApp::MouseMoveXYRightClick (int x, int y )
{  
  double fScreenWidth    = ::GetSystemMetrics( SM_CXSCREEN )-1; 
  double fScreenHeight  = ::GetSystemMetrics( SM_CYSCREEN )-1; 
  double fx = x*(65535.0f/fScreenWidth);
  double fy = y*(65535.0f/fScreenHeight);
  INPUT  Input={0};
  Input.type      = INPUT_MOUSE;
  Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN;
  Input.mi.dx = (LONG) fx;
  Input.mi.dy = (LONG) fy;
  ::SendInput(1,&Input,sizeof(INPUT));
}


void CSkeletalViewerApp::MouseMoveXYRelative (int x, int y )
{  
  double fScreenWidth    = ::GetSystemMetrics( SM_CXSCREEN )-1; 
  double fScreenHeight  = ::GetSystemMetrics( SM_CYSCREEN )-1; 
  double fx = x;
  double fy = y;
  INPUT  Input={0};
  Input.type      = INPUT_MOUSE;
  Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTDOWN;
  Input.mi.dx = (LONG) fx;
  Input.mi.dy = (LONG) fy;
  ::SendInput(1,&Input,sizeof(INPUT));
  Input.type      = INPUT_MOUSE;
  Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
  ::SendInput(1,&Input,sizeof(INPUT));
}
/*
void	CSkeletalViewerApp::Nui_TrainGesturePattern( HWND hWnd )
{
	SetDlgItemText( hWnd, IDC_DEBUG, L"Train Button pressed!");

	LPTSTR tmpstr[255];
	char *patternName = new char[255];
	char *filename = new char[255];
	
	GetDlgItemText( hWnd, IDC_PatternName, (LPTSTR) tmpstr, 255);
	strcpy(patternName, convertLongcharToChar((WCHAR*)&tmpstr));

	GetDlgItemText( hWnd, IDC_Filename, (LPTSTR) tmpstr, 255);
	strcpy(filename, convertLongcharToChar((WCHAR*)&tmpstr));
	//SetDlgItemText( hWnd, IDC_DEBUG, (LPTSTR)filename);
	gestureRec.train(patternName, filename);

	delete [] filename;
	delete [] patternName;

}
*/

WCHAR *CSkeletalViewerApp::convertCharToLongchar(char *text)
{
	int textlen = strlen(text)+1;
	WCHAR *tmpstr = new WCHAR[textlen];
	MultiByteToWideChar(CP_ACP,0, text, -1, tmpstr, textlen);
	return tmpstr;
}

char *CSkeletalViewerApp::convertLongcharToChar(const wchar_t *text)
{
	int textlen = WideCharToMultiByte(CP_ACP, 0, text, -1, NULL, 0, NULL, NULL);
    char* tmpstr = new char[textlen+1];
    WideCharToMultiByte(CP_ACP, 0, text, -1, tmpstr, textlen, NULL, NULL);
    return tmpstr;
}