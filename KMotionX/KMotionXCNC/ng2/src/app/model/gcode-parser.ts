// Copyright (c) 2016 par.hansson@gmail.com
/**
 * Parses a string of gcode instructions, and invokes codeHandlers for each type of
 * command or values.
 */
import { Subject } from 'rxjs/Rx';
import {
  Word,
  BlockPart,
  Block,
  WordParameters,
  Comment,
  ControlWord,
  ParamWord
} from './gcode'


class ParseValue {
  static None = 0
  static Comment = 1
  static ControlWord = 2
  static ParamWord = 3

  val: string = ''
  letter: string = null
  type: number = ParseValue.None

  constructor() { }

  toPart() {
    switch (this.type) {
      case (ParseValue.Comment):
        return new Comment(this.val)
      case (ParseValue.ParamWord):
        return new ParamWord(this.letter, parseFloat(this.val));
      case (ParseValue.ControlWord):
        return new ControlWord(this.letter, parseFloat(this.val));
    }
  }
  next(block: Block, nextType: number, nextLetter?: string) {

    if (this.type === ParseValue.ParamWord) {
      (block.parts[block.parts.length - 1] as WordParameters).value.push(this.toPart() as Word)
    } else if (this.type !== ParseValue.None) {
      block.parts.push(this.toPart())
    }

    if (ParseValue.ParamWord === nextType && this.type !== ParseValue.ParamWord) {
      block.parts.push(new WordParameters())
    }

    this.letter = nextLetter
    this.type = nextType
    this.val = ''
  }
}

export class GCodeParser {
  // Search for codes without space between them
  public static skipCodes = {
    'N': 'Line number'
  };
  public static controlWords = {
    'G': 'G codes, Interpolate, rapid traverse etc',
    'M': 'M code',
    'F': 'Set Feed rate in/min or mm/min',
    'S': 'Spindle Speed',
    'D': 'Tool',
    'O': 'Subroutine Label'
  };
  public static paramWords = {
    'X': 'X axis',
    'Y': 'Y axis',
    'Z': 'Z axis',
    'A': 'A axis',
    'B': 'B axis',
    'C': 'C axis',
    //'E': 'E axis present in 3d printer gcode files',
    'I': 'I parameter on G2,G3',
    'J': 'J parameter on G2,G3',
    'K': 'K parameter on G2,G3',
    'R': 'R parameter on G2,G3',
    'L': 'G10 parameter',
    'P': 'G10 parameter'
  }

  private static valueChars = '+-0123456789.';
  private static blockCommentDepth = {
    '(': 1,
    ';': Infinity
  };

  private static blockCommentEnd = ')';

  public subject: Subject<Block>;


  constructor() {
    this.subject = new Subject<Block>()
  }

  parseLine(rawText: string) {
    let block = new Block(rawText);
    let text = rawText.toUpperCase();
    let len = text.length
    let part = new ParseValue()
    let commentDepth = 0;
    for (var x = 0; x < len; x++) {
      let c = text.charAt(x)
      if (commentDepth == 0 && (commentDepth += (GCodeParser.blockCommentDepth[c] || 0)) > 0) {
        part.next(block, ParseValue.Comment)
      } else if (c === GCodeParser.blockCommentEnd) {
        commentDepth -= 1
        part.val += c
        continue
      }
      if (commentDepth > 0) {
        part.val += c
      } else if (GCodeParser.valueChars.indexOf(c) >= 0) {
        part.val += c
      } else if (GCodeParser.controlWords[c]) {
        part.next(block, ParseValue.ControlWord, c)
      } else if (GCodeParser.paramWords[c]) {
        part.next(block, ParseValue.ParamWord, c)
      } else if (GCodeParser.skipCodes[c]) {
        part.next(block, ParseValue.Comment, c)
      } else {
        if (c !== ' ' && c !== '\r') {
          console.log('Strange unhandled character?', '"' + c + '"', c.charCodeAt(0))
        }
      }
    }
    part.next(block, ParseValue.None)
    return block
  }

  parseLines(gcodeLines: string[]) {
    console.time('parsing');
    let i = 0;
    for (let line of gcodeLines) {
      let block = this.parseLine(line)
      block.line = i++
      this.subject.next(block);

    }
    this.subject.complete();
    console.timeEnd('parsing');

  }

}

