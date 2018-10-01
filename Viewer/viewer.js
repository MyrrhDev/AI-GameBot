// *********************************************************************
// Global variables
// *********************************************************************

var PI = 3.14159265358979323846

var randomNumber = 1;

// Viewer state.
var gamePaused = true;
var gamePreview = false; // If true, render will be called for the next tick even if game is paused, and then will be set to false.
var gameAnim = true;
var gameDirection = 1;
var actRound = 0; // Current round index.


// Data.
var raw_data_str; // String for storing the raw data.
var dataLoaded = false; // Set to true when raw_data_str is ready to be parsed.
var data = { } // Object for storing all the game data.


// Animation.
var speed = 10; // Ticks per second.
var FRAMES_PER_ROUND = 2;
var frames = 0; // Incremented each tick, when it reaches FRAMES_PER_ROUND, actRound is updated (acording to gameDirection).

// Visuals.
var unitSize = 0.6; // 1 = same size as tile.
var unitLineWidth = 2;

var grid_color = "#888888";

var player_colors = {
    '0': "#3153d2",
    '1': "#900090",
    '2': "#FF0000",
    '3': "#FFD700",
}

var SOLDIER    = 1;
var HELICOPTER = 2;

var HIGH_VALUE = 4000;
var LOW_VALUE  = 1000;

var rot = {
    '0': -PI/2,
    '1': PI,
    '2': PI/2,
    '3': 0,
}


// *********************************************************************
// Utility functions
// *********************************************************************

function getURLParameter (name) {
    // http://stackoverflow.com/questions/1403888/get-url-parameter-with-jquery
    var a = (RegExp(name + '=' + '(.+?)(&|$)').exec(location.search)||[,null])[1]
    if (a != null) return decodeURI(a);
    return null;
}


// Callback has a single parameter with the file contents.
function loadFile (file, callback) {
    var xmlhttp;

    if (file == null || file == "") {
	alert("You must specify a file to load.");
	return;
    }

    if (window.XMLHttpRequest) xmlhttp = new XMLHttpRequest(); // Code for IE7+, Firefox, Chrome, Opera, Safari.
    else xmlhttp = new ActiveXObject("Microsoft.XMLHTTP"); // Code for IE6, IE5.

    // http://www.w3schools.com/ajax/ajax_xmlhttprequest_onreadystatechange.asp
    xmlhttp.onreadystatechange = function() {
	// Note: We cannot check xmlhttp.status != 200 for errors because status is not set when loading local files.
	if (xmlhttp.readyState == 4) callback(xmlhttp.responseText);
    }

    xmlhttp.open("GET", file, false);
    xmlhttp.send();
}


function int (s) {
    return parseInt(s);
}


function double (s) {
    return parseFloat(s);
}


function parse_assert (read_value, expected_value) {
    var correct = (read_value == expected_value);
    if (!correct) alert("Error parsing file, expected token: " + expected_value + ", read token: " + read_value + ".");
    return correct;
}


// *********************************************************************
// Initialization functions
// *********************************************************************

