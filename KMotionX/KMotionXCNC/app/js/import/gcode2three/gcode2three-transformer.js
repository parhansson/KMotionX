'use strict';
(function() {
  angular.module('KMXImport').run(register);
  
  register.$inject = ['$q', 'transformer', 'transformerSettings'];
  
  function register($q, transformer, transformerSettings){
    var descriptor = { 
      name: 'G-Code to Three',
      inputMime: ["application/x-gcode"],
      outputMime: "application/x-kmx-three",
      execute: transcodeGCode
    };
    
	  transformer.register(descriptor);
    
    var disableWorker = false;//document.URL.startsWith("file");
    
    function transcodeGCode(source){
        var transformedDefer = $q.defer();
        if (angular.isObject(source)) {
          createObject(transformedDefer,source);     
        } else {
          transformedDefer.reject("Unsupported source: " + (typeof source));
        }
        return transformedDefer.promise; 
      }
      
      
    //Copyright (c) 2012 Joe Walnes
    //Copyright (c) 2014 par.hansson@gmail.com
    function createObject(transformedDefer, gcode){
      
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
  
       if(disableWorker){
         if(window.GCodeParser === undefined){
           KMX.Util.injectScript("js/import/gcode2three/gcode-parser.js").then(function() {
              parse(transformedDefer, gcode);
            },function(reason){
              transformedDefer.reject(reason);
            });
         } else {
           parse(transformedDefer, gcode);
         }    
       } else {
         parseWorker(transformedDefer, gcode);
       }
       
       function parserDataHandler(data){
            var cmd = data.cmd;
            var params = data.params;
            if(cmd){
              var handler = gcodeHandlers[cmd.name] || gcodeHandlers[cmd.code] || defaultHandler;
              handler(cmd, data.line);
            } else if(params){
              parameterHandler(params, data.line)
            } else if(data == 'done'){
              addGeometry();
              console.timeEnd("parsingAndTransfer");
              //console.profileEnd();
              transformedDefer.resolve(group);
            }
           
         }

       function parseWorker(defer, gcode){
         
         KMX.Util.getSingletonWorker("js/import/gcode2three/gcode-worker.js", workerEventHandler)
         .then(
            function(parserWorker) {
              console.time("parsingAndTransfer");
              parserWorker.postMessage({command:'parse',gcode:gcode.lines});
            },function(reason){
              transformedDefer.reject(reason);
            });

         function workerEventHandler(event) {
           parserDataHandler(event.data);
         };
         //console.profile();
       }
       
       function parse(defer, gcode){
          var handler = function codeHandler(obj) {
            parserDataHandler(obj);
          }
          var parser = new GCodeParser(handler,handler);
          parser.parse(gcode.lines);
          parserDataHandler('done');
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
      
  }
  
})();