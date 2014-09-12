var VideoWallStates = {
	"standby": {
		stateid: 1,
		issubstate: false,
		wallpapertoggle: 0,
		positiontoggle: 0,
		enterState: function(){
			// background standby
			VideoWallStates.standby.showBackground();
		
			// shelf YES (active)
			if (navigationvisible) $('#shelf').data('displaystate', 'YES');	
			$('.menuentry').prop("navigationdisabled", false);
			
			// movable elements YES
			VideoWallStates.standby.showAllElements();
			
			// clock NO
			// $('#clock').data('displaystate', 'NO');
			
			// music YES (independent of previous stage)
			/*
			if (VideoWallStates.music.issubstate) { 
				VideoWallStates.music.enterState();
				VideoWallStates.music.active = true;
			}
			*/
			// music YES (if active)
			if (VideoWallStates.music.issubstate && VideoWallStates.music.active) VideoWallStates.music.enterState();
			
			// calendar YES (independent of previous stage)
			/*
			if (VideoWallStates.calendar.issubstate) { 
				VideoWallStates.calendar.enterState();
				VideoWallStates.calendar.active = true;
			}
			*/
			// calendar YES (if active)
			if (VideoWallStates.calendar.issubstate && VideoWallStates.calendar.active) VideoWallStates.calendar.enterState();
		},
		leaveState: function(){
			// shelf NO
			$('#shelf').data('displaystate', 'NO');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// movable elements NO
			VideoWallStates.standby.hideAllElements();
			
			// clock NO
			// $('#clock').data('displaystate', 'NO');
			
			// music NO
			if (VideoWallStates.music.issubstate) VideoWallStates.music.leaveState();
			
			// calendar NO
			// $('#calendar').data('displaystate', 'NO');
			VideoWallStates.calendar.leaveState();

			// workaround to prevent current item to be clicked
			// newspaperapp.ignorenextclick = true;			
		},
		upAction: function(){
			$.keynav.activate();
		},
		pageupAction: function(){
			VideoWallStates.standby.updatePositions(true);
		},
		pagedownAction: function(){
			VideoWallStates.standby.updateWallpaper(true);
		},
		updateWallpaper: function(increase){
			if (increase) {
				VideoWallStates.standby.wallpapertoggle++;
				VideoWallStates.standby.wallpapertoggle = VideoWallStates.standby.wallpapertoggle % 6;
			} else {
				VideoWallStates.standby.wallpapertoggle--;
				if (VideoWallStates.standby.wallpapertoggle < 0) VideoWallStates.standby.wallpapertoggle = 5;
			}

			VideoWallStates.standby.showBackground();
		},
		showBackground: function(){
			var hidecursor = $('#background').hasClass('cursorinvisible');
			var wallpaper = 'wallpaper' + (VideoWallStates.standby.wallpapertoggle+1);
			$('#background').removeClass().addClass(wallpaper);
			if (hidecursor) $('#background').addClass('cursorinvisible');
		},
		showSubpage: function(){
			var hidecursor = $('#background').hasClass('cursorinvisible');
			$('#background').removeClass().addClass('subpage');
			if (hidecursor) $('#background').addClass('cursorinvisible');
		},
		updatePositions: function(increase){
			var currentposition = 'position' + (VideoWallStates.standby.positiontoggle+1);
			if (increase) {
				VideoWallStates.standby.positiontoggle++;
				VideoWallStates.standby.positiontoggle = VideoWallStates.standby.positiontoggle % 3;
			} else {
				VideoWallStates.standby.positiontoggle--;
				if (VideoWallStates.standby.positiontoggle < 0) VideoWallStates.standby.positiontoggle = 2;
			}
			var newposition = 'position' + (VideoWallStates.standby.positiontoggle+1);
		
			$('#tv1, .pictureframe').switchClass(currentposition, newposition, 1500);
		},
		showCentralElements: function(){
			$('#tv1, .pictureframe').each( function() {
				var currentleftposition = $(this).css('left');
				currentleftposition = currentleftposition.replace('px', '');
				if (currentleftposition > 1300) {
					$(this).data('displaystate', 'YES');
				} else {
					$(this).data('displaystate', 'NO');
				}
				$(this).prop("navigationdisabled", true);
			});
		},
		showAllElements: function(){
			$('#tv1, .pictureframe').each( function() {
				$(this).data('displaystate', 'YES');
				$(this).prop("navigationdisabled", true);
			});
		},
		hideAllElements: function(){
			$('#tv1, .pictureframe').each( function() {
				$(this).data('displaystate', 'NO');
				$(this).prop("navigationdisabled", true);
			});
		}
	},
	"newspaper": {
		stateid: 10, 
		issubstate: false,
		enterState: function(){
			// background standby
			VideoWallStates.standby.showBackground();
			
			// shelf YES (inactive)
			if (navigationvisible) $('#shelf').data('displaystate', 'YES');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// movable elements YES
			VideoWallStates.standby.showAllElements();
			
			// newspapers YES (active)
			$('#newspaperselection_flip').data('displaystate', 'YES');
		},
		leaveState: function(){
			// shelf NO
			$('#shelf').data('displaystate', 'NO');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// movable elements NO
			VideoWallStates.standby.hideAllElements();
			
			// newspapers NO
			$('#newspaperselection_flip').data('displaystate', 'NO');
		},
		leftAction: function(){
			$('#newspaperflip').jcoverflip('next', 1, false);
		},
		rightAction: function(){
			$('#newspaperflip').jcoverflip('previous', 1, false);
		},
		downAction: function(){
			switchToState(VideoWallStates.standby);
		},
		upAction: function(){
			switchToState(VideoWallStates.newspaper_viewing);
		},
		pageupAction: function(){
			VideoWallStates.standby.updatePositions(true);
		},
		pagedownAction: function(){
			VideoWallStates.standby.updateWallpaper(true);
		}
	},
	"newspaper_viewing": {
		stateid: 11,
		issubstate: false,		
		enterState: function(contextdata){
			// background subpage
			VideoWallStates.standby.showSubpage();
			
			// shelf NO
			$('#shelf').data('displaystate', 'NO');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// newspaper viewing YES
			$('#newspaperviewing').data('displaystate', 'YES');
			var currentnewspaperindex = $('#newspaperflip').jcoverflip('current');
			var currentnewspaper = $('#newspaperflip li:nth-child(' + (currentnewspaperindex+1) + ') img');
			var currentpages = currentnewspaper.attr('newspaperpages');
			var currentfiles = currentnewspaper.attr('newspaperfiles');	
			for (var i = 0; i < currentpages; i++) {
				var image = document.createElement('img');
				image.src = currentfiles.replace('XX', i);
				image.width = 1436;
				image.height = 2109;
				$('#newspaperreading .b-load').append(image);
			}		
			$('#newspaperreading').booklet({
				width: 2872,
				height: 2109,
				pagePadding: 0,
				pageNumbers: false,
				closed: true,
				// autoCenter: true,
				manual: false,
				keyboard: false
			});
		},
		leaveState: function(){
			// shelf NO (unchanged)
			
			// newspaper viewing NO
			$('#newspaperviewing').data('displaystate', 'NO');
			$('#newspaperreading .b-load').empty();
		},
		leftAction: function(){
			$('#newspaperreading').booklet('next');
		},
		rightAction: function(){
			$('#newspaperreading').booklet('prev');
		},
		downAction: function(){
			switchToState(VideoWallStates.newspaper);
		},
		upAction: function(){
			switchToState(VideoWallStates.standby);
		}
	},
	"photo": {
		stateid: 20,
		issubstate: false,		
		enterState: function(){
			// background subpage
			VideoWallStates.standby.showSubpage();
			
			// shelf NO
			$('#shelf').data('displaystate', 'NO');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// photo albums YES
			$('#photoalbumselection_flip').data('displaystate', 'YES');

			// photo album viewing YES
			$('#photoalbumviewing').data('displaystate', 'YES');
			VideoWallStates.photo.updateContent();
		},
		leaveState: function(){
			// shelf NO (unchanged)
			
			// photo albums NO
			$('#photoalbumselection_flip').data('displaystate', 'NO');
			
			// photo album viewing NO
			$('#photoalbumviewing').data('displaystate', 'NO');
		},
		leftAction: function(){
			$('#photoalbumflip').jcoverflip('next', 1, false);
			VideoWallStates.photo.updateContent();
		},
		rightAction: function(){
			$('#photoalbumflip').jcoverflip('previous', 1, false);
			VideoWallStates.photo.updateContent();
		},
		downAction: function(){
			switchToState(VideoWallStates.standby);
			$('#menu_photo').mouseover();
		},
		upAction: function(){
			switchToState(VideoWallStates.photo_viewing);
		},
		updateContent: function(){
			var currentalbumindex = $('#photoalbumflip').jcoverflip('current');
			var currenttext = $('#photoalbumflip li:nth-child(' + (currentalbumindex+1) + ') .title').text();
			var currentsrc = $('#photoalbumflip li:nth-child(' + (currentalbumindex+1) + ') img').attr('src');
			currentsrc = currentsrc.replace('.JPG', '.jpg').replace('preview', 'preview_ipad');
			$('#photoalbumviewing img').attr("src", currentsrc);
			$('#photoalbumdescription').text(currenttext);
		}
	},
	"photo_viewing": {
		stateid: 21,
		issubstate: false,		
		enterState: function(enteringFromRelatedState){
			// background subpage
			VideoWallStates.standby.showSubpage();
			
			// shelf NO
			$('#shelf').data('displaystate', 'NO');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// bar YES (active)
			$('#bar').data('displaystate', 'YES');	
			$('.submenuentry').prop("navigationdisabled", false);
			// $('#submenu_panorama').mouseover();
			
			// tv position is prepared for overlay (prevent flickering in photo_tv.enterState)
			$('#tv1').addClass('photooverlay');
			
			// slideshow YES
			if (enteringFromRelatedState) {
				// resume playback (independent of previous stage)
				VideoWallStates.photo_pause.leaveState();
				VideoWallStates.photo_pause.active = false;
			} else {
				// initial focus on panorama button
				$('#submenu_panorama').mouseover();
			
				var currentalbumindex = $('#photoalbumflip').jcoverflip('current');
				var currentalbumtext = $('#photoalbumflip li:nth-child(' + (currentalbumindex+1) + ') .title').text();
				var currentalbumimage = $('#photoalbumflip li:nth-child(' + (currentalbumindex+1) + ') img');
				
				var locationstring = currentalbumimage.attr('location');
				var location = eval('(' + locationstring + ')');
				var mapfile = currentalbumimage.attr('mapfile');
				
				// code to display panorama pictures as part of slideshow
				var panoramaoptions = currentalbumimage.attr('panorama');
				var panoramaoptionsarray = null;
				if (panoramaoptions) panoramaoptionsarray = eval('(' + panoramaoptions + ')');
			
				$('#slideshowviewing').empty();
				for (var i = 1; i <= 5; i++) {
					var image = document.createElement('img');
					image.src = currentalbumimage.attr('src').replace('.JPG', '.jpg').replace('preview', i);
					$('#slideshowviewing').append(image);

					var slide = $('#slideshowviewing img:nth-child('+i+')');
					slide.data('sequencenumber', i);
					slide.data('mapfile', mapfile);
					slide.data('location', location);
					slide.data('description', currentalbumtext);
						
					// code to display panorama pictures as part of slideshow
					if (panoramaoptions) {
						var currentslideindex = i-1;
						var currentslidepanorama = (panoramaoptionsarray[currentslideindex] == true || panoramaoptionsarray[currentslideindex].ispanorama == true);
						var currentslidepanoramastart = panoramaoptionsarray[currentslideindex].startposition;
						if (!currentslidepanoramastart) currentslidepanoramastart = 0;
						slide.data('ispanorama', (currentslidepanorama ? 'YES' : 'NO'));
						slide.css({left: -(currentslidepanoramastart)});
					}
				}
				$('#slideshowviewing').cycle({fx: 'fade', after: slideshowCallback});		
			}  
			
			$('#slideshowviewing').data('displaystate', 'YES');
			// $('#slideshowviewing').cycle({fx: 'fade', after: slideshowCallback, fit: true});
			// $('#slideshowviewing').cycle({fx: 'fade', after: slideshowCallback});
		},
		leaveState: function(leavingToRelatedState){
			// shelf NO (unchanged)
			
			// bar NO
			$('#bar').data('displaystate', 'NO');	
			$('.submenuentry').prop("navigationdisabled", true);
			
			// slideshow NO
			$('#slideshowviewing').data('displaystate', 'NO');
			// $('#slideshowviewing').cycle('destroy'); // destroy does not work for multiple slideshows
			// $('#slideshowviewing').empty();
			
			// leave map substate (independent of previous stage)
			VideoWallStates.photo_map.leaveState();
			VideoWallStates.photo_map.active = false;
			
			// leave tv substate (independent of previous stage)
			VideoWallStates.photo_tv.leaveState();
			VideoWallStates.photo_tv.active = false;
			// tv position is reset to previous state (prevent flickering in photo_tv.leaveState)
			$('#tv1').removeClass('photooverlay');
			
			// reset pause mode (if picture function is exited completely)
			if (!leavingToRelatedState) {
				VideoWallStates.photo_pause.leaveState();
				VideoWallStates.photo_pause.active = false;
				// pause playback (if it was started before)
				if (currentState == VideoWallStates.photo_viewing) $('#slideshowviewing').cycle('pause');
			}
		},
		leftAction: function(){

		},
		rightAction: function(){

		},
		downAction: function(){
			switchToState(VideoWallStates.photo);
		},
		upAction: function(){
			$.keynav.activate();
		}
	},
	"photo_map": {
		stateid: 22,
		issubstate: true,		
		active: false,
		enterState: function(){		
			// photo map YES
			$('#photomap').data('displaystate', 'YES');
			
			// map icon active (with focus)
			$('#submenu_map').addClass('submenu_active');
			$('#submenu_map').addClass('submenu_active_focus');
		},
		leaveState: function(){		
			// photo map NO
			$('#photomap').data('displaystate', 'NO');
			
			// map icon inactive
			$('#submenu_map').removeClass('submenu_active');
			$('#submenu_map').removeClass('submenu_active_focus');
		}
	},
	"photo_panorama": {
		stateid: 23, 
		issubstate: false,
		// the following values are overridden at the end of this document:
		stepbystep: true,
		step: 1500,
		stepduration: 400,
		totalduration: 30000,
		enterState: function(){			
			// background subpage
			VideoWallStates.standby.showSubpage();
			
			// shelf NO
			$('#shelf').data('displaystate', 'NO');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// bar YES (inactive)
			$('#bar').data('displaystate', 'YES');	
			$('.submenuentry').prop("navigationdisabled", true);
			
			// panorama YES
			// code to display panorama pictures as part of slideshow
			$('#slideshowviewing').data('displaystate', 'YES');
			$('#slideshowviewing').cycle('pause');
			
			// panorama icon navigation mode
			$('#submenu_panorama').addClass('submenu_navigating');
			
			// autostart
			if (!VideoWallStates.photo_panorama.stepbystep) {
				var currentPosition = 0-parseInt(currentslide.css('left'));
				var rightLimit = currentslide.width()-3840;
				currentslide.stop();
				var leftspeed = (VideoWallStates.photo_panorama.totalduration / rightLimit) * (rightLimit - currentPosition);
				var rightspeed = (VideoWallStates.photo_panorama.totalduration / rightLimit) * (currentPosition);
				if (leftspeed > rightspeed) {
					currentslide.animate({left: -(rightLimit)}, leftspeed, 'linear');
				} else {
					currentslide.animate({left: 0}, rightspeed, 'linear');
				}				
			}	
				
			// code for panorama plugin (requires image.width and image.height to be set explicitly)
			/*
			var currentalbumindex = $('#photoalbumflip').jcoverflip('current');
			var currentsrc = $('#photoalbumflip li:nth-child(' + (currentalbumindex+1) + ') img').attr('src');
			
			var image = document.createElement('img');
			// image.src = "jquery.panorama/img/atelier.jpg";
			// image.src = "slideshows/slideshow15/Panorama4.jpg";
			image.src = currentsrc.replace('preview', 'Panorama'+(currentslide.data('sequencenumber')));
			// image.width = "2642";
			// image.height = "375";
			// image.width = "8964";
			// image.height = "1980";
			$('#panoramaviewing').append(image);
			$('#panoramaviewing').data('displaystate', 'YES');
			
			$("#panoramaviewing img").panorama({
				viewport_width: 3840,
				speed: 100000,
				direction: 'left',
				control_display: 'no',
				mode_360: false
			});
			*/
		},
		leaveState: function(){			
			// shelf NO (unchanged)
							
			// bar NO
			$('#bar').data('displaystate', 'NO');	
			$('.submenuentry').prop("navigationdisabled", true);
			
			// panorama NO
			// code to display panorama pictures as part of slideshow
			$('#slideshowviewing').data('displaystate', 'NO');

			// panorama icon normal mode
			$('#submenu_panorama').removeClass('submenu_navigating');
			
			$('#panoramaviewing').data('displaystate', 'NO');
			$('#panoramaviewing').empty();
		},
		leftAction: function(){
			// code for panorama plugin
			// $('.panorama-control-left').click();
			
			// code for manual control
			var currentPosition = 0-parseInt(currentslide.css('left'));
			var rightLimit = currentslide.width()-3840;
			if (VideoWallStates.photo_panorama.stepbystep) {
				var newPosition = currentPosition + VideoWallStates.photo_panorama.step;
				if (newPosition > rightLimit) newPosition = rightLimit;
				currentslide.animate({left: -(newPosition)}, VideoWallStates.photo_panorama.stepduration, 'swing');
			} else {
				currentslide.stop();
				var speed = (VideoWallStates.photo_panorama.totalduration / rightLimit) * (rightLimit - currentPosition);
				currentslide.animate({left: -(rightLimit)}, speed, 'linear');
			}
		},
		rightAction: function(){
			// code for panorama plugin
			// $('.panorama-control-right').click();
			
			// code for manual control
			var currentPosition = 0-parseInt(currentslide.css('left'));
			var rightLimit = currentslide.width()-3840;
			if (VideoWallStates.photo_panorama.stepbystep) {
				var newPosition = currentPosition - VideoWallStates.photo_panorama.step;
				if (newPosition < 0) newPosition = 0;
				currentslide.animate({left: -(newPosition)}, VideoWallStates.photo_panorama.stepduration, 'swing');
			} else {
				currentslide.stop();
				var speed = (VideoWallStates.photo_panorama.totalduration / rightLimit) * (currentPosition);
				currentslide.animate({left: 0}, speed, 'linear');
			}
		},
		downAction: function(){
			currentslide.stop();
			switchToState(VideoWallStates.photo_viewing, true);
		},
		upAction: function(){
			// code for panorama plugin
			// $('.panorama-control-pause').click();
			
			// code for manual control
			currentslide.stop();
		}
	},
	"photo_tv": {
		stateid: 24,
		issubstate: true,		
		active: false,
		enterState: function(){		
			// tv YES (photooverlay)
			$('#tv1').data('displaystate', 'YES');
			// position is prepared in photo_viewing (prevent flickering if done here)
			// $('#tv1').addClass('photooverlay');
			
			// tv icon active (with focus)
			$('#submenu_tv').addClass('submenu_active');
			$('#submenu_tv').addClass('submenu_active_focus');
		},
		leaveState: function(){		
			// tv NO
			$('#tv1').data('displaystate', 'NO');
			// position is reset in photo_viewing (prevent flickering if done here)
			// $('#tv1').removeClass('photooverlay');
			
			// tv icon inactive
			$('#submenu_tv').removeClass('submenu_active');
			$('#submenu_tv').removeClass('submenu_active_focus');
		}
	},
	"photo_vod": {
		stateid: 25,
		issubstate: false, 
		enterState: function(){			
			// background subpage
			VideoWallStates.standby.showSubpage();
			
			// shelf NO
			$('#shelf').data('displaystate', 'NO');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// bar YES (inactive)
			$('#bar').data('displaystate', 'YES');	
			$('.submenuentry').prop("navigationdisabled", true);
			
			// slideshow YES
			$('#slideshowviewing').data('displaystate', 'YES');
			
			// vod YES
			$('#vodselection_flip').data('displaystate', 'YES');
			VideoWallStates.vod.updateContent();
			
			// vod appearance photomode
			$('#vodselection_flip').removeClass().addClass('photomode');
			
			// vod icon active (with focus)
			$('#submenu_vod').addClass('submenu_active_focus');
		},
		leaveState: function(leavingToRelatedState){			
			// shelf NO (unchanged)
							
			// bar NO
			$('#bar').data('displaystate', 'NO');	
			$('.submenuentry').prop("navigationdisabled", true);
			
			// slideshow NO
			$('#slideshowviewing').data('displaystate', 'NO');

			// vod NO
			$('#vodselection_flip').data('displaystate', 'NO');
			
			// vod icon inactive
			$('#submenu_vod').removeClass('submenu_active_focus');
			
			// reset pause mode (if picture function is exited completely)
			if (!leavingToRelatedState) {
				VideoWallStates.photo_pause.leaveState();
				VideoWallStates.photo_pause.active = false;
				// pause playback (if it was started before)
				if (currentState == VideoWallStates.photo_vod) $('#slideshowviewing').cycle('pause');
			}
		},
		leftAction: function(){
			VideoWallStates.vod.leftAction();
		},
		rightAction: function(){
			VideoWallStates.vod.rightAction();
		},
		downAction: function(){
			switchToState(VideoWallStates.photo_viewing, true);
		},
		upAction: function(){
			var currentvodindex = $('#vodflip').jcoverflip('current');
			var currentsrc = $('#vodflip li:nth-child(' + (currentvodindex+1) + ') img').attr('src');
			var currentformat = $('#vodflip li:nth-child(' + (currentvodindex+1) + ') img').attr('format');
			currentsrc = currentsrc.replace('.JPG', '.jpg').replace('.jpg', '-'+currentformat);
			VideoWallStates.tv.playContent(currentsrc);

			switchToState(VideoWallStates.tv);
		}
	},
	"photo_pause": {
		stateid: 26,
		issubstate: true,		
		active: false,
		enterState: function(){		
			// pause icon active (with focus)
			$('#submenu_play').addClass('submenu_active');
			$('#submenu_play').addClass('submenu_active_focus');
			
			// pause playback
			$('#slideshowviewing').cycle('pause');
		},
		leaveState: function(){			
			// pause icon inactive
			$('#submenu_play').removeClass('submenu_active');
			$('#submenu_play').removeClass('submenu_active_focus');
			
			// resume playback (if it was started before)
			if (currentState == VideoWallStates.photo_viewing) $('#slideshowviewing').cycle('resume', true);
		}
	},
	"tv": {
		stateid: 30,
		issubstate: false,
		tvplayers: [new YouTubePlayer, new FlowFlashPlayer, new HTML5Player],
		// the following values are overridden at the end of this document:
		contenttoggle: 1, // YouTubePlayer: 0; FlowFlashPlayer: 1; HTML5Player: 2 (see array above)
		defaultcontent: 'file:///S:/Working_Documents/11-INOFair/VideoWall/TV-Clips/SwisscomTVLoop.mp4',
		enterState: function(){
			// tv YES (fullscreen)
			$('#tv1').data('displaystate', 'YES');
			$('#tv1').addClass('fullscreen');
			$('.tvcontent').addClass('tvcontent_fullscreen');
		},
		leaveState: function(){
			// tv NO
			$('#tv1').data('displaystate', 'NO');
			$('#tv1').removeClass('fullscreen');
			$('.tvcontent').removeClass('tvcontent_fullscreen');	
		},
		upAction: function(){
			VideoWallStates.tv.playDefaultContent();
			switchToState(VideoWallStates.standby);
			$('#menu_tv').mouseover();
		},
		downAction: function(){
			switchToState(VideoWallStates.standby);
			$('#menu_tv').mouseover();
		},
		playDefaultContent: function(){
			VideoWallStates.tv.playContent(VideoWallStates.tv.defaultcontent);
		},
		playContent: function(clipurl){

			var html5player = document.getElementById('html5video');
			if (html5player) {
				html5player.pause();
				html5player.src = clipurl;
				html5player.loop = 'loop';
				html5player.load();
				// the .play() call might not be needed if your video tag
				// is set for autoplay but it can't hurt to call it
				html5player.play();
			}
			VideoWallStates.tv.tvplayers[2] = new HTML5Player(html5player);
			
		}
	},
	"calendar": {
		stateid: 40,
		issubstate: true,
		// the following values are overridden at the end of this document:
		active: false,
		enterState: function(){
			// calendar YES
			$('#calendar').data('displaystate', 'YES');
		},
		leaveState: function(){		
			// calendar NO
			$('#calendar').data('displaystate', 'NO');
		}
	},
	"music": {
		stateid: 50,
		musicapp: null,
		// the following values are overridden at the end of this document:
		issubstate: false,
		active: false,
		enterState: function(){
			if (!VideoWallStates.music.issubstate) {
				// basic appearance identical to standby
				VideoWallStates.standby.enterState();
				
				// shelf YES (inactive)
				if (navigationvisible) $('#shelf').data('displaystate', 'YES');	
				$('.menuentry').prop("navigationdisabled", true);
			}
			
			// music YES
			$('#musicselection').data('displaystate', 'YES');
			VideoWallStates.music.musicapp.navigationdisabled = false;
		},
		leaveState: function(){	
			if (!VideoWallStates.music.issubstate) {
				// basic appearance identical to standby
				VideoWallStates.standby.leaveState();
				
				// shelf NO
				$('#shelf').data('displaystate', 'NO');	
				$('.menuentry').prop("navigationdisabled", true);
			}
			
			// music NO
			$('#musicselection').data('displaystate', 'NO');
			VideoWallStates.music.musicapp.navigationdisabled = true;
		},
		downAction: function(){
			switchToState(VideoWallStates.standby);
		},
		upAction: function(){
			switchToState(VideoWallStates.standby);
		},
		pageupAction: function(){
			VideoWallStates.standby.updatePositions(true);
		},
		pagedownAction: function(){
			VideoWallStates.standby.updateWallpaper(true);
		}
	},
	"vod": {
		stateid: 60,
		issubstate: false,
		enterState: function(){
			// background standby
			VideoWallStates.standby.showBackground();
			
			// shelf YES (inactive)
			if (navigationvisible) $('#shelf').data('displaystate', 'YES');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// movable elements YES
			VideoWallStates.standby.showAllElements();
			
			// vod YES (active)
			$('#vodselection_flip').data('displaystate', 'YES');
			VideoWallStates.vod.updateContent();
			
			// vod appearance vod
			$('#vodselection_flip').removeClass();
			
		},
		leaveState: function(){
			// shelf NO
			$('#shelf').data('displaystate', 'NO');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// movable elements NO
			VideoWallStates.standby.hideAllElements();
			
			// vod NO
			$('#vodselection_flip').data('displaystate', 'NO');
		},
		leftAction: function(){
			$('#vodflip').jcoverflip('next', 1, false);
			VideoWallStates.vod.updateContent();
		},
		rightAction: function(){
			$('#vodflip').jcoverflip('previous', 1, false);
			VideoWallStates.vod.updateContent();
		},
		downAction: function(){
			switchToState(VideoWallStates.standby);
		},
		upAction: function(){
			var currentvodindex = $('#vodflip').jcoverflip('current');
			var currentsrc = $('#vodflip li:nth-child(' + (currentvodindex+1) + ') img').attr('src');
			var currentformat = $('#vodflip li:nth-child(' + (currentvodindex+1) + ') img').attr('format');
			currentsrc = currentsrc.replace('.JPG', '.jpg').replace('.jpg', '-'+currentformat);
			VideoWallStates.tv.playContent(currentsrc);

			switchToState(VideoWallStates.standby);
			$('#menu_tv').mouseover();
		},
		pageupAction: function(){
			VideoWallStates.standby.updatePositions(true);
		},
		pagedownAction: function(){
			VideoWallStates.standby.updateWallpaper(true);
		},
		updateContent: function(){
			var currentvodindex = $('#vodflip').jcoverflip('current');
			var currentdescription = $('#vodflip li:nth-child(' + (currentvodindex+1) + ') img').attr('description');
			$('#voddescription').text(currentdescription);
		}
	},
	"game":  {
		stateid: 70,
		issubstate: false,		
		enterState: function(){
			// background standby
			VideoWallStates.standby.showBackground();
			
			// shelf YES (inactive)
			if (navigationvisible) $('#shelf').data('displaystate', 'YES');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// movable elements YES (central only)
			VideoWallStates.standby.showCentralElements();
			
			// calendar YES (if active)
			if (VideoWallStates.calendar.active) VideoWallStates.calendar.enterState();
			
			// music YES (if active; control inactive)
			if (VideoWallStates.music.issubstate && VideoWallStates.music.active) VideoWallStates.music.enterState();
			VideoWallStates.music.musicapp.navigationdisabled = true;
			
			// game YES
			$('#game').data('displaystate', 'YES');
			tetris.start();
		},
		leaveState: function(){
			// shelf NO
			$('#shelf').data('displaystate', 'NO');	
			$('.menuentry').prop("navigationdisabled", true);
			
			// movable elements NO
			VideoWallStates.standby.hideAllElements();
			
			// calendar NO (without changing active status)
			VideoWallStates.calendar.leaveState();

			// music NO (without changing active status)
			if (VideoWallStates.music.issubstate) VideoWallStates.music.leaveState();
			VideoWallStates.music.musicapp.navigationdisabled = true;
			
			// game NO
			$('#game').data('displaystate', 'NO');
		},
		leftAction: function(){
			tetris.moveLeft();
		},
		rightAction: function(){
			tetris.moveRight();
		},
		downAction: function(){
			switchToState(VideoWallStates.standby);
			tetris.gameOver();
		},
		upAction: function(){
			tetris.rotate();
		}
	}
};

