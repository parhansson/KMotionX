
import { KMXUtil } from '../../util/kmxutil';
import { GCodeVector, GCodeCurve3, MoveArcArguments, MoveAngularArguments, MoveArguments } from '../vector';
import { GCodeParser } from '../gcode-parser';
import { Block, Word, WordParameters, ControlWord } from '../gcode';
import { Observable, Observer } from 'rxjs/Rx';
import { GCodeSource } from '../igm'
import { ModelTransformer } from './model.transformer';

//Copyright (c) 2014 par.hansson@gmail.com
//Modal G codes 
// Group 1	{G0, G1, G2, G3, G80, G81, G82, G83, G84, G85, G86, G87, G88, G89} - motion
// Group 2	{G17, G18, G19} - plane selection
// Group 3	{G90, G91} - distance mode
// Group 5	{G93, G94} - spindle speed mode
// Group 6	{G20, G21} - units
// Group 7	{G40, G41, G42} - cutter diameter compensation
// Group 8	{G43, G49} - tool length offset
// Group 10	{G98, G99} - return mode in canned cycles
// Group 12	{G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3} coordinate system selection
//Modal M codes 
// Group 2	{M26, M27} - axis clamping
// Group 4	{M0, M1, M2, M30, M60} - stopping
// Group 6	{M6} - tool change
// Group 7	{M3, M4, M5} - spindle turning
// Group 8	{M7, M8, M9} - coolant
// Group 9	{M48, M49} - feed and speed override bypass

export class ModalGroup {
  constructor(initialState: string, groupCodes?: string[]) {
    this.code = initialState
  }
  changed: boolean
  code: string
  setActiveCode(newCode: string) {
    //TODO throw error if state literal is not allowed in this group
    //also check line number (block) if two or more states are set in the same block
    // this is not allowed
    this.changed = this.code !== newCode
    this.code = newCode
  }
}
//Kan användas vid generering av gcode i igm-gcode tranformern för att validera koden när den skapas
//och hålla koll på positionen för att undvika att shapen gör en G1 rad på samma ställe som nyligen G0
export class GCodeState {
  moveGroup = new ModalGroup('G0', ['G0', 'G1', 'G2', 'G3', 'G80', 'G81', 'G82', 'G83', 'G84', 'G85', 'G86', 'G87', 'G88', 'G89'])
  planeGroup = new ModalGroup('G17', ['G17', 'G18', 'G19'])
  //distance mode defaults to absolut
  distanceGroup = new ModalGroup('G90', ['G90', 'G91'])
  //spindle speed mode
  spindleSpeedGroup = new ModalGroup(null, ['G93', 'G94'])
  //units defaults to mm
  unitsGroup = new ModalGroup('G21', ['G20', 'G21'])
  position = new GCodeVector()

}
export class State<ShapeType> extends GCodeState {
  //Motion group

  scale = 1.0
  absolute = true
  currentShape: ShapeType = null
  lineNo = -1
  onBlock(block: Block) {
    this.lineNo = block.line
    this.moveGroup.changed = false;
    this.unitsGroup.changed = false;
    //etc
  }
  handleWord(cmd: Word) {
    const handler = this.wordHandlers[cmd.value] || this.wordHandlers[cmd.literal] || this.wordHandlers.UNKNOWN;
    handler(cmd);
  }
  private wordHandlers = {
    M: (cmd: Word) => {
    },
    F: (cmd: Word) => {
    },
    G0: (cmd: Word) => {
      this.moveGroup.setActiveCode(cmd.value)
    },
    G1: (cmd: Word) => {
      this.moveGroup.setActiveCode(cmd.value)
    },
    G2: (cmd: Word) => {
      this.moveGroup.setActiveCode(cmd.value)
    },
    G3: (cmd: Word) => {
      this.moveGroup.setActiveCode(cmd.value)
    },
    S: (cmd: Word) => {
    },
    G20: (cmd: Word) => {
      this.unitsGroup.setActiveCode(cmd.value)
      this.scale = 25.4; //Inches
    },
    G21: (cmd: Word) => {
      this.unitsGroup.setActiveCode(cmd.value)
      this.scale = 1.0; //mm
    },
    G90: (cmd: Word) => {
      //absolute
      this.absolute = true;
    },
    G91: (cmd: Word) => {
      //relative
      this.absolute = false;
    },
    UNKNOWN: (cmd: Word) => {
      console.info('Unsupported command:', cmd.value, cmd, this.lineNo);
    }
  };
}

