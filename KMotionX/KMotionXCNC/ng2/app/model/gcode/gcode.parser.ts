// Copyright (c) 2014 par.hansson@gmail.com
/**
 * Parses a string of gcode instructions, and invokes codeHandlers for each type of
 * command or values.
 */
export interface ParamCallback {
  (n: any): any;
}

export interface CodeCallback {
  (n: any): any;
}

export class GCodeCmd {
  code: string
  val: any


  constructor(code: string, val: any) {
    this.code = code;
    this.val = val;

  }
  done = function() {
    this.val = parseFloat(this.val);
    this.name = this.code + this.val;
  }
}

export class GCodeParser {
  static State = {

    NONE: 0,
    CONTROL_VAL: 1,
    PARAM_VAL: 2,
    SKIP_VAL: 3
  }
  cmd: GCodeCmd
  values: any
  // Search for codes without space between them
  skipCodes = {
    'N': "Line number"
  };
  controlCodes = {
    'G': "G codes, Interpolate, rapid traverse etc",
    'M': "M code",
    'F': "Set Feed rate in/min or mm/min",
    'S': "Spindle Speed",
    'D': "Tool",
    'O': "Subroutine Label"
  };
  paramCodes = {
    'X': "X axis",
    'Y': "Y axis",
    'Z': "Z axis",
    'A': "A axis",
    'B': "B axis",
    'C': "C axis",
    'E': "E axis present in 3d printer gcode files",
    'I': "I parameter on G2,G3",
    'J': "J parameter on G2,G3",
    'K': "K parameter on G2,G3",
    'R': "R parameter on G2,G3",
    'L': "G10 parameter",
    'P': "G10 parameter"
  }
  
  valueChars = '+-0123456789.';
  codeHandler: CodeCallback
  paramHandler: ParamCallback

  constructor(codeHandler: CodeCallback, paramHandler: ParamCallback) {

    this.paramHandler = paramHandler || function(dummy: any) { };
    this.codeHandler = codeHandler || function(dummy: any) { };
  }
  
  handleCode(cmd, line) {
    this.codeHandler({ cmd: cmd, line: line })
  }
  
  handleParam(values, line) {
    this.paramHandler({ params: values, line: line })
  }

  flush(line) {
    if (this.cmd !== undefined) {
      this.cmd.done();
      //this.handle();
      this.handleCode(this.cmd, line);
      // console.info("handle", this.cmd.name);
      delete this.cmd;
    }
    if (this.values !== undefined) {
      for (var key in this.values) {
        this.values[key] = parseFloat(this.values[key]);
      }
      var values = this.values;

      this.handleParam(this.values, line);
      // console.info("params ", JSON.stringify(this.values));
      delete this.values;
    }
  }

  parseLine(_text, line) {

    var state = GCodeParser.State.NONE;
    var currentControlCode = '';

    _text = _text.replace(/;.*$/, ''); // Remove comments
    _text = _text.replace(/\(.*\)/, '').trim(); // Remove comments
    var text = _text.toUpperCase();
    var arr = text.split("");
    var len = _text.length;
    for (var x = 0; x < len; x++) {
      var c = arr[x];

      // Skip white space
      if (c === ' ') {
        continue;
      }
      if (this.controlCodes[c]) { // If control code G M F S etc flush previous
        // control command if present
        this.flush(line);
        this.cmd = new GCodeCmd(c, '');
        // Set state to parse control values
        state = GCodeParser.State.CONTROL_VAL;
      } else if (this.paramCodes[c]) { // If parameter codes X Y Z etc
        currentControlCode = c;
        if (this.values === undefined) {
          this.values = {};
        }
        this.values[currentControlCode] = ''; // store code even if not values are
        // found later on.
        // Set state to parse parameter values
        state = GCodeParser.State.PARAM_VAL;
      } else if (this.valueChars.indexOf(c) >= 0) {
        if (state == GCodeParser.State.CONTROL_VAL) {
          this.cmd.val += c;
        } else if (state == GCodeParser.State.PARAM_VAL) {
          this.values[currentControlCode] += c;
        } else if (state == GCodeParser.State.SKIP_VAL) {
          // do nothing.
        }

      } else if (this.skipCodes[c]) {
        // If skip code like N(line number) set state to skip values
        state = GCodeParser.State.SKIP_VAL;
      }
    }
    this.flush(line);
  }

  parse(gcodeLines) {
    console.time("parsing");
    for (var i = 0; i < gcodeLines.length; i++) {
      this.parseLine(gcodeLines[i],i)
    // if (this.parseLine(gcodeLines[i], i) === false) {
    //   break;
    // }
    }
    console.timeEnd("parsing");
  }

}

