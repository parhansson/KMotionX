/// <reference path="../../../typings/main/ambient/three/index.d.ts" />
import {KMXUtil} from '../../util/kmxutil';
import {GCodeCurve3,GCodeVector} from '../igm/igm';
import {GCodeParser} from './gcode.parser';
import {Observable} from 'rxjs/Observable';
import {Observer} from 'rxjs/Observer';

//Copyright (c) 2014 par.hansson@gmail.com
export class ExtendedGCodeVector extends GCodeVector {
  e = 0
  extruding = false
}

export class Gcode2Three {
  constructor() {

  }

  transform(transformedDefer: Observer<any>, gcode, disableWorker: boolean) {

    // Create the final Object3d to add to the scene
    var group = new THREE.Group();
    //var lineGeometry = new THREE.Geometry();

    var interpolateColor = new THREE.Color(0x080808);
    var positionColor = new THREE.Color(0xAAAAFF);
    var lineMaterial = new THREE.LineBasicMaterial({
      opacity: 0.6,
      transparent: true,
      linewidth: 1,
      vertexColors: THREE.FaceColors
    });

    group.name = 'GCODE';
    //group.add(new THREE.Line( lineGeometry, lineMaterial ));

    //Bind this since handler is called from worker context 
    var thisParameterHandler = parameterHandler.bind(this);

    var state = {
      lastVector: new ExtendedGCodeVector(),
      prevCommand: null,
      lastCommand: {
        code: 'NONE',
        val: -1
      },
      scale: 1.0,
      absolute: true,
      lineGeometry: null as THREE.Geometry,
      lineNo: -1
    };

    var gcodeHandlers = {
      M: ()=> {
      },
      F: ()=> {
      },
      G: (cmd, line)=> {
        state.lastCommand = cmd;
        state.lineNo = line;
      },
      S: ()=> {
      },
      G20: (cmd, line)=> {
        state.scale = 25.4; //Inches
      },
      G21: (cmd, line)=> {
        state.scale = 1.0; //mm
      },
      G90: (cmd, line)=> {
        //absolute
        state.absolute = true;
      },
      G91: (cmd, line)=> {
        //relative
        state.absolute = false;
      }
    };

    function newLine(lineNo) {
      var lineMaterial2 = new THREE.LineBasicMaterial({
        opacity: 0.6,
        transparent: true,
        linewidth: 1,
        vertexColors: THREE.FaceColors
      });
      var lineGeometry = new THREE.Geometry();
      var line = new THREE.Line(lineGeometry, lineMaterial2);
      line.userData = { lineNo: lineNo }
      group.add(line);
      console.log("new line");
      return lineGeometry;
    }


    function parameterHandler(args, line) {
      var lastCommand = state.lastCommand;
      var prevCommand = state.prevCommand;
      var lastVector = state.lastVector;
      var lineGeometry = state.lineGeometry
      var scale = state.scale;
      var absolute = state.absolute;
      //Only handle G codes 1-3
      if (lastCommand.code != 'G' || lastCommand.val < 0 || lastCommand.val > 3) return;
      if (lineGeometry == null || prevCommand == null || prevCommand.code !== lastCommand.code || prevCommand.val !== lastCommand.val) {
        state.lineGeometry = newLine(state.lineNo);
        lineGeometry = state.lineGeometry;
      }
      var currentVector = new ExtendedGCodeVector();
      if (absolute) {
        currentVector.X = args.X !== undefined ? args.X * scale : lastVector.X
        currentVector.Y = args.Y !== undefined ? args.Y * scale : lastVector.Y
        currentVector.Z = args.Z !== undefined ? args.Z * scale : lastVector.Z
        currentVector.A = args.A !== undefined ? args.A * scale : lastVector.A
        currentVector.B = args.B !== undefined ? args.B * scale : lastVector.B
        currentVector.C = args.C !== undefined ? args.C * scale : lastVector.C
      } else {
        currentVector.X = args.X !== undefined ? args.X * scale + lastVector.X : lastVector.X
        currentVector.Y = args.Y !== undefined ? args.Y * scale + lastVector.Y : lastVector.Y
        currentVector.Z = args.Z !== undefined ? args.Z * scale + lastVector.Z : lastVector.Z
        currentVector.A = args.A !== undefined ? args.A * scale + lastVector.A : lastVector.A
        currentVector.B = args.B !== undefined ? args.B * scale + lastVector.B : lastVector.B
        currentVector.C = args.C !== undefined ? args.C * scale + lastVector.C : lastVector.C
      }

      if (lastCommand.val == 0) {
        lastVector = this.createLine(lastCommand, args, lastVector, currentVector, lineGeometry, positionColor);
      } else if (lastCommand.val == 1) {
        lastVector = this.createLine(lastCommand, args, lastVector, currentVector, lineGeometry, interpolateColor);
        //lastVector = this.createLinePrinter(lastCommand, args, lastVector, currentVector,geometry);
      } else if (lastCommand.val == 2) {
        lastVector = this.createArc(lastCommand, args, lastVector, currentVector, lineGeometry, interpolateColor, true);
      } else if (lastCommand.val == 3) {
        lastVector = this.createArc(lastCommand, args, lastVector, currentVector, lineGeometry, interpolateColor, false);
      }
      state.lastVector = currentVector;
      state.prevCommand = { code: lastCommand.code, val: lastCommand.val };
    };

    new GCodeParser(parserDataHandler, parserDataHandler).parse(gcode.lines);
    parserDataHandler('done');
    /*
        if (disableWorker) {
          // parse without worker. Application will freeze during parsing
          KMXUtil.injectScript("js/import/gcode2three/gcode-parser.js", window.GCodeParser] !== undefined)
            .then(
            function() {
              new GCodeParser(parserDataHandler, parserDataHandler).parse(gcode.lines);
              parserDataHandler('done');
            },
            function(reason) {
              transformedDefer.reject(reason);
            });
        } else {
          //Parse with worker in background
          KMXUtil.getSingletonWorker("js/import/gcode2three/gcode-worker.js",
            function workerEventHandler(event) {
              parserDataHandler(event.data);
            })
            .then(
            function(parserWorker) {
              console.time("parsingAndTransfer");
              parserWorker.postMessage({ command: 'parse', gcode: gcode.lines });
            },
            function(reason) {
              transformedDefer.reject(reason);
            });
          //console.profile();
        }
    */


    function parserDataHandler(data) {
      var cmd = data.cmd;
      var params = data.params;
      if (cmd) {
        var handler = gcodeHandlers[cmd.name] || gcodeHandlers[cmd.code] || defaultHandler;
        handler(cmd, data.line);
      } else if (params) {
        thisParameterHandler(params, data.line)
      } else if (data == 'done') {
        console.timeEnd("parsingAndTransfer");
        //console.profileEnd();
        transformedDefer.next(group);
        transformedDefer.complete();
      }

      function defaultHandler(args, info) {
        console.info('Unknown command:', args.name, args, info);
      }
    }
  }

