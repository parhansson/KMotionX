
import {KMXUtil} from '../../util/kmxutil';
import {GCodeVector, GCodeCurve3} from '../vector';
import {GCodeParser} from '../gcode-parser';
import {Block, Word, WordParameters, ControlWord} from '../gcode';
import {Observable, Observer} from 'rxjs/Rx';
import {GCodeSource} from '../igm'
import {ModelTransformer} from './model.transformer';

//Copyright (c) 2014 par.hansson@gmail.com
class ExtendedGCodeVector extends GCodeVector {
  e = 0
  extruding = false
}

class State {
  lastVector = new ExtendedGCodeVector()
  prevMoveCommand: Word = null
  currentMoveCommand: Word = new ControlWord('G', 0)
  scale = 1.0
  absolute = true
  lineGeometry = null as THREE.Geometry
  lineNo = -1

  wordHandlers = {
    M: () => {
    },
    F: () => {
    },
    G: (cmd: Word, line: number) => {
      this.currentMoveCommand = cmd;
      this.lineNo = line;
    },
    S: () => {
    },
    G20: (cmd: Word, line: number) => {
      this.scale = 25.4; //Inches
    },
    G21: (cmd: Word, line: number) => {
      this.scale = 1.0; //mm
    },
    G90: (cmd: Word, line: number) => {
      //absolute
      this.absolute = true;
    },
    G91: (cmd: Word, line: number) => {
      //relative
      this.absolute = false;
    },
    UNKNOWN: (cmd: Word, line: number) => {
      console.info('Unknown command:', cmd.value, cmd, line);
    }
  };
}

export class Gcode2ThreeTransformer extends ModelTransformer<GCodeSource, THREE.Group>{
  // Create the final Object3d to add to the scene
  group: THREE.Group
  interpolateColor = new THREE.Color(0x080808);
  positionColor = new THREE.Color(0xAAAAFF);
  lineMaterial = new THREE.LineBasicMaterial({
    opacity: 0.6,
    transparent: true,
    linewidth: 1,
    vertexColors: THREE.FaceColors
  });
  private state: State

  constructor(private disableWorker?: boolean) { super() }

  execute(gcode: GCodeSource, observer: Observer<THREE.Group>) {
    this.group = new THREE.Group();
    this.group.name = 'GCODE';
    this.state = new State()
    var parser = new GCodeParser()
    parser.subject.subscribe(
      (block) => {
        this.onBlock(block);
      },
      (error) => { observer.error(error) },
      () => {
        observer.next(this.group);
        //transformedDefer.complete();

      })

    parser.parseLines(gcode.lines);
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

  }

  onBlock(block: Block) {
    for (let part of block.parts) {
      if (part instanceof Word) {
        let handler = this.state.wordHandlers[part.value] || this.state.wordHandlers[part.literal] || this.state.wordHandlers.UNKNOWN;
        handler(part, block.line);
      } else if (part instanceof WordParameters) {
        let params = []
        for (let word of part.value) {
          params[word.literal] = word.address
        }
        this.parameterHandler(params, block.line)
        //console.log(word.value)
      }
    }

  }

  private newLine(lineNo: number, group: THREE.Group) {

    let lineGeometry = new THREE.Geometry();
    let line = new THREE.Line(lineGeometry, new THREE.LineBasicMaterial({
      opacity: 0.6,
      transparent: true,
      linewidth: 1,
      vertexColors: THREE.FaceColors
    }));
    line.userData = { lineNo: lineNo }
    group.add(line);
    //console.log("new line");
    return lineGeometry;
  }

