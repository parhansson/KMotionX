import { IGM, GCodeSource } from '../igm';
import { SVGModelSettings } from '../model.settings.service'
import { ModelTransformer } from './model.transformer';
import { Observer } from 'rxjs/Rx'


class GCodeOutput {
  code: string[] = [];
  filter: boolean

  constructor(filter: boolean) {
    this.filter = filter
  }
  push(gc: string[] | string) {
    var line
    //TODO handle other types as well
    if (Array.isArray(gc)) {
      line = gc.join(' ');
    } else {
      line = gc;
    }
    if (this.filter) {
      //filter duplicate lines
      if (this.code[this.code.length - 1] !== line) {
        this.code.push(line);
      }
    } else {
      this.code.push(line);
    }
  }
}

export class Igm2GcodeTransformer extends ModelTransformer<IGM, GCodeSource>{

  constructor(private settings: SVGModelSettings) {
    super()
  }

  execute(igm: IGM, targetObserver: Observer<GCodeSource>) {

    let passWidth = this.settings.materialWidth / this.settings.passes;
    let settings = this.settings
    //settings.seekRate = settings.seekRate || 800;
    //settings.bitWidth = settings.bitWidth || 1; // in mm 
    //settings.dpi = settings.dpi || null;

    var dpiScale = 1;
    var mmPerInch = 25.4;

    if (settings.dpi) {
      if (settings.unit == 'mm') {
        dpiScale = mmPerInch / settings.dpi;
      } else if (settings.unit == 'in') {
        dpiScale = 1 / (settings.dpi);
      }
    }

    var scaleNoDPI = function (val) {
      return format(val * settings.scale);
    }
    var ratio = settings.scale * dpiScale;

    var format = function (numb) {
      //fix fractional digits
      numb = +numb.toFixed(settings.fractionalDigits);
      // Note the plus sign that drops any 'extra' zeroes at the end.
      // It changes the result (which is a string) into a number again (think '0 + foo'),
      // which means that it uses only as many digits as necessary.
      return numb;
    }
    var paths = igm.applyModifications(settings, ratio);
    //var paths = igm.alllayers;

    var LaserON = 'M3 (laser on)';
    var LaserOFF = 'M5 (laser off)';
    //var LaserON = '(BUF,SetBitBuf14)';
    //var LaserOFF = '(BUF,ClearBitBuf14)';
    //G20 Inch units
    //G21 mm units
    var gcode = new GCodeOutput(true);


    var maxBounds = igm.getMaxBounds(paths);
    function describe(rect) {
      return 'Width: ' + format(rect.width()) + ' Height: ' + format(rect.height()) + ' Area: ' + format(rect.area());
    }

    gcode.push('(' + describe(maxBounds) + ')');

    gcode.push('G90'); //Absolute Coordinates
    if (settings.unit == 'mm') {
      gcode.push('G21');
    } else if (settings.unit == 'in') {
      gcode.push('G22');
    }
    if (settings.initCode) {
      gcode.push(settings.initCode);

    }
    //gcode.push('G1 Z' + scaleNoDPI(settings.safeZ), 'M4');
    gcode.push('F' + settings.feedRate);
    gcode.push('G0 Z' + scaleNoDPI(settings.safeZ));
    for (var pathIdx = 0, pathLength = paths.length; pathIdx < pathLength; pathIdx++) {
      var igmObj = paths[pathIdx];
      var path = igmObj.vectors;
      //add scale and transform
      var startVec = path[0];//.add(translateVec,true);

      gcode.push(LaserOFF);
      //gcode.push('F' + settings.seekRate);
      // seek to index 0
      //gcode.push('N 100 ');
      gcode.push([
        'G0',
        'X' + format(startVec.x),
        'Y' + format(startVec.y)/*,
        'F' + settings.seekRate*/
      ]);

      for (var p = passWidth; p <= settings.materialWidth; p += passWidth) {
        gcode.push('(Forward pass depth ' + p + ')');
        // begin the cut by dropping the tool to the work
        gcode.push([
          'G0',
          'Z' + scaleNoDPI(settings.cutZ + p)
        ]);

        gcode.push('(' + describe(igmObj.bounds) + ')');

        gcode.push(LaserON);

        // keep track of the current path being cut, as we may need to reverse it
        var localPath = [];
        for (var segmentIdx = 0, segmentLength = path.length; segmentIdx < segmentLength; segmentIdx++) {
          //add transform
          var segment = path[segmentIdx];//.add(translateVec,true);

          var localSegment = [
            'G1',
            'X' + format(segment.x),
            'Y' + format(segment.y)/*,
            'F' + settings.feedRate*/
          ].join(' ');

          // feed through the material
          gcode.push(localSegment);
          localPath.push(localSegment);

          // if the path is not closed, reverse it, drop to the next cut depth and cut
          // this handles lines
          if (segmentIdx === segmentLength - 1) {

            if (segment.x !== startVec.x || segment.y !== startVec.y) {

              p += passWidth;
              if (p <= settings.materialWidth) {
                gcode.push('(Reverse pass depth ' + p + ')');
                // begin the cut by dropping the tool to the work
                gcode.push(['G0',
                  'Z' + scaleNoDPI(settings.cutZ + p)
                ]);
                //this fails now
                //Array.prototype.push.apply(gcode, localPath.reverse());
                //TODO  Must use something like this. or even better use push function on  
                Array.prototype.push.apply(gcode.code, localPath.reverse());
              }
            }

          }
        }
      }
      gcode.push(LaserOFF);
      // go safe
      gcode.push(['G0',
        'Z' + scaleNoDPI(settings.safeZ)
      ]);
    }

    // just wait there for a second
    gcode.push('G4 P1');

    // turn off the spindle
    gcode.push('M5');

    // go home
    gcode.push('G0 Z0 F300');
    gcode.push('G0 X0 Y0 F800');
    gcode.push('M2');

    targetObserver.next(new GCodeSource(gcode.code))

  }
}


