/**
 * service.js
 *
 * Computer Science 50
 * Problem Set 8
 *
 * Implements a shuttle service.
 */
 
// timer
var time = 60;

// points
var points = 0;

// minus point
var minPoints = 0;

// interval which runs every second
var counter = setInterval(timer, 1000); 

// default height
var HEIGHT = 0.8;

// default latitude
var LATITUDE = 42.3745615030193;

// default longitude
var LONGITUDE = -71.11803936751632;

// default heading
var HEADING = 1.757197490907891;

// default number of seats
var SEATS = 10;

// default velocity
var VELOCITY = 50;

// global reference to shuttle's marker on 2D map
var bus = null;

// global reference to 3D Earth
var earth = null;

// global reference to 2D map
var map = null;

// global reference to shuttle
var shuttle = null;

// load version 1 of the Google Earth API
google.load("earth", "1");

// load version 3 of the Google Maps API
google.load("maps", "3", {other_params: "sensor=false"});

// once the window has loaded
$(window).load(function() {

    // listen for keydown anywhere in body
    $(document.body).keydown(function(event) {
        return keystroke(event, true);
    });

    // listen for keyup anywhere in body
    $(document.body).keyup(function(event) {
        return keystroke(event, false);
    });

    // listen for click on Drop Off button
    $("#dropoff").click(function(event) {
        dropoff();
    });

    // listen for click on Pick Up button
    $("#pickup").click(function(event) {
        pickup();
    });

    // load application
    load();
});

// unload application
$(window).unload(function() {
    unload();
});

/**
 * Renders seating chart.
 */
function chart()
{
    var html = "<ol start='0'>";
    for (var i = 0; i < shuttle.seats.length; i++)
    {
        if (shuttle.seats[i] == null)
        {
            html += "<li>Empty Seat</li>";
        }
        else
        {
            html += "<li>" + shuttle.seats[i].name + ", "+ shuttle.seats[i].house +"</li>";
        }
    }
    html += "</ol>";
    $("#chart").html(html);
}

/**
 * Drops up passengers if their stop is nearby.
 */
function dropoff()
{
    var shuttleLL;
    var dropoff = false;
	
	// look through all the seats
	for(var i  = 0; i < shuttle.seats.length; i++)
	{
		// if a seat is not empty
		if(shuttle.seats[i] != null)
		{
			// check shuttle distance versus passengers house
			shuttleLL = shuttle.distance(HOUSES[shuttle.seats[i].house].lat, HOUSES[shuttle.seats[i].house].lng);
			if(shuttleLL < 30)
			{
				shuttle.seats[i] = null;
				chart();
				dropoff = true;
				points++;

				// if all passengers hav been dropped of display the final score
				if(points+minPoints == 102)
				{
					$("#announcements").html("Every passenger has been dropped of !, final score is "+points);
				}
				else
				{
					$("#announcements").html("Your score is : "+points);
				}
			}
		}
	}
	
	// if there are no passengers dropped of here, displat this
	if(!dropoff)
	{
		$("#announcements").html("No passenger drop offs here..");
	}
}


/**
 * Called if Google Earth fails to load.
 */
function failureCB(errorCode) 
{
    // report error unless plugin simply isn't installed
    if (errorCode != ERR_CREATE_PLUGIN)
    {
        alert(errorCode);
    }
}

/**
 * Handler for Earth's frameend event.
 */
function frameend() 
{
    shuttle.update();
}

/**
 * Called once Google Earth has loaded.
 */