var currentState = null;
function switchToState(newstate, data) {
	if (!newstate || newstate.issubstate || currentState == newstate) return;
	if (currentState && currentState.leaveState) currentState.leaveState(data);
	currentState = newstate;
	if (currentState.enterState) currentState.enterState(data);
	
	updateVisibility();
}
function toggleSubState(substate, data) {
	if (!substate || !(substate.issubstate)) return;
	if (substate.active) {
		if (substate.leaveState) substate.leaveState();
		substate.active = false;
	} else {
		if (substate.enterState) substate.enterState(data);
		substate.active = true;
	}
	
	updateVisibility();
}
function updateVisibility() {
	// $('div').each( function() {
	$('div').not('#tv1').each( function() {
		if ($(this).data('displaystate')) { 
			if ($(this).data('displaystate') == 'YES')
				$(this).fadeIn("slow");
			else
				$(this).fadeOut("slow");
		} // do nothing if the data is not present
	});	
	
	// fading does not work for the TV set because playback of the embedded content is stopped
	if ($('#tv1').data('displaystate')) { 
		if ($('#tv1').data('displaystate') == 'YES') {
			$('#tv1').css({visibility:"visible"});
			
			var currentcontent = '#tvcontent' + (VideoWallStates.tv.contenttoggle+1);
			$(currentcontent).css({visibility:"visible"});
			$('.tvcontent').not(currentcontent).css({visibility:"hidden"});
			
			for (var i = 0; i < VideoWallStates.tv.tvplayers.length; i++) {
				if (VideoWallStates.tv.contenttoggle == i) {
					VideoWallStates.tv.tvplayers[i].play();
				} else {
					VideoWallStates.tv.tvplayers[i].pause();
				}
			}	
			
		} else {
			$('#tv1').css({visibility:"hidden"});
			$('.tvcontent').css({visibility:"hidden"});
			
			for (var i = 0; i < VideoWallStates.tv.tvplayers.length; i++) {
				VideoWallStates.tv.tvplayers[i].pause();
			}
			
		}
	}
}

