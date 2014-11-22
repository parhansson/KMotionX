function svg2gcode(svg, settings) {
  // clean off any preceding whitespace
  svg = svg.replace(/^[\n\r \t]/gm, '');
  settings = settings || {};
  settings.passes = settings.passes || 1;
  settings.materialWidth = settings.materialWidth || 6;
  settings.passWidth = settings.materialWidth/settings.passes;
  settings.scale = settings.scale || -1;
  settings.cutZ = settings.cutZ || -108; // cut z
  settings.safeZ = settings.safeZ || -106;   // safe z
  settings.feedRate = settings.feedRate || 1400;
  settings.seekRate = settings.seekRate || 1100;
  settings.bitWidth = settings.bitWidth || 1; // in mm
  settings.fractionalDigits = settings.fractionalDigits || 3; //Thousands will do 
  settings.unit = settings.unit || "mm";
  //settings.dpi = settings.dpi || null;
  
  var dpiScale = 1;
  var mmPerInch = "25.4";
  if(settings.dpi){
    if(settings.unit == "mm"){
        dpiScale = mmPerInch/settings.dpi;  
    } else if(settings.unit == "in"){
        dpiScale = 1/(settings.dpi);  
    }
  }
  var scaleNoDPI=function(val){
    return format(val * settings.scale);
  }
  var scale=function(val) {
    return format(val * settings.scale * dpiScale);
  }
  var format=function(numb){
    //fix fractional digits
    numb = +numb.toFixed(settings.fractionalDigits); 
    // Note the plus sign that drops any "extra" zeroes at the end.
    // It changes the result (which is a string) into a number again (think "0 + foo"),
    // which means that it uses only as many digits as necessary.
    return numb;    
  }
  var paths = SVGReader.parse(svg, {}).allcolors;
  var gcode;
  var path;

  var idx = paths.length;
  while(idx--) {
    var subidx = paths[idx].length;
    var bounds = { x : Infinity , y : Infinity, x2 : -Infinity, y2: -Infinity, area : 0};

    // find lower and upper bounds
    while(subidx--) {
      if (paths[idx][subidx][0] < bounds.x) {
        bounds.x = paths[idx][subidx][0];
      }

      if (paths[idx][subidx][1] < bounds.y) {
        bounds.y = paths[idx][subidx][0];
      }

      if (paths[idx][subidx][0] > bounds.x2) {
        bounds.x2 = paths[idx][subidx][0];
      }
      if (paths[idx][subidx][1] > bounds.y2) {
        bounds.y2 = paths[idx][subidx][0];
      }
    }

    // calculate area
    bounds.area = (1 + bounds.x2 - bounds.x) * (1 + bounds.y2-bounds.y);
    paths[idx].bounds = bounds;
  }

  // cut the inside parts first
  paths.sort(function(a, b) {
    // sort by area
    return (a.bounds.area < b.bounds.area) ? -1 : 1;
  });
  var LaserON ='(BUF,SetBitBuf2)';
  var LaserOFF ='(BUF,ClearBitBuf2)';
  //G20 Inch units
  //G21 mm units
  gcode = [
    'G90', //Absolute Coordinates
  ];
  if(settings.unit == "mm"){
    gcode.push('G21');
  } else if(settings.unit == "in"){
    gcode.push('G22');
  } 
  gcode.push('G1 Z' + scaleNoDPI(settings.safeZ), 'M4');
  
  for (var pathIdx = 0, pathLength = paths.length; pathIdx < pathLength; pathIdx++) {
    path = paths[pathIdx];
    gcode.push(LaserOFF);
    // seek to index 0
    gcode.push([
      'G0',
      'X' + scale(path[0].x),
      'Y' + scale(path[0].y),
      'F' + settings.seekRate
    ].join(' '));

    for (var p = settings.passWidth; p<=settings.materialWidth; p+=settings.passWidth) {

      // begin the cut by dropping the tool to the work
      gcode.push([
        'G0',
        'Z' + scaleNoDPI(settings.cutZ + p),
        'F' + '200'
      ].join(' '));
      
      gcode.push(LaserON);
      // keep track of the current path being cut, as we may need to reverse it
      var localPath = [];
      for (var segmentIdx=0, segmentLength = path.length; segmentIdx<segmentLength; segmentIdx++) {
        var segment = path[segmentIdx];

        var localSegment = [
          'G1',
          'X' + scale(segment.x),
          'Y' + scale(segment.y),
          'F' + settings.feedRate
        ].join(' ');

        // feed through the material
        gcode.push(localSegment);
        localPath.push(localSegment);

        // if the path is not closed, reverse it, drop to the next cut depth and cut
        // this handles lines
        if (segmentIdx === segmentLength - 1 &&
            (segment.x !== path[0].x || segment.y !== path[0].y))
        {

          p+=settings.passWidth;
          if (p<settings.materialWidth) {
            // begin the cut by dropping the tool to the work
            gcode.push(['G0',
              'Z' + scaleNoDPI(settings.cutZ + p),
              'F' + '200'
            ].join(' '));

            Array.prototype.push.apply(gcode, localPath.reverse());
          }
        }
      }
    }
    gcode.push(LaserOFF);
    // go safe
    gcode.push(['G0',
      'Z' + scaleNoDPI(settings.safeZ),
      'F' + '300'
    ].join(' '));
  }

  // just wait there for a second
  gcode.push('G4 P1');

  // turn off the spindle
  gcode.push('M5');

  // go home
  gcode.push('G0 Z0 F300');
  gcode.push('G0 X0 Y0 F800');
  gcode.push('M2');
  return gcode;
}