  private parameterHandler(args: any, line) {
    let lastVector = this.state.lastVector;
    let lineGeometry = this.state.lineGeometry
    let scale = this.state.scale;
    let absolute = this.state.absolute;
    //Only handle G codes 1-3
    if (this.state.currentMoveCommand.address < 0 || this.state.currentMoveCommand.address > 3) {
      return;
    }

    if (lineGeometry == null
      || this.state.prevMoveCommand == null
      || this.state.prevMoveCommand.value !== this.state.currentMoveCommand.value) {
      this.state.lineGeometry = this.newLine(this.state.lineNo, this.group);
      lineGeometry = this.state.lineGeometry;
    }
    var currentVector = new ExtendedGCodeVector();
    if (absolute) {
      currentVector.x = args.X !== undefined ? args.X * scale : lastVector.x
      currentVector.y = args.Y !== undefined ? args.Y * scale : lastVector.y
      currentVector.z = args.Z !== undefined ? args.Z * scale : lastVector.z
      currentVector.a = args.A !== undefined ? args.A * scale : lastVector.a
      currentVector.b = args.B !== undefined ? args.B * scale : lastVector.b
      currentVector.c = args.C !== undefined ? args.C * scale : lastVector.c
    } else {
      currentVector.x = args.X !== undefined ? args.X * scale + lastVector.x : lastVector.x
      currentVector.y = args.Y !== undefined ? args.Y * scale + lastVector.y : lastVector.y
      currentVector.z = args.Z !== undefined ? args.Z * scale + lastVector.z : lastVector.z
      currentVector.a = args.A !== undefined ? args.A * scale + lastVector.a : lastVector.a
      currentVector.b = args.B !== undefined ? args.B * scale + lastVector.b : lastVector.b
      currentVector.c = args.C !== undefined ? args.C * scale + lastVector.c : lastVector.c
    }

    switch (this.state.currentMoveCommand.address) {
      case (0):
        this.createLine(args, lastVector, currentVector, lineGeometry, this.positionColor);
        break
      case (1):
        this.createLine(args, lastVector, currentVector, lineGeometry, this.interpolateColor);
        //this.createLinePrinter(lastCommand, args, lastVector, currentVector,geometry);
        break
      case (2):
        this.createArc(args, lastVector, currentVector, lineGeometry, this.interpolateColor, true);
        break
      case (3):
        this.createArc(args, lastVector, currentVector, lineGeometry, this.interpolateColor, false);
        break
    }

    this.state.lastVector = currentVector;
    this.state.prevMoveCommand = this.state.currentMoveCommand;
  };



  //use for 3dprinter files
  private createLinePrinter(args, lastVector: ExtendedGCodeVector, currentVector: ExtendedGCodeVector, geometry: THREE.Geometry) {

    currentVector.e = args.E !== undefined ? args.E : lastVector.e,
      //TODO doesn't work as expected due to changing feedrate in the middle of line.
      currentVector.extruding = (currentVector.e - lastVector.e) > 0;
    if (currentVector.extruding) {
      var color = new THREE.Color(currentVector.extruding ? 0xBBFFFF : 0xFF00FF);
      geometry.vertices.push(new THREE.Vector3(lastVector.x, lastVector.y, lastVector.z));
      geometry.vertices.push(new THREE.Vector3(currentVector.x, currentVector.y, currentVector.z));
      geometry.colors.push(color);
      geometry.colors.push(color);
    }
    return currentVector;
  }

  private createLine(args, lastVector: GCodeVector, currentVector: GCodeVector, geometry: THREE.Geometry, color) {

    geometry.vertices.push(new THREE.Vector3(lastVector.x, lastVector.y, lastVector.z));
    geometry.vertices.push(new THREE.Vector3(currentVector.x, currentVector.y, currentVector.z));
    geometry.colors.push(color);
    geometry.colors.push(color);
    return currentVector;
  }

  private createArc(args, lastVector: GCodeVector, currentVector: GCodeVector, geometry: THREE.Geometry, color, clockWise) {

    var curve = new GCodeCurve3(
      lastVector,
      currentVector,
      args,
      clockWise);
    //geometry.vertices.push(new THREE.Vector3(lastVector.x, lastVector.y,lastVector.z));
    var vectors = curve.getPoints(50);
    for (var i = 0; i < vectors.length; i++) {
      geometry.vertices.push(new THREE.Vector3(vectors[i].x, vectors[i].y, vectors[i].z));
    }
    new THREE.Vector3().fromAttribute
    for (var i = 0; i < vectors.length; i++) {
      geometry.colors.push(color);
    }


    return currentVector;
  }

}