jQuery(document).ready(function() {
	
    // Initialize jQuery keyboard navigation	
	$('.menuentry').keynav('menu_focus','menu_nofocus');
	$('.submenuentry').keynav('submenu_focus','submenu_nofocus');
    // $('.pictureframe img').keynav('pictureframe_focus','pictureframe_nofocus');
	
    // set the initial focus
	$('#menu_tv').removeClass('menu_nofocus').addClass('menu_focus');
	$('#submenu_map').removeClass('subsubmenu_nofocus').addClass('submenu_focus');
	// $('.pictureframe img').addClass('pictureframe_nofocus');

	// bind click event to perform some useful tasks
	$('.menuentry').bind("click", function() {
		openMenuEntry($(this));
	});
	$('.submenuentry').bind("click", function() {
		openMenuEntry($(this));
	});
	/*
	$('.menuentry').bind("focus", function() {						  
		if (!navigationvisible) {
			$('#shelf').fadeToggle("slow");
			navigationvisible = true;
		}			
	});
	*/
	
	// panorama button has different focus appearance if current slide is a panorama
	$('#submenu_panorama').bind("focus", function() {						  
		if (currentslide && currentslide.data('ispanorama') == 'YES') {
			$('#submenu_panorama').addClass('submenu_active_focus');
		}			
	});
	$('#submenu_panorama').bind("blur", function() {						  
		$('#submenu_panorama').removeClass('submenu_active_focus');	
	});
	
	// map button has different focus appearance if map is shown
	$('#submenu_map').bind("focus", function() {						  
		if (VideoWallStates.photo_map.active) {
			$('#submenu_map').addClass('submenu_active_focus');
		}	
	});
	$('#submenu_map').bind("blur", function() {						  
		$('#submenu_map').removeClass('submenu_active_focus');	
	});
	
	// play button has different focus appearance if playback is paused
	$('#submenu_play').bind("focus", function() {						  
		if (VideoWallStates.photo_pause.active) {
			$('#submenu_play').addClass('submenu_active_focus');
		}	
	});
	$('#submenu_play').bind("blur", function() {						  
		$('#submenu_play').removeClass('submenu_active_focus');	
	});
	
	// tv button has different focus appearance if map is shown
	$('#submenu_tv').bind("focus", function() {						  
		if (VideoWallStates.photo_tv.active) {
			$('#submenu_tv').addClass('submenu_active_focus');
		}	
	});
	$('#submenu_tv').bind("blur", function() {						  
		$('#submenu_tv').removeClass('submenu_active_focus');	
	});
	
	/*
	$('.pictureframe').bind("click", function() {
		switchToState(VideoWallStates.photo);
	});
	
	$('.pictureframe img').bind("focus", function() {	
		if (navigationvisible) {
			$('#shelf').fadeToggle("slow");
			navigationvisible = false;
		}								  
	});
	*/
	
	// set initial wallpaper for background
	$('#background').removeClass().addClass('wallpaper1');
	
	// set initial position of movable elements
	$('.pictureframe').removeClass().addClass('pictureframe position1');
	$('#tv1').removeClass().addClass('position1');
	
	/*
	$('.pictureframe').draggable();
	$('#tv1').draggable();
	*/
	
	VideoWallStates.tv.playDefaultContent();

	VideoWallStates.music.musicapp = initializeCoverflowApp('#musicselection', 6);
	/*
	$('#musicselection #coverflow img').bind("click", function() {
		alert($(this).attr('src'));
	});
	*/
		
	$('#musicselection').draggable();	
	
	$('#photomapdot').draggable();
	
	// photoalbum formatting specified by callback functions
	$('#photoalbumflip').jcoverflip({current:6, beforeCss:photoalbumflip_before, afterCss:photoalbumflip_after, currentCss:photoalbumflip_current, 
									change:VideoWallStates.photo.updateContent});
	
	// vod formatting formatting specified by callback functions (identical to newspaper formatting)
	$('#vodflip').jcoverflip({current:6, beforeCss:newspaperflip_before, afterCss:newspaperflip_after, currentCss:newspaperflip_current});
	
	var flip = document.createElement('ul');
	flip.setAttribute('id', 'newspaperflip');		
	for (var i = 0; i < newspaperlist.length; i++) {
		var newspaper = document.createElement('li');
		var title = document.createElement('span');
		title.setAttribute('class', 'title');
		var titletext = document.createTextNode(newspaperlist[i].title);
		title.appendChild(titletext);
		var image = document.createElement('img');
		// image.src = '../NewsPaper/20111018/DerBund-'+i+'.jpg';
		// image.src = '../NewsPaper/20111018/DerBund-0.jpg';
		image.setAttribute('newspaperfiles', newspaperlist[i].folder + newspaperlist[i].namepattern);
		image.setAttribute('newspaperpages', newspaperlist[i].pages);
		image.src = image.getAttribute('newspaperfiles').replace('XX', '0');
		newspaper.appendChild(title);
		newspaper.appendChild(image);
		flip.appendChild(newspaper);
	}
	$('#newspaperselection_flip').append(flip);
	
	// newspaper formatting specified by callback functions
	$('#newspaperflip').jcoverflip({current: (newspaperlist.length-1), beforeCss:newspaperflip_before, afterCss:newspaperflip_after, currentCss:newspaperflip_current});		
	
	
	// iPad-specific actions
	$('#photoalbumviewing img').bind("click", function() {
		VideoWallStates.photo.upAction();
	});
	$('#slideshowviewing img').live("click", function() {
		VideoWallStates.photo_viewing.upAction();
	});
	$('#leftbutton').bind("click", function() {
		var e = jQuery.Event("keydown");
		e.which = 37;
		$(document).trigger(e);
	});
	$('#downbutton').bind("click", function() {
		var e = jQuery.Event("keydown");
		e.which = 40;
		$(document).trigger(e);
	});
	$('#upbutton').bind("click", function() {
		var e = jQuery.Event("keydown");
		e.which = 38;
		$(document).trigger(e);
	});		
	$('#rightbutton').bind("click", function() {
		var e = jQuery.Event("keydown");
		e.which = 39;
		$(document).trigger(e);
	});
	$('#tv1, .pictureframe').bind("click", function() {
		if (currentState && currentState.pageupAction) currentState.pageupAction();
	});
	
	start_clocks();
		
	// call leaveState of every state to hide all elements
	for (var stateIteration in VideoWallStates) {
		if (VideoWallStates[stateIteration].leaveState) VideoWallStates[stateIteration].leaveState();
	}
	// initialize to standby state
	switchToState(VideoWallStates.standby);	 
});

