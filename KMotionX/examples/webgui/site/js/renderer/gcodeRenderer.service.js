(function() {
  angular.module('KMXRenderer')
    .factory('GCodeRenderer',GCodeRenderer);
  
  GCodeRenderer.$inject = ['$q','kmxThreeView'];
  
  function GCodeRenderer($q,kmxThreeView) {
    var currentObject = null;
    
    var instance = {
        clearScene: clearScene,
        renderGCode: renderGCode
    }; 
    var gworker = null;
    var background = true;
    
    return instance;
    
    
    function clearScene(){
      if (currentObject) {
        kmxThreeView.scene.remove(currentObject);
      }
    }
    
    function renderGCode(gcode){
      createObject(gcode).then(function (object) {
          clearScene();
          currentObject = object;
          kmxThreeView.scene.add(currentObject);
      });
    }
    
    
    
    //Copyright (c) 2012 Joe Walnes
    //Copyright (c) 2014 par.hansson@gmail.com
    function createObject(gcode){
      
       var group = new THREE.Group();
       group.name = 'GCODE';

       var interpolateColor = new THREE.Color(0x080808);
       var positionColor = new THREE.Color(0xAAAAFF);
       var lineMaterial = new THREE.LineBasicMaterial({
         opacity: 0.6,
         transparent: true,
         linewidth: 1,
         vertexColors: THREE.FaceColors
       });
       
       var lastVector = {
         x: 0,
         y: 0,
         z: 0,
         a: 0,
         b: 0,
         c: 0,
         e: 0,
         extruding: false
       };
       var lastCommand = {
           code:'NONE',
           val: -1
       };
       
       var scale = 1.0;
       var absolute = true;
       
       var gcodeHandlers = {
           M: function() {
           },
           F: function() {
           },
           G: function(cmd, line) {
             lastCommand = cmd;
           },
           S: function() {
           },
           G20: function(cmd, line) {
             scale = 25.4; //Inches
           },
           G21: function(cmd, line) {
             scale = 1.0; //mm
           },
           G90: function(cmd, line) {
             //absolute
             absolute = true;
           },
           G91: function(cmd, line) {
             //relative
             absolute = false;
           }
         };
  
       var lineGeometry = new THREE.Geometry();
       
       // Create the final Object3d to add to the scene
       group.add(new THREE.Line( lineGeometry, lineMaterial ));


       
       
       function parameterHandler(args, line) {
         //Only handle G codes 1-3
         if(lastCommand.code != 'G' || lastCommand.val < 0 || lastCommand.val >  3) return;
         var currentVector;
         if(absolute){
           currentVector = {
               x: args.X !== undefined ? args.X*scale : lastVector.x,
               y: args.Y !== undefined ? args.Y*scale : lastVector.y,
               z: args.Z !== undefined ? args.Z*scale : lastVector.z,
               a: args.A !== undefined ? args.A*scale : lastVector.a,
               b: args.B !== undefined ? args.B*scale : lastVector.b,
               c: args.C !== undefined ? args.C*scale : lastVector.c
           };
           
         } else {
           currentVector = {
               x: args.X !== undefined ? args.X*scale+lastVector.x : lastVector.x,
               y: args.Y !== undefined ? args.Y*scale+lastVector.y : lastVector.y,
               z: args.Z !== undefined ? args.Z*scale+lastVector.z : lastVector.z,
               a: args.A !== undefined ? args.A*scale+lastVector.a : lastVector.a,
               b: args.B !== undefined ? args.B*scale+lastVector.b : lastVector.b,
               c: args.C !== undefined ? args.C*scale+lastVector.c : lastVector.c
           };
           
         }
  
         if(lastCommand.val == 0 ){
           lastVector = createLine(lastCommand, args, lastVector, currentVector,lineGeometry, positionColor);
         } else if(lastCommand.val == 1 ){
           lastVector = createLine(lastCommand, args, lastVector, currentVector,lineGeometry,interpolateColor);
           //lastVector = createLinePrinter(lastCommand, args, lastVector, currentVector,geometry);
         } else if(lastCommand.val == 2){
           lastVector = createArc(lastCommand, args, lastVector, currentVector,lineGeometry, interpolateColor,true);
         } else if(lastCommand.val == 3){
           lastVector = createArc(lastCommand, args, lastVector, currentVector,lineGeometry, interpolateColor,false);
         }
         lastVector = currentVector;
       };
       
       function defaultHandler(args, info) {
         console.info('Unknown command:', args.name, args, info);
       };
      
       function addGeometry(){
         //console.info("Nr of vertices", ext);
         //object.add(new THREE.Line(geometry, lineMaterial/*, THREE.LinePieces*/));
         //object.add(new THREE.Line(moveGeometry, lineMaterial, THREE.LinePieces));
         
         //object.add(moveGeometry);
         //object.add(geometry);
         //object.add(arcs);
         // Center
         //geometry.computeBoundingBox();
         //var center = new THREE.Vector3().addVectors(geometry.boundingBox.min,
         //    geometry.boundingBox.max).divideScalar(2);
         var scale = 3; // TODO: Auto size
         //var scale = 10; // TODO: Auto size
         //object.position = center.multiplyScalar(-scale);
         //object.scale.multiplyScalar(scale);
         
       }
  
       if(background){
         return parseWorker(gcode);
       } else {
         return parse(gcode);
       }
       
       function parseWorker(gcode){
         
         var defer = $q.defer();
         if(!gworker){
           gworker = new Worker("js/renderer/gcode-worker.js");
         }
         gworker.onmessage = function(event) {
           
           var cmd = event.data.cmd;
           var params = event.data.params;
           if(cmd){
             var handler = gcodeHandlers[cmd.name] || gcodeHandlers[cmd.code] || defaultHandler;
             handler(cmd, event.data.line);
           } else if(params){
             parameterHandler(params, event.data.line)
           } else if(event.data == 'done'){
             addGeometry();
             console.timeEnd("parsingAndTransfer");
             //console.profileEnd();
             defer.resolve(group);
           }
         };
         console.time("parsingAndTransfer");
         //console.profile();
         var objData =
         {
             command: "parse",
             ab: new ArrayBuffer(100)
             ,i8: new Int8Array(200)
         };
         //objWorker.postMessage(objData, [objData.ab, objData.i8.buffer]);
         
         gworker.postMessage({command:'parse',gcode:gcode.lines});
         return defer.promise;
       }
       
       function parse(gcode){
         var defer = $q.defer();
         var parser = new GCodeParser(gcodeHandlers,parameterHandler,defaultHandler);
         parser.parse(gcode.lines);
         addGeometry();
         defer.resolve(group);
         return defer.promise;
       }  
      }
    
    
    }
  
  //use for 3dprinter files
  function createLinePrinter(cmd, args, lastVector, currentVector, geometry){

      currentVector.e = args.E !== undefined ? args.E : lastVector.e,
      //TODO doesn't work as expected due to changing feedrate in the middle of line.
      currentVector.extruding = (currentVector.e - lastVector.e) > 0;
      if (currentVector.extruding) {
        var color = new THREE.Color(currentVector.extruding ? 0xBBFFFF : 0xFF00FF);
        geometry.vertices.push(new THREE.Vector3(lastVector.x, lastVector.y,lastVector.z));
        geometry.vertices.push(new THREE.Vector3(currentVector.x, currentVector.y,currentVector.z));
        geometry.colors.push(color);
        geometry.colors.push(color);
      }    
    return currentVector;
  }
  
  function createLine(cmd, args, lastVector, currentVector,geometry,color){

    
    geometry.vertices.push(new THREE.Vector3(lastVector.x, lastVector.y,lastVector.z));
    geometry.vertices.push(new THREE.Vector3(currentVector.x, currentVector.y,currentVector.z));
    geometry.colors.push(color);
    geometry.colors.push(color);
    
    
    return currentVector;
  }
  
  function createArc(cmd, args, lastVector, currentVector,geometry, color,clockWise){
 
    var curve = new GCode.Curve3(
        lastVector,
        currentVector,
        args,
        clockWise);    
    //geometry.vertices.push(new THREE.Vector3(lastVector.x, lastVector.y,lastVector.z));
    geometry.vertices.push.apply(geometry.vertices,curve.getPoints( 50 ));
    
    
    return currentVector;
  }
    
  
})();