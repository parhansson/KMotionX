// Copyright (c) 2012 Joe Walnes
// Copyright (c) 2014 par.hansson@gmail.com
function createObjectFromGCode(gcode) {
  
  var object = new THREE.Object3D();

  var geometry = new THREE.Geometry();

  var lastLine = {
    x: 0,
    y: 0,
    z: 0,
    a: 0,
    b: 0,
    c: 0,
    e: 0,
    extruding: false
  };
  var ext = 0;
  var lastCommand = null;
  var parser = new GCodeParser({
    M: function() {
    },
    F: function() {
    },
    G: function(cmd, line) {
      lastCommand = cmd;
    },
    S: function() {
    },
    G0: function(cmd, line) {
      lastCommand = cmd;
    },
    G1: function(cmd, line) {
      lastCommand = cmd;
    },
    G2: function(cmd, line) {
      lastCommand = cmd;
    },
    G3: function(cmd, line) {
      lastCommand = cmd;
    }
  },
  function(args, line) {
    //Only handle G codes
    if(lastCommand.code != 'G') return;
    
    //G1, G2 and G3
    var interpolating = lastCommand.val > 0 && lastCommand.val <  4;
    //G0
    var positioning = lastCommand.val == 0;
    var newLine = {
        x: args.X !== undefined ? args.X : lastLine.x,
        y: args.Y !== undefined ? args.Y : lastLine.y,
        z: args.Z !== undefined ? args.Z : lastLine.z,
        a: args.A !== undefined ? args.A : lastLine.a,
        b: args.B !== undefined ? args.B : lastLine.b,
        c: args.C !== undefined ? args.C : lastLine.c
    };
    var printer3d = false;
    if(printer3d){
      newLine.e = args.E !== undefined ? args.E : lastLine.e,
      //TODO doesn't work as expected due to changing feedrate in the middle of line.
      newLine.extruding = (newLine.e - lastLine.e) > 0;
      if (newLine.extruding) {
        var color = new THREE.Color(newLine.extruding ? 0xBBFFFF : 0xFF00FF);
        geometry.vertices.push(new THREE.Vector3(lastLine.x, lastLine.y,lastLine.z));
        geometry.vertices.push(new THREE.Vector3(newLine.x, newLine.y,newLine.z));
        geometry.colors.push(color);
        geometry.colors.push(color);
        ext++;
      }
      
    } else {
        //THREE.GeometryUtils.merge(geometry, otherGeometry);
        var color = new THREE.Color(interpolating ? 0x080808 : 0xAAAAFF);
        //var color = new THREE.Color(interpolating ? 0x010101 : 0x888888);
        geometry.vertices.push(new THREE.Vector3(lastLine.x, lastLine.y,lastLine.z));
        geometry.vertices.push(new THREE.Vector3(newLine.x, newLine.y,newLine.z));
        geometry.colors.push(color);
        geometry.colors.push(color);
        ext++;
    }
    
    lastLine = newLine;
    
    
  },function(args, info) {
    console.info('Unknown command:', args.name, args, info);
  });

  parser.parse(gcode.lines);
  console.info("Nr of vertices", ext);
  var lineMaterial = new THREE.LineBasicMaterial({
    opacity: 0.6,
    transparent: true,
    linewidth: 1,
    vertexColors: THREE.FaceColors
  });
  object.add(new THREE.Line(geometry, lineMaterial, THREE.LinePieces));

  // Center
  geometry.computeBoundingBox();
  var center = new THREE.Vector3().addVectors(geometry.boundingBox.min,
      geometry.boundingBox.max).divideScalar(2);
  var scale = 3; // TODO: Auto size
  object.position = center.multiplyScalar(-scale);
  object.scale.multiplyScalar(scale);

  return object;
}