// callback functions for photoalbum formatting of jcoverflow
function photoalbumflip_before (el, container, offset) {
	return [
		$.jcoverflip.animationElement(el, { left: (container.width() / 2 - 840 - 440 * offset) + 'px', bottom: '20px' }, {}),
		$.jcoverflip.animationElement(el.find('img'), { opacity: 0.5, width: '400px' }, {})
	];
}
function photoalbumflip_after (el, container, offset) {
	return [
		$.jcoverflip.animationElement(el, { left: (container.width() / 2 + 440 + 440 * offset) + 'px', bottom: '20px' }, {}),
		$.jcoverflip.animationElement(el.find('img'), { opacity: 0.5, width: '400px' }, {})
	];
}	
function photoalbumflip_current (el, container) {
	return [
		$.jcoverflip.animationElement(el, { left: (container.width() / 2 - 400) + 'px', bottom: 0 }, {}),
		$.jcoverflip.animationElement(el.find('img'), { opacity: 1, width: '800px' }, {})
	];
}

// callback function for newspaper and vod formatting of jcoverflow
function newspaperflip_before (el, container, offset) {
	return [
		$.jcoverflip.animationElement(el, { left: (container.width() / 2 - 630 - 330 * offset) + 'px', bottom: '20px' }, {}),
		$.jcoverflip.animationElement(el.find('img'), { opacity: 0.5, width: '300px' }, {})
	];
}
function newspaperflip_after (el, container, offset) {
	return [
		$.jcoverflip.animationElement(el, { left: (container.width() / 2 + 330 + 330 * offset) + 'px', bottom: '20px' }, {}),
		$.jcoverflip.animationElement(el.find('img'), { opacity: 0.5, width: '300px' }, {})
	];
}	
function newspaperflip_current (el, container) {
	return [
		$.jcoverflip.animationElement(el, { left: (container.width() / 2 - 300) + 'px', bottom: 0 }, {}),
		$.jcoverflip.animationElement(el.find('img'), { opacity: 1, width: '600px' }, {})
	];
}

