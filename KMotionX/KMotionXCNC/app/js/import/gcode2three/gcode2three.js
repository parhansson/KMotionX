//Copyright (c) 2012 Joe Walnes
//Copyright (c) 2014 par.hansson@gmail.com
var KMX = KMX || {};
KMX.Transformers = KMX.Transformers || {};
KMX.Transformers.gcode2three = {

    transform:  function transform(transformedDefer, gcode, disableWorker){

       // Create the final Object3d to add to the scene
       var group = new THREE.Group();
       var lineGeometry = new THREE.Geometry();
       
       var interpolateColor = new THREE.Color(0x080808);
       var positionColor = new THREE.Color(0xAAAAFF);
       var lineMaterial = new THREE.LineBasicMaterial({
         opacity: 0.6,
         transparent: true,
         linewidth: 1,
         vertexColors: THREE.FaceColors
       });
       
       group.name = 'GCODE';
       group.add(new THREE.Line( lineGeometry, lineMaterial ));

       //Bind this since handler is called from worker context 
       var thisParameterHandler = parameterHandler.bind(this); 
       
       var state = {
          lastVector: {
            x: 0,
            y: 0,
            z: 0,
            a: 0,
            b: 0,
            c: 0,
            e: 0,
            extruding: false
          },
          lastCommand: {
              code:'NONE',
              val: -1
          },
          scale: 1.0,
          absolute: true         
       };
       
       var gcodeHandlers = {
           M: function() {
           },
           F: function() {
           },
           G: function(cmd, line) {
             state.lastCommand = cmd;
           },
           S: function() {
           },
           G20: function(cmd, line) {
             state.scale = 25.4; //Inches
           },
           G21: function(cmd, line) {
             state.scale = 1.0; //mm
           },
           G90: function(cmd, line) {
             //absolute
             state.absolute = true;
           },
           G91: function(cmd, line) {
             //relative
             state.absolute = false;
           }
        };
       
       
         
       
       function parameterHandler(args, line) {
         var lastCommand = state.lastCommand;
         var lastVector = state.lastVector;
         var scale = state.scale;
         var absolute = state.absolute;
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
           lastVector = this.createLine(lastCommand, args, lastVector, currentVector,lineGeometry, positionColor);
         } else if(lastCommand.val == 1 ){
           lastVector = this.createLine(lastCommand, args, lastVector, currentVector,lineGeometry,interpolateColor);
           //lastVector = this.createLinePrinter(lastCommand, args, lastVector, currentVector,geometry);
         } else if(lastCommand.val == 2){
           lastVector = this.createArc(lastCommand, args, lastVector, currentVector,lineGeometry, interpolateColor,true);
         } else if(lastCommand.val == 3){
           lastVector = this.createArc(lastCommand, args, lastVector, currentVector,lineGeometry, interpolateColor,false);
         }
         state.lastVector = currentVector;
       };
     
  
       if(disableWorker){
          // parse without worker. Application will freeze during parsing
          KMX.Util.injectScript("js/import/gcode2three/gcode-parser.js", window.GCodeParser !== undefined)
          .then(
            function() {
              new GCodeParser(parserDataHandler,parserDataHandler).parse(gcode.lines);
              parserDataHandler('done');
            },
            function(reason){
              transformedDefer.reject(reason);
            });  
       } else {
         //Parse with worker in background
          KMX.Util.getSingletonWorker("js/import/gcode2three/gcode-worker.js", 
            function workerEventHandler(event) {
              parserDataHandler(event.data);
            })
          .then(
            function(parserWorker) {
              console.time("parsingAndTransfer");
              parserWorker.postMessage({command:'parse',gcode:gcode.lines});
            },
            function(reason){
              transformedDefer.reject(reason);
            });
         //console.profile();
       }
       

       
       function parserDataHandler(data){
            var cmd = data.cmd;
            var params = data.params;
            if(cmd){
              var handler = gcodeHandlers[cmd.name] || gcodeHandlers[cmd.code] || defaultHandler;
              handler(cmd, data.line);
            } else if(params){
              thisParameterHandler(params, data.line)
            } else if(data == 'done'){
              console.timeEnd("parsingAndTransfer");
              //console.profileEnd();
              transformedDefer.resolve(group);
            } 
            
            function defaultHandler(args, info) {
                console.info('Unknown command:', args.name, args, info);
            }
         }
    },
      
    //use for 3dprinter files
    createLinePrinter: function createLinePrinter(cmd, args, lastVector, currentVector, geometry){
  
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
    },
    
    createLine: function createLine(cmd, args, lastVector, currentVector,geometry,color){
  
      
      geometry.vertices.push(new THREE.Vector3(lastVector.x, lastVector.y,lastVector.z));
      geometry.vertices.push(new THREE.Vector3(currentVector.x, currentVector.y,currentVector.z));
      geometry.colors.push(color);
      geometry.colors.push(color);
      
      
      return currentVector;
    },
    
    createArc: function createArc(cmd, args, lastVector, currentVector,geometry, color,clockWise){
  
      var curve = new GCode.Curve3(
          lastVector,
          currentVector,
          args,
          clockWise);    
      //geometry.vertices.push(new THREE.Vector3(lastVector.x, lastVector.y,lastVector.z));
      var vectors = curve.getPoints( 50 );
      geometry.vertices.push.apply(geometry.vertices,vectors);
      for(var i = 0; i < vectors.length; i++){
        geometry.colors.push(color);
      }
      
      
      return currentVector;
    }
}