export abstract class GCodeTransformer<ShapeType, OutputType> extends ModelTransformer<GCodeSource, OutputType>{
  // Create the final Object3d to add to the scene
  output: OutputType

  protected state: State<ShapeType>

  constructor(protected disableWorker?: boolean) { super() }
  protected abstract createOutput(): OutputType
  protected abstract createShapeType(): ShapeType
  protected abstract addLineShapePosition(newPosition: GCodeVector, shape: ShapeType)
  protected abstract createCurveShape(curve: GCodeCurve3, shape: ShapeType)

  execute(gcode: GCodeSource, observer: Observer<OutputType>) {
    this.output = this.createOutput();
    //this.group.name = 'GCODE';
    this.state = new State<ShapeType>()
    //TODO parsing should be done outside of this transformer
    //this transformer should Subject<Block> instead of GCodeSource
    const parser = new GCodeParser()
    parser.subject.subscribe(
      (block) => {
        this.onBlock(block);
      },
      (error) => { observer.error(error) },
      () => {
        observer.next(this.output);
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

  private onBlock(block: Block) {
    //A Block is one line of gcode
    //a Word is a code (G0, G1, M1 etc)
    //WordParameters are parameters to Words (X93 Y87 P4 etc)
    //one block does not need to contain the actual word
    // hence this is valid gcode
    // G1 X10 Y10
    // X5 Y5
    // Move on second row is made in G1 mode
    this.state.onBlock(block)
    for (let part of block.parts) {
      if (part instanceof Word) {
        this.state.handleWord(part)
      } else if (part instanceof WordParameters) {
        const params: MoveArguments = {}
        for (let word of part.value) {
          params[word.literal] = word.address
        }
        this.onWordParameter(params)
        //console.log(word.value)
      }
    }

  }

  protected onWordParameter(args: MoveArguments) {


    const newPosition = this.getNewPosition(args)
    if (newPosition == null) {
      return
    }

    let currentShape = this.state.currentShape
    const position = this.state.position;


    if (this.state.moveGroup.changed || currentShape == null) {
      currentShape = this.state.currentShape = this.createShapeType();
      if (this.state.moveGroup.code === 'G0' || this.state.moveGroup.code === 'G1') {
        //det här kan man inte göra på en curve
        this.addLineShapePosition(position, currentShape);
      }
    }

    //TODO if args X Y and Z is undefined then there is no vector in this Parameter object

    switch (this.state.moveGroup.code) {
      case ('G0'):
        this.addLineShapePosition(newPosition, currentShape);
        break
      case ('G1'):
        this.addLineShapePosition(newPosition, currentShape);
        break
      case ('G2'):
        this.addCurve(args, position, newPosition, true, currentShape);
        break
      case ('G3'):
        this.addCurve(args, position, newPosition, false, currentShape);
        break
    }

    this.state.position = newPosition;
  };
  private addCurve(args: MoveArcArguments, position: GCodeVector, newPosition: GCodeVector, clockWise: boolean, currentShape) {
    let curve = new GCodeCurve3(
      position,
      newPosition,
      args,
      clockWise);
    this.createCurveShape(curve, currentShape);
  }
  private getNewPosition(args: MoveArguments) {
    if (!this.containsMoveData(args)) {
      return null;
    }
    const position = this.state.position;
    const scale = this.state.scale;
    const absolute = this.state.absolute;
    const newPosition = new GCodeVector();
    if (absolute) {
      newPosition.x = args.X !== undefined ? args.X * scale : position.x
      newPosition.y = args.Y !== undefined ? args.Y * scale : position.y
      newPosition.z = args.Z !== undefined ? args.Z * scale : position.z
      newPosition.a = args.A !== undefined ? args.A * scale : position.a
      newPosition.b = args.B !== undefined ? args.B * scale : position.b
      newPosition.c = args.C !== undefined ? args.C * scale : position.c
    } else {
      newPosition.x = args.X !== undefined ? args.X * scale + position.x : position.x
      newPosition.y = args.Y !== undefined ? args.Y * scale + position.y : position.y
      newPosition.z = args.Z !== undefined ? args.Z * scale + position.z : position.z
      newPosition.a = args.A !== undefined ? args.A * scale + position.a : position.a
      newPosition.b = args.B !== undefined ? args.B * scale + position.b : position.b
      newPosition.c = args.C !== undefined ? args.C * scale + position.c : position.c
    }
    return newPosition
  }
  private containsMoveData(args: any) {
    return args.X !== undefined || args.Y !== undefined || args.Z !== undefined || args.A !== undefined || args.B !== undefined || args.C !== undefined
  }
}