var currentslide = null;
function slideshowCallback(curr,next,opts) {
	if (currentState != VideoWallStates.photo_viewing &&
		currentState != VideoWallStates.photo_vod &&
		currentState != VideoWallStates.photo_tv) return;
	
	currentslide = $('#slideshowviewing img:nth-child('+(opts.currSlide+1)+')');
			
	if (currentslide && currentslide.data('ispanorama') == 'YES') {
		$('#submenu_panorama').addClass('submenu_active');
		if ($('#submenu_panorama').is('.submenu_focus')) {
			$('#submenu_panorama').addClass('submenu_active_focus');
		} 
	} else {
		$('#submenu_panorama').removeClass('submenu_active_focus').removeClass('submenu_active');
	}

	if (currentslide && currentslide.data('mapfile')) {
		$('#photomap img').attr("src", currentslide.data('mapfile'));
		if (currentslide.data('location')) {
			$('#photomapdot').css(currentslide.data('location'));
			$('#photomapdot').css({visibility:"visible"});
		} else {
			$('#photomapdot').css({visibility:"hidden"});
		}
	}
	
	if (currentslide && currentslide.data('description')) {
		$('#submenudescription').text(currentslide.data('description'));
		$('#photomapdescription').text(currentslide.data('description'));
	}
}	
	