function parseData (raw_data_str) {

    data.nb_rounds  = 200;
    data.nb_players =   4;
    data.nb_rows    =  60;
    data.nb_cols    =  60;
    data.nb_posts   =  16;

    data.tileSize = 50;

    if (raw_data_str == "") {
	alert("Could not load game file.");
	return false;
    }

    // Convert text to tokens.
    var st = raw_data_str + "";
    var t = st.replace('\n', ' ').split(/\s+/);
    var p = 0;

    data.secgame = (t[p++] == "SecGame");

    parse_assert(t[p++], "Seed");
    data.seed = int(t[p++]);

    // Game and version.
    if (t[p++] != "ApocalypseNow2018") {
	alert("Are you sure this is an Apocalypse Now 2018 game file?");
	document.getElementById('file').value = "";
	document.getElementById('inputdiv').style.display = "";
	document.getElementById('loadingdiv').style.display = "none";
	return false;
    }

    data.version = t[p++];
    if (data.version != "3.0") alert("Unsupported game version! Trying to load it anyway.");

    parse_assert(t[p++], "names");
    data.names = new Array();
    for (var i = 0; i < data.nb_players; ++i) {
        data.names[i] = t[p++];
    }

    data.rounds = new Array();
    for (var round = 0; round <= data.nb_rounds; ++round) {

        data.rounds[round] = new Object();

        // land
        parse_assert(t[p++], "land");
	parse_assert(t[p++], "000000000011111111112222222222333333333344444444445555555555"); // p++; 
        parse_assert(t[p++], "012345678901234567890123456789012345678901234567890123456789"); // p++; 

        data.rounds[round].rows = new Array();
        for (var i = 0; i < data.nb_rows; ++i) {
            var aux = t[p++];
	    var correct = (int(aux) == i);
	    if (!correct) alert("Error parsing file, read token: " + aux);
            data.rounds[round].rows[i] = t[p++];
        }

	// posts
        parse_assert(t[p++], "posts");
        parse_assert(t[p++], "player");
        parse_assert(t[p++], "i");
        parse_assert(t[p++], "j");
        parse_assert(t[p++], "value");
	data.rounds[round].posts = new Array();
	for (var pt = 0; pt < data.nb_posts; ++pt) {
	    data.rounds[round].posts[pt] = new Object();
	    data.rounds[round].posts[pt].player = int(t[p++]);
	    data.rounds[round].posts[pt].i     	= int(t[p++]);
	    data.rounds[round].posts[pt].j     	= int(t[p++]);
	    data.rounds[round].posts[pt].value 	= int(t[p++]);
	}

	// units
        parse_assert(t[p++], "units");
        parse_assert(t[p++], "id");
        parse_assert(t[p++], "type");
        parse_assert(t[p++], "player");
        parse_assert(t[p++], "i");
        parse_assert(t[p++], "j");
        parse_assert(t[p++], "life");
        parse_assert(t[p++], "ori");
        parse_assert(t[p++], "napalm");
        parse_assert(t[p++], "parachuters");

        data.rounds[round].units = new Array();
	var id = int(t[p++]);
	var i = 0;
        while (id != -1) {
	    data.rounds[round].units[i] = new Object();
	    data.rounds[round].units[i].id     = id;
	    data.rounds[round].units[i].type   = int(t[p++]);
	    data.rounds[round].units[i].player = int(t[p++]);
	    data.rounds[round].units[i].i      = int(t[p++]);
	    data.rounds[round].units[i].j      = int(t[p++]);
	    data.rounds[round].units[i].life   = int(t[p++]);
	    data.rounds[round].units[i].ori    = int(t[p++]);
	    data.rounds[round].units[i].napalm = int(t[p++]);
	    var sz = int(t[p++]);
	    for (var j = 0; j < sz; ++j) p++;
	    id = int(t[p++]);
            ++i;
	}
	data.rounds[round].units_size = i;

	// fire
        parse_assert(t[p++], "fire");
        parse_assert(t[p++], "rounds");
        parse_assert(t[p++], "i");
        parse_assert(t[p++], "j");
	data.rounds[round].fire = new Array();
	var rounds = int(t[p++]);
	var i = 0;
	while (rounds != -1) {
	    data.rounds[round].fire[i] = new Object();
	    data.rounds[round].fire[i].rounds = rounds;
	    data.rounds[round].fire[i].i      = int(t[p++]);
	    data.rounds[round].fire[i].j      = int(t[p++]);
	    rounds = int(t[p++]);
	    ++i;
	}
	data.rounds[round].fire_size = i;

	// round
        parse_assert(t[p++], "round");
        if (int(t[p++]) != round) alert("Wrong round number!");
	
        // score
        parse_assert(t[p++], "total_score");
        data.rounds[round].total_score = new Array();
        for (var i = 0; i < data.nb_players; ++i) {
            data.rounds[round].total_score[i] = int(t[p++]);
        }

        // status
        parse_assert(t[p++], "status");
        data.rounds[round].cpu = new Array();
        for (var i = 0; i < data.nb_players; ++i) {
            var cpu = int(double(t[p++])*100);
            data.rounds[round].cpu[i] = (cpu == -100)? "out" : cpu+"%";
        }
    }
    return true;
}


