function simulateKeyPress(keycode) {
  jQuery.event.trigger({ type:'keydown', which:keycode });
}

/* key codes as used in 
$(document).keydown(...)
in videowall.js
*/
var KEY = {
	LEFT : 37,  
	RIGHT: 39,
	UP : 38,
	DOWN: 40,
	PAGEUP: 33,
	PAGEDOWN: 34,
	DELETE: 46
};

var paused = true;
var currentaction = 0;
var actionsequence = [
	// toggle wallpapers and object positions (action 0)
	{key: KEY.PAGEDOWN, wait: 2000}, 
	{key: KEY.PAGEDOWN, wait: 2000}, 
	{key: KEY.PAGEUP, wait: 2000}, 
	{key: KEY.PAGEDOWN, wait: 2000}, 
	{key: KEY.PAGEDOWN, wait: 2000},
	{key: KEY.PAGEUP, wait: 2000}, 	
	{key: KEY.PAGEDOWN, wait: 2000}, 
	{key: KEY.PAGEDOWN, wait: 2000}, 
	{key: KEY.PAGEUP, wait: 2000}, 

	// from tv to newspaper (action 9)
	{key: KEY.RIGHT, wait: 1500}, 
	{key: KEY.RIGHT, wait: 1500}, 
	
	// newspaper
	{key: KEY.UP, wait: 3000}, 
	{key: KEY.UP, wait: 3000}, 
	{key: KEY.LEFT, wait: 3000},
	{key: KEY.LEFT, wait: 3000},
	{key: KEY.LEFT, wait: 3000},
	{key: KEY.UP, wait: 3000}, 

	// from newspaper to calendar and back
	{key: KEY.RIGHT, wait: 1500}, 
	{key: KEY.UP, wait: 3000},
	{key: KEY.UP, wait: 1500},
	{key: KEY.LEFT, wait: 1500}, 
	
	// from newspaper to tv
	{key: KEY.LEFT, wait: 1500}, 
	{key: KEY.LEFT, wait: 1500}, 
	// from tv to photo (action 23)
	{key: KEY.LEFT, wait: 1500}, 
	
	// photo (navigate to album)
	{key: KEY.UP, wait: 3000}, 
	{key: KEY.RIGHT, wait: 3000},
	{key: KEY.RIGHT, wait: 3000},
	{key: KEY.RIGHT, wait: 3000},
	{key: KEY.RIGHT, wait: 3000},
	{key: KEY.RIGHT, wait: 3000},
	{key: KEY.RIGHT, wait: 3000},
	// photo (wait for panoramic picture)
	{key: KEY.UP, wait: 12000, cleanup: cleanupPhotoAlbums}, 
	// photo (navigate in panoramic picture)
	{key: KEY.UP, wait: 3000}, 
	{key: KEY.LEFT, wait: 2000}, 
	{key: KEY.LEFT, wait: 2000}, 
	{key: KEY.LEFT, wait: 2000}, 
	{key: KEY.LEFT, wait: 2000}, 
	{key: KEY.LEFT, wait: 2000}, 
	{key: KEY.LEFT, wait: 2000}, 
	{key: KEY.LEFT, wait: 2000}, 
	{key: KEY.LEFT, wait: 2000}, 
	{key: KEY.LEFT, wait: 2000},
	{key: KEY.LEFT, wait: 2000},
	{key: KEY.UP, wait: 3000}, 	
	// photo map
	{key: KEY.LEFT, wait: 1500}, 
	{key: KEY.LEFT, wait: 1500}, 
	{key: KEY.LEFT, wait: 1500}, 
	{key: KEY.UP, wait: 4000}, 
	// photo tv
	{key: KEY.RIGHT, wait: 2000}, 
	{key: KEY.UP, wait: 4000},
	{key: KEY.UP, wait: 2000}, 	
	// {key: KEY.UP, wait: 1500},
	// photo vod
	{key: KEY.RIGHT, wait: 2000},
	{key: KEY.UP, wait: 6000},
	
	// vod fullscreen playback (action 53)
	{key: KEY.UP, wait: 40000},
	{key: KEY.UP, wait: 3000},
];


$(document).keydown(function(event) {
	switch (event.which) {
		case 80: // p (play/pause)
			if (paused) {
				paused = false;
				next_action();
			} else {
				paused = true;
			}
			break;
		case 82: // r (reset)
			// paused = true;
			currentaction = 0;
			resetToInitialPositions();
			break;
	}
});


function cleanupPhotoAlbums() {
	// set initial position of cover flow element
	$('#photoalbumflip').jcoverflip('current', 6);
	VideoWallStates.photo.updateContent();
}
function resetToInitialPositions() {	
	// set initial position of movable elements
	VideoWallStates.standby.positiontoggle = 0;
	$('.pictureframe').removeClass().addClass('pictureframe position1');
	$('#tv1').removeClass().addClass('position1');
	
	// set initial positions of cover flow elements
	cleanupPhotoAlbums();
	$('#vodflip').jcoverflip('current', 6);
	VideoWallStates.vod.updateContent();
	$('#newspaperflip').jcoverflip('current', newspaperlist.length-1);

	// initialize to standby state
	VideoWallStates.standby.wallpapertoggle = 0;
	VideoWallStates.calendar.active = true;
	VideoWallStates.tv.contenttoggle = 1;
	VideoWallStates.tv.playDefaultContent();
	if (currentState == VideoWallStates.standby) {	
		// explicit call in case there is no switch of state	
		VideoWallStates.standby.enterState();
		updateVisibility(); 
	} else {
		switchToState(VideoWallStates.standby);	
	}
	$('#menu_tv').mouseover();
}

function next_action() {
	if (paused) return;
	var action = actionsequence[currentaction];
	currentaction = (currentaction+1) % actionsequence.length;
	if (action.key) simulateKeyPress(action.key);
	if (action.cleanup) action.cleanup();
	if (action.wait) window.setTimeout("next_action()", action.wait);
}