var navigationvisible = true;	


$(document).keydown(function(event) {
	switch (event.which) {
		case 37: // left
			if (currentState && currentState.leftAction) currentState.leftAction();
			break;
			
		case 39: // right
			if (currentState && currentState.rightAction) currentState.rightAction();
			break;
			
		case 38: // up
			if (currentState && currentState.upAction) currentState.upAction();
			break;

		case 40: // down
			if (currentState && currentState.downAction) currentState.downAction();
			break;
			
		/*
		case 13: // enter
			break;
		*/
			
		case 33: // page up
			if (currentState && currentState.pageupAction) currentState.pageupAction();
			break;
			
		case 34: // page down
			if (currentState && currentState.pagedownAction) currentState.pagedownAction();
			break;
		
		case 35: // end	
			if (VideoWallStates.tv.tvplayers[VideoWallStates.tv.contenttoggle]) {
				VideoWallStates.tv.tvplayers[VideoWallStates.tv.contenttoggle].toggle();
			}
			break;
		
		case 36: // home
			VideoWallStates.tv.contenttoggle++;
			VideoWallStates.tv.contenttoggle = VideoWallStates.tv.contenttoggle % 3;
			updateVisibility();
			break;
		
		case 46: // delete
			$('#background').toggleClass('cursorinvisible');
			break;
			
		default:
			break;
	}
});
		
function openMenuEntry($e) {
	switch ($e.attr('id')) {
		case "menu_tv": 
			switchToState(VideoWallStates.tv);
			// switchToState(VideoWallStates.standby);	
			// VideoWallStates.tv.playDefaultContent();			
			break;
			
		case "menu_newspaper":
			switchToState(VideoWallStates.newspaper);
			break;
			
		case "menu_photo":
			switchToState(VideoWallStates.photo);
			break;
			
		case "menu_dvd":
			switchToState(VideoWallStates.vod);
			break;
			
		case "menu_game":
			switchToState(VideoWallStates.game);
			break;
			
		case "menu_calendar":
			toggleSubState(VideoWallStates.calendar);
			break;
			
		case "menu_headphone":
			/*
			if (VideoWallStates.music.issubstate) {
				toggleSubState(VideoWallStates.music);
			} else {
				switchToState(VideoWallStates.music);
			}
			*/
			switchToState(VideoWallStates.standby);			
			break;
			
		case "submenu_map":
			if (currentslide && currentslide.data('mapfile')) toggleSubState(VideoWallStates.photo_map);
			break;
			
		case "submenu_play":
			toggleSubState(VideoWallStates.photo_pause);
			break;
						
		case "submenu_tv":
			toggleSubState(VideoWallStates.photo_tv);
			break;
			
		case "submenu_vod":
			switchToState(VideoWallStates.photo_vod, true);
			break;
						
		case "submenu_panorama":
			if (currentslide && currentslide.data('ispanorama') == 'YES') switchToState(VideoWallStates.photo_panorama, true);
			break;
			
		default:
			// alert($e.attr('id'));
		
	}
}
	

function onYouTubePlayerReady(playerId) {	
	var youtubeplayer = document.getElementById("youtube");
	// youtubeplayer.loadVideoById("N0m1XmvBey8");
	// youtubeplayer.loadPlaylist(["N0m1XmvBey8", "9dgSa4wmMzk"]);
	youtubeplayer.cuePlaylist(["N0m1XmvBey8", "9dgSa4wmMzk"]);
	youtubeplayer.setLoop(true);
	youtubeplayer.setPlaybackQuality("highres");
	VideoWallStates.tv.tvplayers[0] = new YouTubePlayer(youtubeplayer);
}