// Initializing the game.
function initGame (raw_data) {
    document.getElementById("loadingdiv").style.display = "";

    // TODO: Next two calls could run concurrently.
    if (parseData(raw_data) === false) return;
    preloadImages();

    // Prepare state variables.
    // if (getURLParameter("start") == "yes") gamePaused = false;
    // else gamePaused = true;
    gamePaused = false;

    gamePreview = true;

    // Canvas element.
    canvas = document.getElementById('myCanvas');
    context = canvas.getContext("2d");

    // Prepare the slider.
    $("#slider").slider({
	slide: function(event, ui) {
	    var value = $("#slider").slider( "option", "value" );
	    actRound = value;
	    frames = 0;
	    gamePaused = true;
	    gamePreview = true;
	}
    });
    $("#slider").width(600);
    $("#slider").slider("option", {min: 0, max: data.nb_rounds});

    // Set the listerners for interaction.
    document.addEventListener('mousewheel', onDocumentMouseWheel, false);
    document.addEventListener('keydown', onDocumentKeyDown, false);
    document.addEventListener('keyup', onDocumentKeyUp, false);

    window.addEventListener('resize', onWindowResize, false);
    onWindowResize();

    document.getElementById("loadingdiv").style.display = "none";
    document.getElementById("gamediv").style.display = "";

    mainloop();
}


function preloadImages () {

    data.img = new Array();

    // Grass
    data.img.grass = new Array();

    data.img.grass[0] = new Image();
    data.img.grass[0].src = "img/grass0.png";

    data.img.grass[1] = new Image();
    data.img.grass[1].src = "img/grass1.png";

    data.img.grass[2] = new Image();
    data.img.grass[2].src = "img/grass2.png";

    data.img.grass[3] = new Image();
    data.img.grass[3].src = "img/grass3.png";

    data.img.grass[4] = new Image();
    data.img.grass[4].src = "img/grass4.png";


    // Tree
    data.img.tree = new Image();
    data.img.tree.src = "img/tree.png";

    // Water
    data.img.water = new Image();
    data.img.water.src = "img/water.png";


    // Mountain
    data.img.mountain = new Image();
    data.img.mountain.src = "img/mountain.png";


    // Posts
    data.img.hi_posts = new Array();

    data.img.hi_posts[0] = new Array();
    data.img.hi_posts[0][0] = new Image();
    data.img.hi_posts[0][0].src = "img/square0.3153d2.png";
    data.img.hi_posts[0][1] = new Image();
    data.img.hi_posts[0][1].src = "img/square1.3153d2.png";

    data.img.hi_posts[1] = new Array();
    data.img.hi_posts[1][0] = new Image();
    data.img.hi_posts[1][0].src = "img/square0.900090.png";
    data.img.hi_posts[1][1] = new Image();
    data.img.hi_posts[1][1].src = "img/square1.900090.png";

    data.img.hi_posts[2] = new Array();
    data.img.hi_posts[2][0] = new Image();
    data.img.hi_posts[2][0].src = "img/square0.FF0000.png";
    data.img.hi_posts[2][1] = new Image();
    data.img.hi_posts[2][1].src = "img/square1.FF0000.png";

    data.img.hi_posts[3] = new Array();
    data.img.hi_posts[3][0] = new Image();
    data.img.hi_posts[3][0].src = "img/square0.FFD700.png";
    data.img.hi_posts[3][1] = new Image();
    data.img.hi_posts[3][1].src = "img/square1.FFD700.png";

    data.img.hi_posts[4] = new Array();
    data.img.hi_posts[4][0] = new Image();
    data.img.hi_posts[4][0].src = "img/square0.FFFFFF.png";
    data.img.hi_posts[4][1] = new Image();
    data.img.hi_posts[4][1].src = "img/square1.FFFFFF.png";

    data.img.lo_posts = new Array();

    data.img.lo_posts[0] = new Image();
    data.img.lo_posts[0].src = "img/square.3153d2.png";

    data.img.lo_posts[1] = new Image();
    data.img.lo_posts[1].src = "img/square.900090.png";

    data.img.lo_posts[2] = new Image();
    data.img.lo_posts[2].src = "img/square.FF0000.png";

    data.img.lo_posts[3] = new Image();
    data.img.lo_posts[3].src = "img/square.FFD700.png";

    data.img.lo_posts[4] = new Image();
    data.img.lo_posts[4].src = "img/square.FFFFFF.png";


    // Fire
    data.img.fire = new Array();

    data.img.fire[0] = new Image();
    data.img.fire[0].src = "img/fire0.png";

    data.img.fire[1] = new Image();
    data.img.fire[1].src = "img/fire1.png";


    // Soldiers
    data.img.soldier = new Array();

    data.img.soldier[0] = new Image();
    data.img.soldier[0].src = "img/dot.3153d2.png";

    data.img.soldier[1] = new Image();
    data.img.soldier[1].src = "img/dot.900090.png";

    data.img.soldier[2] = new Image();
    data.img.soldier[2].src = "img/dot.FF0000.png";

    data.img.soldier[3] = new Image();
    data.img.soldier[3].src = "img/dot.FFD700.png";


    // Helicopters
    data.img.heli = new Array();

    data.img.heli[0] = new Array();

    data.img.heli[0][0] = new Image();
    data.img.heli[0][0].src = "img/helicopter.x.3153d2.png";

    data.img.heli[0][1] = new Image();
    data.img.heli[0][1].src = "img/helicopter.x.900090.png";

    data.img.heli[0][2] = new Image();
    data.img.heli[0][2].src = "img/helicopter.x.FF0000.png";

    data.img.heli[0][3] = new Image();
    data.img.heli[0][3].src = "img/helicopter.x.FFD700.png";

    data.img.heli[1] = new Array();

    data.img.heli[1][0] = new Image();
    data.img.heli[1][0].src = "img/helicopter.+.3153d2.png";

    data.img.heli[1][1] = new Image();
    data.img.heli[1][1].src = "img/helicopter.+.900090.png";

    data.img.heli[1][2] = new Image();
    data.img.heli[1][2].src = "img/helicopter.+.FF0000.png";

    data.img.heli[1][3] = new Image();
    data.img.heli[1][3].src = "img/helicopter.+.FFD700.png";
}