function initCB(instance) 
{
    // retain reference to GEPlugin instance
    earth = instance;

    // specify the speed at which the camera moves
    earth.getOptions().setFlyToSpeed(100);

    // show buildings
    earth.getLayerRoot().enableLayerById(earth.LAYER_BUILDINGS, true);

    // disable terrain (so that Earth is flat)
    earth.getLayerRoot().enableLayerById(earth.LAYER_TERRAIN, false);

    // prevent mouse navigation in the plugin
    earth.getOptions().setMouseNavigationEnabled(false);

    // instantiate shuttle
    shuttle = new Shuttle({
        heading: HEADING,
        height: HEIGHT,
        latitude: LATITUDE,
        longitude: LONGITUDE,
        planet: earth,
        seats: SEATS,
        velocity: VELOCITY
    });

    // synchronize camera with Earth
    google.earth.addEventListener(earth, "frameend", frameend);

    // synchronize map with Earth
    google.earth.addEventListener(earth.getView(), "viewchange", viewchange);

    // update shuttle's camera
    shuttle.updateCamera();

    // show Earth
    earth.getWindow().setVisibility(true);

    // render seating chart
    chart();

    // populate Earth with passengers and houses
    populate();
}

/**
 * Handles keystrokes.
 */
function keystroke(event, state)
{
    // ensure we have event
    if (!event)
    {
        event = window.event;
    }
    
    // when shutlle is moved, display standard message
    $("#announcements").html("No announcements at this time.");

	// page up
	if (event.keyCode == 33)
    {
        shuttle.velocity += 1;
        return false;
    }
    
    // page down
    if (event.keyCode == 34)
    {
        shuttle.velocity -= 1;
        return false;
    }

    // left arrow
    if (event.keyCode == 37)
    {
        shuttle.states.turningLeftward = state;
        return false;
    }

    // up arrow
    else if (event.keyCode == 38)
    {
        shuttle.states.tiltingUpward = state;
        return false;
    }

    // right arrow
    else if (event.keyCode == 39)
    {
        shuttle.states.turningRightward = state;
        return false;
    }

    // down arrow
    else if (event.keyCode == 40)
    {
        shuttle.states.tiltingDownward = state;
        return false;
    }

    // A, a
    else if (event.keyCode == 65 || event.keyCode == 97)
    {
        shuttle.states.slidingLeftward = state;
        return false;
    }

    // D, d
    else if (event.keyCode == 68 || event.keyCode == 100)
    {
        shuttle.states.slidingRightward = state;
        return false;
    }
  
    // S, s
    else if (event.keyCode == 83 || event.keyCode == 115)
    {
        shuttle.states.movingBackward = state;     
        return false;
    }

    // W, w
    else if (event.keyCode == 87 || event.keyCode == 119)
    {
        shuttle.states.movingForward = state;    
        return false;
    }
  
    return true;
}

/**
 * Loads application.
 */
function load()
{
    // embed 2D map in DOM
    var latlng = new google.maps.LatLng(LATITUDE, LONGITUDE);
    map = new google.maps.Map($("#map").get(0), {
        center: latlng,
        disableDefaultUI: true,
        mapTypeId: google.maps.MapTypeId.ROADMAP,
        scrollwheel: false,
        zoom: 17,
        zoomControl: true
    });

    // prepare shuttle's icon for map
    bus = new google.maps.Marker({
        icon: "https://maps.gstatic.com/intl/en_us/mapfiles/ms/micons/bus.png",
        map: map,
        title: "you are here"
    });

    // embed 3D Earth in DOM
    google.earth.createInstance("earth", initCB, failureCB);
}

/**
 * Picks up nearby passengers.
 */
function pickup()
{
    var shuttleLL;
    var features = earth.getFeatures();
    var pickup = false;
    
    // if there is a seat available proceed
	if(getSeat() != "full")
	{
		// check all passengers
		for(var i  = 0; i < PASSENGERS.length; i++)
		{
			// house of the passenger must be present in HOUSES and have a placemark
			if(HOUSES[PASSENGERS[i].house] != null && PASSENGERS[i].placemark != null )
			{
				// calculate distance between shuttle and passenger
				shuttleLL = shuttle.distance(PASSENGERS[i].placemark.getGeometry().getLatitude(), PASSENGERS[i].placemark.getGeometry().getLongitude());
				if(shuttleLL < 15.0)
				{
					shuttle.seats[getSeat()] = PASSENGERS[i];
					features.removeChild(PASSENGERS[i].placemark);
					PASSENGERS[i].placemark = null;
					PASSENGERS[i].marker.setMap(null);
					chart();
					pickup = true;
					
					// if time was up, relaunch the timer, else add 30 seconds
					if(time == 0)
					{
						time += 60;
						counter = setInterval(timer, 1000);
						timer();
						$("#announcements").html("No announcements at this time.");
					}
					else
					{
						time += 30;
					}
					
				}
			}
		}
	}
	
	// if there are no seats available or no one was picked up, display this
	if(getSeat() == "full" || !pickup)
	{
		if(getSeat() == "full")
		{
			$("#announcements").html("No seats available at this time.");
		}
		else
		{		
			$("#announcements").html("No passengers present in a 15 meter radius.");
		}
	}
}