function YouTubePlayer(player_reference) {
	this.player = player_reference;
}
YouTubePlayer.prototype.play = function() {
	if (this.player && this.player.playVideo) this.player.playVideo();
}
YouTubePlayer.prototype.pause = function() {
	if (this.player && this.player.pauseVideo) this.player.pauseVideo();
}
YouTubePlayer.prototype.toggle = function() {
	if (this.player && this.player.getPlayerState && this.player.getPlayerState() == 1) {
		if (this.player && this.player.pauseVideo) this.player.pauseVideo();
	} else {
		if (this.player && this.player.playVideo) this.player.playVideo();
		// if (this.player && this.player.nextVideo) this.player.nextVideo();
	}
}

function FlowFlashPlayer(player_reference) {
	this.player = player_reference;
}
FlowFlashPlayer.prototype.play = function() {
	if (this.player && this.player.resume) this.player.resume();
}
FlowFlashPlayer.prototype.pause = function() {
	if (this.player && this.player.pause) this.player.pause();
}
FlowFlashPlayer.prototype.toggle = function() {
	if (this.player && this.player.getState && this.player.getState() != 3) {
		if (this.player && this.player.resume) this.player.resume();
	} else {
		if (this.player && this.player.pause) this.player.pause();
	}
}

function HTML5Player(player_reference) {
	this.player = player_reference;
}
HTML5Player.prototype.play = function() {
	if (this.player && this.player.play) this.player.play();
}
HTML5Player.prototype.pause = function() {
	if (this.player && this.player.pause) this.player.pause();
}
HTML5Player.prototype.toggle = function() {
	if (this.player && this.player.paused) {
		if (this.player && this.player.play) this.player.play();
	} else {
		if (this.player && this.player.pause) this.player.pause();
	}
}


$(function(){	

	var demo = {

		yScroll: function(wrapper, scrollable) {

			var wrapper = $(wrapper), scrollable = $(scrollable),
				loading = $('<div class="loading">Loading...</div>').appendTo(wrapper),
				internal = null,
				images	= null;
				scrollable.hide();
				images = scrollable.find('img');
				completed = 0;
				
				images.each(function(){
					if (this.complete) completed++;	
				});
				
				if (completed == images.length){
					setTimeout(function(){							
						loading.hide();
						wrapper.css({overflow: 'hidden'});						
						scrollable.slideDown('slow', function(){
							enable();	
						});					
					}, 0);	
				}
		
			
			function enable(){
				var inactiveMargin = 99,
					wrapperWidth = wrapper.width(),
					wrapperHeight = wrapper.height(),
					scrollableHeight = scrollable.outerHeight() + 2*inactiveMargin,
					wrapperOffset = 0,
					top = 0,
					lastTarget = null;

				
				wrapper.mousemove(function(e){
					lastTarget = e.target;
					wrapperOffset = wrapper.offset();		
					top = (e.pageY -  wrapperOffset.top) * (scrollableHeight - wrapperHeight) / wrapperHeight - inactiveMargin;
					if (top < 0){
						top = 0;
					}			
					wrapper.scrollTop(top);
				});	
			}			
		}
	}

	demo.yScroll('#scroll-pane', 'ul#sortable');
		
});


function initializeCoverflowApp(appid, initialItem) {	

	var coverflowApp = coverflowApp || {};
	
    coverflowApp = {
		navigationdisabled: false,
		ignorenextclick: false,
	
        // defaultItem: 6,
		defaultItem: initialItem,
        //default set item to be centered on
        defaultDuration: 1200,
        //animation duration
        html: $('#demo-frame div.wrapper').html(),
		
		// VideoWall: appid not hardcoded but given by function parameter
		/*
        imageCaption: $('.demo #imageCaption'),
        sliderCtrl: $('.demo #slider'),
        coverflowCtrl: $('.demo #coverflow'),
        coverflowImages: $('.demo #coverflow').find('img'),
        coverflowItems: $('.demo .coverflowItem'),
        sliderVertical: $(".demo #slider-vertical"),
		*/
		imageCaption: $(appid + ' #imageCaption'),
        sliderCtrl: $(appid + ' #slider'),
        coverflowCtrl: $(appid + ' #coverflow'),
        coverflowImages: $(appid + ' #coverflow').find('img'),
        coverflowItems: $(appid + ' .coverflowItem'),
        sliderVertical: $(appid + " #slider-vertical"),
		
		
        origSliderHeight: '',
        sliderHeight: '',
        sliderMargin: '',
        difference: '',
        proportion: '',
        handleHeight: '',

        listContent: "",


        artist: "",
        album: "",
		// sortable: $('#sortable'),
		// scrollPane: $('#scroll-pane'),
        sortable: $(appid + ' #sortable'),
        scrollPane: $(appid + ' #scroll-pane'),

        setDefault: function () {
            this.defaultItem -= 1;
            // $('.coverflowItem').eq(this.defaultItem).addClass('ui-selected');
			$(appid + ' .coverflowItem').eq(this.defaultItem).addClass('ui-selected');
        },

        setCaption: function (caption) {
            this.imageCaption.html(caption);
        },

        init_coverflow: function (elem) {

            this.setDefault();
            this.coverflowCtrl.coverflow({
                item: coverflowApp.defaultItem,
                duration: 1200,
                select: function (event, sky) {
                    coverflowApp.skipTo(sky.value);
                }
            });

            //
            this.coverflowImages.each(function (index, value) {
                var current = $(this);
                try {
                    coverflowApp.listContent += "<li class='ui-state-default coverflowItem' data-itemlink='" + (index) + "'>" + current.data('artist') + " - " + current.data('album') + "</li>";
				} catch (e) {}
            });

            //Skip all controls to the current default item
            this.coverflowItems = this.getItems();
            this.sortable.html(this.listContent);
            this.skipTo(this.defaultItem);


            //
            this.init_slider(this.sliderCtrl, 'horizontal');
            //this.init_slider($("#slider-vertical"), 'vertical');
            //change the main div to overflow-hidden as we can use the slider now
            this.scrollPane.css('overflow', 'hidden');

            //calculate the height that the scrollbar handle should be
            this.difference = this.sortable.height() - this.scrollPane.height(); //eg it's 200px longer 
            this.proportion = this.difference / this.sortable.height(); //eg 200px/500px
            this.handleHeight = Math.round((1 - this.proportion) * this.scrollPane.height()); //set the proportional height
            ///
            this.setScrollPositions(this.defaultItem);

            //
            this.origSliderHeight = this.sliderVertical.height();
            this.sliderHeight = this.origSliderHeight - this.handleHeight;
            this.sliderMargin = (this.origSliderHeight - this.sliderHeight) * 0.5;

            //
            this.init_mousewheel();
            this.init_keyboard();
            this.sortable.selectable({
                stop: function () {
                    var result = $("#select-result").empty();
                    $(".ui-selected", this).each(function () {
                        var index = $("#sortable li").index(this);
                        coverflowApp.skipTo(index);
                    });
                }
            });


        },

        init_slider: function (elem, direction) {
            if (direction == 'horizontal') {
                elem.slider({
                    min: 0,
                    max: $(appid + ' #coverflow > *').length - 1,
                    value: coverflowApp.defaultItem,
                    slide: function (event, ui) {

                        // var current = $('.coverflowItem');
						var current = $(appid + ' .coverflowItem');
                        coverflowApp.coverflowCtrl.coverflow('select', ui.value, true);						
                        current.removeClass('ui-selected');
                        current.eq(ui.value).addClass('ui-selected');
                        coverflowApp.setCaption(current.eq(ui.value).html());
                    }
                })
            } else {
                if (direction == 'vertical') {
                    elem.slider({
                        orientation: direction,
                        range: "max",
                        min: 0,
                        max: 100,
                        value: 0,
                        slide: function (event, ui) {
                            console.log('aaa');
                            var topValue = -((100 - ui.value) * coverflowApp.difference / 100);
                            coverflowApp.sortable.css({
                                top: topValue
                            });
                        }
                    })
                }
            }
        },

        getItems: function () {
            // var refreshedItems = $('.demo .coverflowItem');
			var refreshedItems = $(appid + ' .coverflowItem');
            return refreshedItems;
        },

        skipTo: function (itemNumber) {

            // var items = $('.coverflowItem');
			var items = $(appid + ' .coverflowItem');
            this.sliderCtrl.slider("option", "value", itemNumber);
            this.coverflowCtrl.coverflow('select', itemNumber, true);
            items.removeClass('ui-selected');
            items.eq(itemNumber).addClass('ui-selected');
			this.setCaption(items.eq(itemNumber).html());
			
			// VideoWall: trigger custom event
			var event = jQuery.Event('coverflowselect');
			$(appid + ' #coverflow img:nth-child(' + (itemNumber+1) + ')').trigger(event);
        },

        init_mousewheel: function () {
            $('body').mousewheel(function (event, delta) {

                var speed = 1,
                    sliderVal = coverflowApp.sliderCtrl.slider("value"),
                    coverflowItem = 0,
                    cflowlength = $(appid + ' #coverflow > *').length - 1,
                    leftValue = 0;

                //check the deltas to find out if the user has scrolled up or down 
                if (delta > 0 && sliderVal > 0) {
                    sliderVal -= 1;
                } else {
                    if (delta < 0 && sliderVal < cflowlength) {
                        sliderVal += 1;
                    }
                }

                leftValue = -((100 - sliderVal) * coverflowApp.difference / 100); //calculate the content top from the slider position
                if (leftValue > 0) leftValue = 0; //stop the content scrolling down too much
                if (Math.abs(leftValue) > coverflowApp.difference) leftValue = (-1) * coverflowApp.difference; //stop the content scrolling up beyond point desired
				coverflowItem = Math.floor(sliderVal);
                coverflowApp.skipTo(coverflowItem);

            });
        },

        init_keyboard: function () {
			
            $(document).keydown(function (e) {
			
				// VideoWall: ignore commands if coverflowApp is disabled
				if (coverflowApp.navigationdisabled) return;
			
                var current = coverflowApp.sliderCtrl.slider('value');
				/*
                if (e.keyCode == 37) {
                    if (current > 0) {
                        current--;
                        coverflowApp.skipTo(current);
                    }
                } else {
                    if (e.keyCode == 39) {
                        if (current < $(appid + ' #coverflow > *').length - 1) {
                            current++;
                            coverflowApp.skipTo(current);
                        }
                    }
                } 
				*/
				
				// VideoWall: inverse left/right and add enter/up
				switch (e.keyCode) {
					case 39: // right
						if (current > 0) {
							current--;
							coverflowApp.skipTo(current);
						}
						break;
					case 37: // left
						if (current < $(appid + ' #coverflow > *').length - 1) {
                            current++;
                            coverflowApp.skipTo(current);
                        }
						break;
					case 13: // enter
					case 38: // up
						if (coverflowApp.ignorenextclick) {
							coverflowApp.ignorenextclick = false;
							break;
						} 
						$(appid + ' #coverflow img:nth-child(' + (current+1) + ')').click();
						break;
					default:
						break;
				}

            })
			
        },

        generateList: function () {
            this.coverflowImages.each(function (index, value) {
                var t = $(this);
                try {
                    listContent += "<li class='ui-state-default coverflowItem' data-itemlink='" + (index) + "'>" + t.data('artist') + " - " + t.data('album') + "</li>";
                } catch (e) {}
            })
        },


        setScrollPositions: function () {
            // $('#slider-vertical').slider('value', this.item * 5);
			$(appid + ' #slider-vertical').slider('value', this.item * 5);
            this.sortable.css('top', -this.item * 5 + -35);
        },

        handleScrollpane: function () {
            this.scrollPane.css('overflow', 'hidden');

            //calculate the height that the scrollbar handle should be
            difference = this.sortable.height() - this.scrollPane.height(); //eg it's 200px longer 
            proportion = difference / this.sortable.height(); //eg 200px/500px
            handleHeight = Math.round((1 - proportion) * this.scrollPane.height()); //set the proportional height
        }
    };


    coverflowApp.init_coverflow();

	return coverflowApp;
}