// *********************************************************************
// Main loop functions
// *********************************************************************

function updateGame () {
    $("#slider").slider("option", "value", actRound);
}


function drawGame () {

    randomNumber = 1;

    //Boundary check
    if (actRound < 0) actRound = 0;
    if (actRound >= data.nb_rounds) actRound = data.nb_rounds;

    if (canvas.getContext) {
        var context = canvas.getContext('2d');
        var rectSize = data.tileSize;

	var fixedMargin = 0; // 10;
	var heightScore = 120; // 180;

	canvas.width  = window.innerWidth  - 2*fixedMargin;
        canvas.height = window.innerHeight - 2*fixedMargin - heightScore;

	var sw = canvas.width  / (rectSize*data.nb_cols);
	var sh = canvas.height / (rectSize*data.nb_rows);
	var s;
	if (sw < sh) {
	    s = sw;
	    var offset = (canvas.height - s*rectSize*data.nb_rows)/ 2;
	    canvas.style.marginTop  = fixedMargin + offset;
	    canvas.style.marginLeft = fixedMargin;
	}
	else {
	    s = sh;
	    var offset = (canvas.width - s*rectSize*data.nb_cols)/ 2;
	    canvas.style.marginTop  = fixedMargin;
	    canvas.style.marginLeft = fixedMargin + offset;
	}
        context.scale(s, s);

        // outer rectangle
        context.fillStyle = "rgb(30,30,30)";
        context.fillRect(0, 0, rectSize*data.nb_cols, rectSize*data.nb_rows);

	// Draw background
	for (var i = 0; i < data.nb_rows; i++)
            drawBackground(actRound, i);

	// Draw posts
	for (var pt = 0; pt < data.nb_posts; ++pt) {
	    var     i = data.rounds[actRound].posts[pt].i;
	    var     j = data.rounds[actRound].posts[pt].j;
	    var player = data.rounds[actRound].posts[pt].player;
	    var value = data.rounds[actRound].posts[pt].value;
	    if (player == -1) player = 4;
	    var rectSize = data.tileSize;
	    var post;
	    if (value == HIGH_VALUE) {
		var param;
		if (actRound % 4 < 2) param = 0;
		else                  param = 1;
		if (gameAnim) post = data.img.hi_posts[player][param];
		else          post = data.img.hi_posts[player][0];
	    }
	    else {
		post = data.img.lo_posts[player];
	    } 
	    context.drawImage(post, (j-1)*rectSize, (i-1)*rectSize);
	}

	// Draw forest
	for (var i = 0; i < data.nb_rows; i++)
            drawForest(actRound, i);

	// Draw fire
	for (var k = 0; k < data.rounds[actRound].fire_size; ++k) {
	    if (data.rounds[actRound].fire[k].rounds > 0) {
		var i = data.rounds[actRound].fire[k].i;
		var j = data.rounds[actRound].fire[k].j;
		var fire;
		if (gameAnim) fire = data.img.fire[frames % 2]; 
		else          fire = data.img.fire[0]; 
		context.drawImage(fire, j*data.tileSize, i*data.tileSize);
	    }
	}

	// Draw soldiers.
	for (var k = 0; k < data.rounds[actRound].units_size; ++k) {
            var id    = data.rounds[actRound].units[k].id;
            var i     = data.rounds[actRound].units[k].i;
            var j     = data.rounds[actRound].units[k].j;
	    var player = data.rounds[actRound].units[k].player;
	    var type = data.rounds[actRound].units[k].type;
	    if (type == SOLDIER) {
	    	if (actRound < data.nb_rounds && gameAnim) {
		    var pos = 0;
		    for (; pos < data.rounds[actRound+1].units_size &&
			     data.rounds[actRound+1].units[pos].id != id; ++pos) {
			;
		    }
	    	    if (pos != data.rounds[actRound+1].units_size) {
	    	    	var ni = data.rounds[actRound+1].units[pos].i;
	    	    	var nj = data.rounds[actRound+1].units[pos].j;
	    	    	i = correctI(i, ni);
	    	    	j = correctJ(j, nj);
	    	    }
	    	}
	    	var soldier = data.img.soldier[player];
	    	context.drawImage(soldier, j*data.tileSize, i*data.tileSize);
	    }
	}

	// Draw helicopters.
	for (var k = 0; k < data.rounds[actRound].units_size; ++k) {
            var id    = data.rounds[actRound].units[k].id;
            var i     = data.rounds[actRound].units[k].i;
            var j     = data.rounds[actRound].units[k].j;
	    var player = data.rounds[actRound].units[k].player;
	    var type = data.rounds[actRound].units[k].type;
	    if (type != SOLDIER) {
		var ori = data.rounds[actRound].units[k].ori;
		var diff = 0;
		if (actRound < data.nb_rounds && gameAnim) {
		    var pos = 0;
		    for (; pos < data.rounds[actRound+1].units_size &&
			 data.rounds[actRound+1].units[pos].id != id; ++pos) {
			;
		    }
		    var ni   = data.rounds[actRound+1].units[pos].i;
		    var nj   = data.rounds[actRound+1].units[pos].j;
		    var nori = data.rounds[actRound+1].units[pos].ori;
		    var diff = nori - ori;
		    if (diff ==  3) diff = -1;
		    if (diff == -3) diff =  1;
		    i = correctI(i, ni);
		    j = correctJ(j, nj);
		}
		var heli;
		if (gameAnim) {
		    heli = data.img.heli[frames % 2][player];
		}
		else {
		    heli = data.img.heli[0][player];
		    diff = 0;
		}
		context.save(); 
		context.translate( (2*j+1) * data.tileSize/2, (2*i+1) * data.tileSize/2);
		context.rotate(rot[ori] - (frames*diff*PI) / (2*FRAMES_PER_ROUND));
		context.drawImage(heli, -5*data.tileSize/2, -5*data.tileSize/2);
		context.restore();
	    }
	}
    }
}