/**
 * Returns the index of a seat that is available, if there is none returns full
 */
function getSeat()
{
	for(var i = 0; i < shuttle.seats.length; i++)
	{
		if(shuttle.seats[i] == null)
		{
			return i;
		}
	}
	return "full";
}

/**
 * Timer which counts down
 */
function timer()
{
	time = time-1;
	if (time <= 0)
	{
		clearInterval(counter);
		points--;
		minPoints++;
		$("#announcements").html("Time ended, you were not fast enough. You lost 1 point, score is now "+points);
		return;
	}
	document.getElementById("timer").innerHTML = time + " secs";
}

/**
 * Populates Earth with passengers and houses.
 */
function populate()
{
    // mark houses
    for (var house in HOUSES)
    {
        // plant house on map
        new google.maps.Marker({
            icon: "https://google-maps-icons.googlecode.com/files/home.png",
            map: map,
            position: new google.maps.LatLng(HOUSES[house].lat, HOUSES[house].lng),
            title: house
        });
    }

    // get current URL, sans any filename
    var url = window.location.href.substring(0, (window.location.href.lastIndexOf("/")) + 1);

    // scatter passengers
    for (var i = 0; i < PASSENGERS.length; i++)
    {
        // pick a random building
        var building = BUILDINGS[Math.floor(Math.random() * BUILDINGS.length)];

        // prepare placemark
        var placemark = earth.createPlacemark("");
        placemark.setName(PASSENGERS[i].name + " to " + PASSENGERS[i].house);

        // prepare icon
        var icon = earth.createIcon("");
        icon.setHref(url + "/img/" + PASSENGERS[i].username + ".jpg");

        // prepare style
        var style = earth.createStyle("");
        style.getIconStyle().setIcon(icon);
        style.getIconStyle().setScale(4.0);

        // prepare stylemap
        var styleMap = earth.createStyleMap("");
        styleMap.setNormalStyle(style);
        styleMap.setHighlightStyle(style); 

        // associate stylemap with placemark
        placemark.setStyleSelector(styleMap);

        // prepare point
        var point = earth.createPoint("");
        point.setAltitudeMode(earth.ALTITUDE_RELATIVE_TO_GROUND);
        point.setLatitude(building.lat);
        point.setLongitude(building.lng);
        point.setAltitude(0.0);

        // associate placemark with point
        placemark.setGeometry(point);

        // add placemark to Earth
        earth.getFeatures().appendChild(placemark);

        // add marker to map
        var marker = new google.maps.Marker({
            icon: "https://maps.gstatic.com/intl/en_us/mapfiles/ms/micons/man.png",
            map: map,
            position: new google.maps.LatLng(building.lat, building.lng),
            title: PASSENGERS[i].name + " at " + building.name
        });

        // put placemark and marker into the passenger array
        PASSENGERS[i].placemark = placemark;
        PASSENGERS[i].marker = marker;
    }
}

/**
 * Handler for Earth's viewchange event.
 */
function viewchange() 
{
    // keep map centered on shuttle's marker
    var latlng = new google.maps.LatLng(shuttle.position.latitude, shuttle.position.longitude);
    map.setCenter(latlng);
    bus.setPosition(latlng);
}

/**
 * Unloads Earth.
 */
function unload()
{
    google.earth.removeEventListener(earth.getView(), "viewchange", viewchange);
    google.earth.removeEventListener(earth, "frameend", frameend);
}