function start_clocks() {
    increment_clocks();
}
function increment_clocks() {
	var now = new Date();
	var hours = now.getHours();
	hours = (hours < 10) ? "0" + hours : hours + "";
	/*
	var ampm = (hours < 12) ? "am" : "pm"
	hours = (hours > 12) ? (hours - 12) + "" : hours + ""
	hours = (hours == "0") ? "12" : hours
	hours = (hours < 10) ? "0" + hours : hours + ""
	*/
	var minutes = now.getMinutes();
	minutes = (minutes < 10) ? "0" + minutes : minutes + "";
	var seconds = now.getSeconds();
	seconds = (seconds < 10) ? "0" + seconds : seconds + "";
	// var divider = (seconds % 2) ? "&nbsp" : ":";
	var divider = ":";
	// $('#digitalclock_berne').html(hours+':'+minutes+':'+seconds);
	// $('#digitalclock_berne').html(hours+divider+minutes);
	$('#berne_hours').html(hours);
	$('#berne_divider').html(divider);
	$('#berne_minutes').html(minutes);

	now.setDate(now.getDate()+2);
	var day = now.getDate();
	day = (day < 10) ? "0" + day : day + "";
	var month = now.getMonth() + 1;
	month = (month < 10) ? "0" + month : month + "";	
	var year = now.getFullYear();
	$('#todayplus2').html(day+'.'+month+'.'+year+', 14:00');

	now.setDate(now.getDate()+2);
	day = now.getDate();
	day = (day < 10) ? "0" + day : day + "";
	month = now.getMonth() + 1;
	month = (month < 10) ? "0" + month : month + "";	
	year = now.getFullYear();
	$('#todayplus4').html(day+'.'+month+'.'+year+', 20:30');
	
	now.setHours(now.getHours()-6);
	hours = now.getHours();
	hours = (hours < 10) ? "0" + hours : hours + "";
	minutes = now.getMinutes();
	minutes = (minutes < 10) ? "0" + minutes : minutes + "";
	seconds = now.getSeconds();
	seconds = (seconds < 10) ? "0" + seconds : seconds + "";
	// $('#digitalclock_ny').html(hours+':'+minutes+':'+seconds);
	// $('#digitalclock_ny').html(hours+divider+minutes);
	$('#ny_hours').html(hours);
	$('#ny_divider').html(divider);
	$('#ny_minutes').html(minutes)
	
	setTimeout("increment_clocks()", 1000);
}
  
  
    
// CONFIG VALUES:
VideoWallStates.photo_panorama.stepbystep = true;
VideoWallStates.photo_panorama.step = 1500;
VideoWallStates.photo_panorama.stepduration = 400;
VideoWallStates.photo_panorama.totalduration = 30000;
VideoWallStates.calendar.active = true;
VideoWallStates.music.issubstate = false;
VideoWallStates.music.active = true;
VideoWallStates.tv.contenttoggle = 2;
// VideoWallStates.tv.defaultcontent = 'file:///S:/Working_Documents/11-INOFair/VideoWall/TV-Clips/SwisscomTVLoop.mp4';
// VideoWallStates.tv.defaultcontent = 'file:///C:/Users/Demo/Desktop/VideoWall/TV-Clips/SwisscomTVLoop.mp4';
VideoWallStates.tv.defaultcontent = '../videowall_content/tvclips_ipad/SwisscomTVLoop.mp4';