function writeGameState () {
    // Write round.
    $("#round").html("Round: " + actRound);

    // Update scoreboard.
    var scoreboard = "";
    for (var i = 0; i < data.nb_players; ++i) {
	scoreboard += "<span class='score'>"
        scoreboard += "<span class='score'>"
            + "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:"+ player_colors[i] +"'></div>"
	    + "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>" + data.names[i] + "</div>"
	    + "<br/>"
	    + "<div style='margin-left: 10px;'>"
	    + (data.secgame ? "<div style='padding:2px;'>CPU: " + data.rounds[actRound].cpu[i] + "</div>" : "")
	    + "</div>"
	    + "</span><br/><br/>";
    }
    $("#scores").html(scoreboard);

    var order = [0, 1, 2, 3];
    for (var i = 0; i < 3; ++i) {
	for (var j = i + 1; j < 4; ++j) {
	    if (data.rounds[actRound].total_score[order[i]] < data.rounds[actRound].total_score[order[j]]) {
		var k = order[i];
		order[i] = order[j];
		order[j] = k;
	    }
	}
    }

    var totalboard = "";
    for (var i = 0; i < data.nb_players; ++i) {
	totalboard += "<span class='total'>"
	    + "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:" + player_colors[order[i]] + "'></div>"
	    + "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>"
	    + data.rounds[actRound].total_score[order[i]] + "</div>"
	    + "</span><br/><br/>";
    }
    $("#totals").html(totalboard);
}


