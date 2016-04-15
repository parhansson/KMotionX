// Copyright (c) 2014 par.hansson@gmail.com
/**
 * Parses a string of gcode instructions, and invokes codeHandlers for each type of
 * command or values.
 */
var GCodeParserState = {    
  NONE: 0,
  CONTROL_VAL: 1,
  PARAM_VAL: 2,
  SKIP_VAL: 3
};
function GCodeParser(codeHandler, paramHandler) {
  
  paramHandler = paramHandler || function() {};
  codeHandler = codeHandler || function() {};

  this.handleCode = function(cmd,line) {
    codeHandler({cmd:cmd,line:line});
  }
  this.handleParam = function(values, line){
    paramHandler({params:values,line:line});
  
  }

  // Search for codes without space between them
  this.skipCodes = {
    'N': "Line number"
  };
  this.controlCodes = {
    'G': "G codes, Interpolate, rapid traverse etc",
    'M': "M code",
    'F': "Set Feed rate in/min or mm/min",
    'S': "Spindle Speed",
    'D': "Tool",
    'O': "Subroutine Label"
  };
  this.paramCodes = {
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
  };
  this.valueChars = '+-0123456789.';
}

function GCodeCmd(code, val) {
  this.code = code;
  this.val = val;

}
GCodeCmd.prototype.done = function() {
  this.val = parseFloat(this.val);
  this.name = this.code + this.val;
}

GCodeParser.prototype.flush = function(line) {
  if (this.cmd !== undefined) {
    this.cmd.done();
    //this.handle();
    this.handleCode(this.cmd,line);
    // console.info("handle", this.cmd.name);
    delete this.cmd;
  }
  if (this.values !== undefined) {
    for ( var key in this.values) {
      this.values[key] = parseFloat(this.values[key]);
    }
    var values = this.values;
    
    this.handleParam(this.values, line);
    // console.info("params ", JSON.stringify(this.values));
    delete this.values;
  }
}

GCodeParser.prototype.parseLine = function(_text, line) {

  var state = GCodeParserState.NONE;
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
      state = GCodeParserState.CONTROL_VAL;
    } else if (this.paramCodes[c]) { // If parameter codes X Y Z etc
      currentControlCode = c;
      if (this.values === undefined) {
        this.values = {};
      }
      this.values[currentControlCode] = ''; // store code even if not values are
                                            // found later on.
      // Set state to parse parameter values
      state = GCodeParserState.PARAM_VAL;
    } else if (this.valueChars.indexOf(c) >= 0) {
      if (state == GCodeParserState.CONTROL_VAL) {
        this.cmd.val += c;
      } else if (state == GCodeParserState.PARAM_VAL) {
        this.values[currentControlCode] += c;
      } else if (state == GCodeParserState.SKIP_VAL) {
        // do nothing.
      }

    } else if (this.skipCodes[c]) {
      // If skip code like N(line number) set state to skip values
      state = GCodeParserState.SKIP_VAL;
    }
  }
  this.flush(line);
};

GCodeParser.prototype.parse = function(gcodeLines) {
  console.time("parsing");
  for (var i = 0; i < gcodeLines.length; i++) {
    if (this.parseLine(gcodeLines[i], i) === false) {
      break;
    }
  }
  console.timeEnd("parsing");
};