  //use for 3dprinter files
  createLinePrinter(cmd, args, lastVector: ExtendedGCodeVector, currentVector:ExtendedGCodeVector, geometry:THREE.Geometry) {

    currentVector.e = args.E !== undefined ? args.E : lastVector.e,
      //TODO doesn't work as expected due to changing feedrate in the middle of line.
      currentVector.extruding = (currentVector.e - lastVector.e) > 0;
    if (currentVector.extruding) {
      var color = new THREE.Color(currentVector.extruding ? 0xBBFFFF : 0xFF00FF);
      geometry.vertices.push(new THREE.Vector3(lastVector.X, lastVector.Y, lastVector.Z));
      geometry.vertices.push(new THREE.Vector3(currentVector.X, currentVector.Y, currentVector.Z));
      geometry.colors.push(color);
      geometry.colors.push(color);
    }
    return currentVector;
  }

  createLine(cmd, args, lastVector:GCodeVector, currentVector:GCodeVector, geometry:THREE.Geometry, color) {

    geometry.vertices.push(new THREE.Vector3(lastVector.X, lastVector.Y, lastVector.Z));
    geometry.vertices.push(new THREE.Vector3(currentVector.X, currentVector.Y, currentVector.Z));
    geometry.colors.push(color);
    geometry.colors.push(color);
    return currentVector;
  }

  createArc(cmd, args, lastVector:GCodeVector, currentVector:GCodeVector, geometry:THREE.Geometry, color, clockWise) {

    var curve = new GCodeCurve3(
      lastVector,
      currentVector,
      args,
      clockWise);
    //geometry.vertices.push(new THREE.Vector3(lastVector.x, lastVector.y,lastVector.z));
    var vectors = curve.getPoints(50);
    geometry.vertices.push.apply(geometry.vertices, vectors);
    for (var i = 0; i < vectors.length; i++) {
      geometry.colors.push(color);
    }


    return currentVector;
  }
}
