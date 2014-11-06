// Copyright (c) 2014 par.hansson@gmail.com
var viewer;
var machine;
var gcodeText;
var scene = null;
var object = null;

function clone(obj) {
    return JSON.parse(JSON.stringify(obj));
}

function logText(elementId, color, text) {
    var logNode = document.getElementById(elementId);
    var div = document.createElement('div');
    div.innerHTML = '<span style="color: ' + color + ';">' + text + ' </span>';
    logNode.appendChild(div);
    logNode.scrollTop = div.offsetTop;
}

var socketHandlers = {
    STATUS: function(obj) {
        //select gcode row
        var line = obj.data.line;
        gcodeText.select(line);
        viewer.draw(obj.data.message);
        logText('status', 'green', line + ": " + obj.data.message);
    },
    COMPLETE: function(obj) {
        //select gcode row
        var line = obj.data.line;
        gcodeText.select(line);
        logText('status', 'green', "Done: " + line + ": " + obj.data.message);
    },
    ERR_MSG: function(obj) {
        logText('error', 'red', obj.data);
    },
    CONSOLE: function(obj) {
        logText('console', 'blue', obj.data);
    },
    USER: function(obj) {
        if (confirm('USR: ' + obj.data + ' ?')) {
            return 0; // zero is true for this callback
        } else {
            return 1;
        }
    },
    USER_M_CODE: function(obj) {
        if (confirm('Are you sure you want to continue after M' + obj.data + ' ?')) {
            return 0; // zero is true for this callback
        } else {
            return 1;
        }
    },
    STATE: function(obj) {
	    $( "#feed_hold_btn" ).toggleClass( "button_pressed", obj.data.feedHold == 1 );
	    $( "#simulate_btn" ).toggleClass( "button_pressed", obj.data.simulate == 1 );
        //TODO if interpreting prevent import file drop events.	    
	    $( "#interpret_btn" ).toggleClass( "button_pressed", obj.data.interpreting == 1 );

        //only load if different from loaded
        //TODO or if file has been updated, need a force flag
		var lastLoaded = localStorage.getItem('last-loaded');
        if (obj.data.file != "" && obj.data.file != lastLoaded) {
            loadGCodeFromPath(obj.data.file);
        }
        //TODO listen for machine configuration changes
    },
    MESSAGEBOX: function(obj) {
        alert(obj.data.message);
        return 0;
    }

}

function listFiles(selector, dir) {
    $(selector).load(dir + "  tr > td > a", function() {
        $(selector +" > a").replaceWith(function() {
        	var fileName = this.innerText;
        	var path = dir + fileName;
        	return "<li><a href='javascript:loadGCode(\"" + path + "\")'>" + path + "</a></li>";
        });
    });
}

function error(msg) {
  alert(msg);
}

function loadGCode(path) {
	$('#openModal').modal('hide');
    RPC.loadGCodeFile.invoke(path);
}


function loadFile(path, callback /* function(contents) */) {
  $.get(path, null, callback, 'text')
    .error(function() { error() });
}

function about() {
  $('#aboutModal').modal();
}

function loadGCodeFromPath(path) {
  $('#openModal').modal('hide');
  if (object) {
    scene.remove(object);
  }
    //TODO failure to load file should be handled somehow
	loadFile(path, function(gcode) {
		gcodeText.init(gcode);
    	object = createObjectFromGCode(gcode);
    	scene.add(object);
    	localStorage.setItem('last-loaded', path);
    	localStorage.removeItem('last-imported');
    });
}

function openGCodeFromText(gcode) {
  $('#openModal').modal('hide');
  if (object) {
    scene.remove(object);
  }
  gcodeText.init(gcode);
  object = createObjectFromGCode(gcode);
  scene.add(object);
  localStorage.setItem('last-imported', gcode);
  localStorage.removeItem('last-loaded');
}

function createScene(element) {
//Copyright (c) 2012 Joe Walnes
  // Renderer
  var renderer = new THREE.WebGLRenderer({clearColor:0x000000, alpha: true});
  renderer.setSize(element.width(), element.height());
  element.append(renderer.domElement);
  renderer.clear();

  // Scene
  var scene = new THREE.Scene(); 

  // Lights...
  [[0,0,1,  0xFFFFCC],
   [0,1,0,  0xFFCCFF],
   [1,0,0,  0xCCFFFF],
   [0,0,-1, 0xCCCCFF],
   [0,-1,0, 0xCCFFCC],
   [-1,0,0, 0xFFCCCC]].forEach(function(position) {
    var light = new THREE.DirectionalLight(position[3]);
    light.position.set(position[0], position[1], position[2]).normalize();
    scene.add(light);
  });

  // Camera...
  var fov    = 45,
      aspect = element.width() / element.height(),
      near   = 1,
      far    = 10000,
      camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
  //camera.rotationAutoUpdate = true;
  //camera.position.x = 0;
  //camera.position.y = 500;
  camera.position.z = 300;
  //camera.lookAt(scene.position);
  scene.add(camera);
  controls = new THREE.TrackballControls(camera, document.getElementById('renderArea') );
  controls.noPan = true;
  controls.dynamicDampingFactor = 0.15;

  // Action!
  function render() {
    controls.update();
    renderer.render(scene, camera);

    requestAnimationFrame(render); // And repeat...
  }
  render();

  // Fix coordinates up if window is resized.
  $(window).on('resize', function() {
    renderer.setSize(element.width(), element.height());
    camera.aspect = element.width() / element.height();
    camera.updateProjectionMatrix();
    controls.screen.width = window.innerWidth;
    controls.screen.height = window.innerHeight;
  });

  return scene;
}

