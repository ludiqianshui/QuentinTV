/************************************************************************
*                                                                       *
*   SkeletalViewer.h -- Declares of CSkeletalViewerApp class            *
*                                                                       *
* Copyright (c) Microsoft Corp. All rights reserved.                    *
*                                                                       *
* This code is licensed under the terms of the                          *
* Microsoft Kinect for Windows SDK (Beta) from Microsoft Research       *
* License Agreement: http://research.microsoft.com/KinectSDK-ToU        *
*                                                                       *
************************************************************************/

#pragma once

#include "resource.h"
#include "MSR_NuiApi.h"
#include "DrawDevice.h"

// added by U. Marti, for gesture recognition
#include "GestureRecognizer.h"
#include "WindowGestureCommander.h"


#define SZ_APPDLG_WINDOW_CLASS        _T("SkeletalViewerAppDlgWndClass")

// added by U. Marti, for gesture recognition
typedef struct tagPOINT3D
{
    LONG  x;
    LONG  y;
	LONG  z;
} POINT3D;


class CSkeletalViewerApp
{
public:
    HRESULT Nui_Init();
    void                    Nui_UnInit( );
    void                    Nui_GotDepthAlert( );
    void                    Nui_GotVideoAlert( );
    void                    Nui_GotSkeletonAlert( );
    void                    Nui_Zero();
    void                    Nui_BlankSkeletonScreen( HWND hWnd );
    void                    Nui_DoDoubleBuffer(HWND hWnd,HDC hDC);
    void                    Nui_DrawSkeleton( bool bBlank, NUI_SKELETON_DATA * pSkel, HWND hWnd, int WhichSkeletonColor );
    void                    Nui_DrawSkeletonSegment( NUI_SKELETON_DATA * pSkel, int numJoints, ... );

    RGBQUAD                 Nui_ShortToQuad_Depth( USHORT s );

    static LONG CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HWND m_hWnd;

	// added by U. Marti, for gesture recognition
	POINT3D					oldPosition;
	void					Nui_TrainGesturePattern( HWND hWnd ); // added by U. Marti
	void					Nui_ParameterAdjustment( HWND hWnd ); // added by U. Marti
	void					Nui_SetCommandSentTo( int i );
	void					MouseMoveXY(int x, int y);
	void					MouseMoveXYRightClick (int x, int y );
	void					MouseMoveXYRelative (int x, int y );
	int						getDepthValue(int x, int y);
	int			sendCommandsToApp; // 1 = Firefox, 2 = Chrome 3 = Take from File

private:
    static DWORD WINAPI     Nui_ProcessThread(LPVOID pParam);


    // NUI and draw stuff
    DrawDevice    m_DrawDepth;
    DrawDevice    m_DrawVideo;

    // thread handling
    HANDLE        m_hThNuiProcess;
    HANDLE        m_hEvNuiProcessStop;

    HANDLE        m_hNextDepthFrameEvent;
    HANDLE        m_hNextVideoFrameEvent;
    HANDLE        m_hNextSkeletonEvent;
    HANDLE        m_pDepthStreamHandle;
    HANDLE        m_pVideoStreamHandle;
    HFONT         m_hFontFPS;
    HPEN          m_Pen[6];
    HDC           m_SkeletonDC;
    HBITMAP       m_SkeletonBMP;
    HGDIOBJ       m_SkeletonOldObj;
    int           m_PensTotal;
    POINT3D       m_Points[NUI_SKELETON_POSITION_COUNT];
    RGBQUAD       m_rgbWk[640*480];
    int           m_LastSkeletonFoundTime;
    bool          m_bScreenBlanked;
    int           m_FramesTotal;
    int           m_LastFPStime;
    int           m_LastFramesTotal;

	// added by U. Marti, for gesture recognition
	double       userAngle;
	bool		 sendWindowToBackground;
	int          rightHandLengthThreshold;
	
	static const int numberOfRecognizers = 2;
	static const int maxNumberOfSkeletons = 20;
	int numberOfUsedRecognizers;
	int skeleton2recognizer[maxNumberOfSkeletons];
	wchar_t cmdDestApp[256];
	GestureRecognizer gestureRec[numberOfRecognizers];
	WindowGestureCommander wgCommander;
	

	WCHAR *convertCharToLongchar(char *text);
	char *convertLongcharToChar(const wchar_t *text);
};

int MessageBoxResource(HWND hwnd,UINT nID,UINT nType);