function correctI (oldI, newI) {
    return oldI + ((newI-oldI) * frames) / FRAMES_PER_ROUND;
}

function correctJ (oldJ, newJ) {
    return oldJ + ((newJ-oldJ) * frames) / FRAMES_PER_ROUND;
}


function drawBackground (round, row) {
    var ctx = canvas.getContext('2d');
    var rectSize = data.tileSize;
    for (var i = 0; i < data.nb_cols; ++i) {
	randomNumber = (125 * randomNumber + 1) % 4096;
	var grass = data.img.grass[randomNumber % 5]; 
	ctx.drawImage(grass, i*rectSize, row*rectSize);
        var type = data.rounds[round].rows[row][i];
	if (type != '.' && type != ':') { 
	    // Grass is already there.
	    // Trees are foreground.
            var tile = selectTile(round, type, row, i);
            ctx.drawImage(tile, i*rectSize, row*rectSize);
	}
    }
}


function drawForest (round, row) {
    var ctx = canvas.getContext('2d');
    var rectSize = data.tileSize;
    for (var i = 0; i < data.nb_cols; ++i) {
        var type = data.rounds[round].rows[row][i];
	if (type == ':') { // Tree
            ctx.drawImage(data.img.tree, i*rectSize, row*rectSize);
	}
    }
}


function selectTile (round, type, row, col) {
    switch (type) {
    case 'O': // Water
        return data.img.water;
        break;
    case 'X': // Mountain
        return data.img.mountain;
        break;
    default:
        break;
    }
}


// *********************************************************************
// Button events
// *********************************************************************

function playButton () {
    gamePaused = false;
}


function pauseButton () {
    gamePaused = true;
    gamePreview = true; // To call render again.
    frames = 0;
}


function startButton () {
    gamePaused = true;
    gamePreview = true;
    frames = 0;
    actRound = 0;
}


function endButton () {
    gamePreview = true;
    frames = 0;
    actRound = data.nb_rounds;
}


function animButton () {
    gameAnim = !gameAnim;
}


function closeButton () {
    window.close();
}


function helpButton () {
    help();
}


// *********************************************************************
// Keyboard and Mouse events
// *********************************************************************

function onDocumentMouseWheel (event) {
}


function onDocumentKeyDown (event) {
}