$(function() {

    machine = new Machine();
    viewer = new ViewerSVG("#svgviewer");
    gcodeText = new LineSelectingTextArea('code_text');
    new SocketHandler('ws://' + window.location.host + '/ws', socketHandlers ,'output' );

    $("#svgviewer").hide();
	
	
    $("#interpret_btn").click(function() {
	  var lastImported = localStorage.getItem('last-imported');
	  var lastLoaded = localStorage.getItem('last-loaded');
	  if (lastImported) {
    	alert("Cannot interpret unsaved file.");
	  } else {
		RPC.interpret.invoke([2, lastLoaded, 0, -1, 1]);
	  }
        
    });

    $("#list_files_btn").click(function() {
        listFiles("#file_list","gcode/");
	    $('#openModal').modal();
    });

    $("#toggle_renderer_btn").click(function() {
        $("#svgviewer").toggle();
        $("#renderArea").toggle();
    });
    $("#feed_hold_btn").click(function() {
        RPC.feedHold.invoke();
    });
    $("#simulate_btn").click(function() {
        RPC.simulate.invoke();
    });

    $("#status_log_btn").click(function() {
        $("#status").empty();
    });
    $("#error_log_btn").click(function() {
        $("#error").empty();
    });
    $("#console_log_btn").click(function() {
        $("#console").empty();
    });

    $("#raw_log_btn").click(function() {
        $("#output").empty();
    });

    $("#set_motion_params_btn").click(function() {
        RPC.setMotionParams.invoke(machine.axes);
    });

    $("#init_machine_btn").click(function() {
        RPC.compileAndLoadCoff.invoke([machine.initProgram, machine.initThread]);
    });
    
    $("#edit_machine_btn").click(function() {
        machine.edit();
        $('#settingsModal').modal();
    });

    $("#settingsModalBody").load("settings.html", function() {
        // Load was performed

        $("#save_machine_btn").click(function() {
            machine.store();
            var file = "settings/" + machine.name + ".cnf";
    		var param = {};
		    param[file] = machine;
			RPC.saveMachine.invoke(param);
        });

        $("#new_machine_btn").click(function() {
            machine.clear();
            machine.edit();
        });
    });

    $.getJSON("settings/machine.cnf")
        .done(function(data) {
            machine.copy(data);
        }).fail(function() {
            machine.clear();
        });

  //Copyright (c) 2012 Joe Walnes
  if (!Modernizr.webgl) {
    alert('Sorry, you need a WebGL capable browser to use this.\n\nGet the latest Chrome or FireFox.');
    return;
  }

  if (!Modernizr.localstorage) {
    alert("Man, your browser is ancient. I can't work with this. Please upgrade.");
    return;
  }

  // Show 'About' dialog for first time visits.
  if (!localStorage.getItem("not-first-visit")) {
    localStorage.setItem("not-first-visit", true);
    setTimeout(about, 500);
  }

  // Drop files from desktop onto main page to import them.
  $('body').on('dragover', function(event) {
    event.stopPropagation();
    event.preventDefault();
    event.originalEvent.dataTransfer.dropEffect = 'copy'
  }).on('drop', function(event) {
    event.stopPropagation();
    event.preventDefault();
    var files = event.originalEvent.dataTransfer.files;
    if (files.length > 0) {
      var reader = new FileReader();
      reader.onload = function() {
        openGCodeFromText(reader.result);
      };
      reader.readAsText(files[0]);
    }
  });

  scene = createScene($('#renderArea'));
  //TODO wait for state callback before doing this.
  var lastImported = localStorage.getItem('last-imported');
  var lastLoaded = localStorage.getItem('last-loaded');
  if (lastImported) {
    openGCodeFromText(lastImported);
  } else if(lastLoaded){
	  loadGCodeFromPath(lastLoaded);
    //loadGCodeFromPath(lastLoaded || 'examples/hand_ok.gcode');
  }
});