function onDocumentKeyUp (event) {
    // http://www.webonweboff.com/tips/js/event_key_codes.aspx
    switch (event.keyCode) {
    case 36: // Start.
	gamePreview = true;
	actRound = 0;
	frames = 0;
	break;

    case 35: // End.
	gamePreview = true;
	actRound = data.nb_rounds;
	frames = 0;
	break;

    case 33: // PageDown.
	gamePreview = true;
	actRound -= 10;
	frames = 0;
	break;

    case 34: // PageUp.
	gamePreview = true;
	actRound += 10;
	frames = 0;
	break;

    case 38: // ArrowUp.
    case 37: // ArrowLeft.
	gamePaused= true;
	gamePreview = true;
	--actRound;
	frames = 0;
	break;

    case 40: // ArrowDown.
    case 39: // ArrowRight.
	gamePaused = true;
	gamePreview = true;
	++actRound;
	frames = 0;
	break;

    case 32: // Space.
	if (gamePaused) playButton();
	else pauseButton();
	break;

    case 72: // "h"
	helpButton();
	break;

    default:
	// $("#debug").html(event.keyCode);
	break;
    }
}


function onWindowResize (event) {
    // Constants.
    var header_height = 150;
    var canvas_margin = 20;

    // Set canvas size.
    var size = Math.min(document.body.offsetWidth, document.body.offsetHeight - header_height) - canvas_margin*2;

    canvas.width  = size;
    canvas.height = size;

    var max_dimension = Math.max(data.cols,data.rows);
    tileSize = size / max_dimension;

    drawGame();
}


function help () {
    // Opens a new popup with the help page.
    var win = window.open('help.html', 'name', 'height=400, width=300');
    if (window.focus) win.focus();
    return false;
}


// *********************************************************************
// This function is called periodically.
// *********************************************************************

function mainloop () {
    // Configure buttons.
    if (gamePaused) {
	$("#but_play").show();
	$("#but_pause").hide();
    }
    else {
	$("#but_play").hide();
	$("#but_pause").show();
    }

    if (actRound < 0) actRound = 0;

    if (actRound > data.nb_rounds) {
	actRound = data.nb_rounds;
	gamePaused = true;
	frames = 0;
    }

    if (!gamePaused || gamePreview) {
	updateGame();
	drawGame();
	writeGameState();

	if (gamePreview) {
	    frames = 0;
	    gamePreview = false;
	}
	else {
	    ++frames;
	    if (frames == FRAMES_PER_ROUND) {
		frames = 0;
		actRound += gameDirection;
	    }
	}
    }

    // Periodically call mainloop.
    var frame_time = 1000/speed;
    setTimeout(mainloop, frame_time);
}


// *********************************************************************
// Main function, it is called when the document is ready.
// *********************************************************************

function init () {
    // Get url parameters.
    var game;
    if (getURLParameter("sub") != null) {
	var domain = window.location.protocol + "//" + window.location.host;
	if (getURLParameter("nbr") != null)
	    game = domain + "/?cmd=lliuraments&sub=" + getURLParameter("sub") + "&nbr=" + getURLParameter("nbr") + "&download=partida";
	else
	    game = domain + "/?cmd=partida&sub=" + getURLParameter("sub") + "&download=partida";
    }
    else game = getURLParameter("game");

    if (game == null || game == "") {
	// Ask the user for a game input.
	var inputdiv = document.getElementById('inputdiv')
	inputdiv.style.display = "";
	document.getElementById('file').addEventListener('change', function(evt) {
	    //http://www.html5rocks.com/en/tutorials/file/dndfiles/
	    var file = evt.target.files[0];
	    var reader = new FileReader();
	    reader.readAsText(file);
	    reader.onloadend = function(evt) {
		if (evt.target.readyState != FileReader.DONE) alert("Error accessing file.");
		else { // DONE == 2.
		    inputdiv.style.display = "none";
		    document.getElementById("loadingdiv").style.display = "";
		    initGame(reader.result);
		}
	    };
	}, false);
    }
    else {
	document.getElementById("loadingdiv").style.display = "";
	// Load the given game.
	loadFile(game, initGame);
    }
}
