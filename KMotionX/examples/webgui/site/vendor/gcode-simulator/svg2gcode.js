function svg2gcode(svg, settings) {
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
  var transformVec = new Vec2(0,0);
  
  var scaleNoDPI=function(val){
    return format(val * settings.scale);
  }
  var ratio=settings.scale * dpiScale;
  
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
  
  function Rect(){
    this.x = Infinity;
    this.y = Infinity;
    this.x2 = -Infinity;
    this.y2 = -Infinity
    
    this.area = area;
    this.height = height;
    this.width = width;
    this.include = include;
    this.describe = describe;
    this.scale = scale;
    
    function scale(ratio){
      this.x = this.x*ratio;
      this.y = this.y*ratio;
      this.x2 = this.x2*ratio;
      this.y2 = this.y2*ratio;
      return this;      
    }
    
    function include(vec){
      var x = vec.x;
      var y = vec.y;
      
      if (x < this.x) {
        this.x = x;
      }

      if (y < this.y) {
        this.y = y;
      }

      if (x > this.x2) {
        this.x2 = x;
      }
      if (y > this.y2) {
        this.y2 = y;
      }
    }
    
    function area(){
      return this.height() * this.width();
    }
    
    function height(){
      var height = this.y2-this.y;
      return height;
    }
    
    function width(){
      var width = this.x2-this.x;      
      return width;
    }
        
  }
  
  var idx = paths.length;
  var maxBounds = new Rect();
  while(idx--) {
    var subidx = paths[idx].length;
    var bounds = new Rect();

    // find lower and upper bounds
    while(subidx--) {
      //transpose coordinates
      var vec = paths[idx][subidx];
      //transpose(vec);

      bounds.include(vec);
      maxBounds.include(vec);
    }
    bounds.scale(ratio);

    paths[idx].bounds = bounds;
  }
  maxBounds.scale(ratio);
  
  transformVec.x = -maxBounds.x;
  transformVec.y = -maxBounds.y;
  
  function transpose(vec){
    var tx = vec.x;
    var ty = vec.y;
    vec.x = ty;
    vec.y = tx;
  }
  
  function describe(rect){
    return 'Width: ' + format(rect.width()) + ' Height: ' + format(rect.height()) +' Area: '+ format(rect.area());
  }

  if(true){
    // cut the inside parts first
    paths.sort(function(a, b) {
      // sort by area    
      return (a.bounds.area() < b.bounds.area()) ? -1 : 1;
    });
  }
  if(true){
    paths.sort(function(a, b) {      
      return (a.bounds.y < b.bounds.y) ? -1 : 1;
    });    
  }
  
  if(true){
    paths.sort(function(a, b) {
      var aDist = a[0].lengthSquared();
      var bDist = b[0].lengthSquared();
      
      return aDist - bDist;
    });
    
  }
  
  var LaserON = 'M3 (laser on)';
  var LaserOFF = 'M5 (laser off)';
  //var LaserON = '(BUF,SetBitBuf14)';
  //var LaserOFF = '(BUF,ClearBitBuf14)';
  //G20 Inch units
  //G21 mm units
  gcode = [];
  gcode.push('('+describe(maxBounds)+')');
  
  gcode.push('G90'); //Absolute Coordinates
  if(settings.unit == "mm"){
    gcode.push('G21');
  } else if(settings.unit == "in"){
    gcode.push('G22');
  }
  gcode.push('M100 P400 Q100'); 
  //gcode.push('G1 Z' + scaleNoDPI(settings.safeZ), 'M4');
  gcode.push('G0 Z' + scaleNoDPI(settings.safeZ));
  gcode.push('F' + settings.seekRate);
  for (var pathIdx = 0, pathLength = paths.length; pathIdx < pathLength; pathIdx++) {
    path = paths[pathIdx];
    //add scale and transform
    var startVec = path[0].scale(ratio,true).add(transformVec,true);
    
    gcode.push(LaserOFF);
    //gcode.push('F' + settings.seekRate);
    // seek to index 0
    gcode.push([
      'G0',
      'X' + format(startVec.x),
      'Y' + format(startVec.y)/*,
      'F' + settings.seekRate*/
    ].join(' '));

    for (var p = settings.passWidth; p<=settings.materialWidth; p+=settings.passWidth) {

      // begin the cut by dropping the tool to the work
      gcode.push([
        'G0',
        'Z' + scaleNoDPI(settings.cutZ + p)
      ].join(' '));
      
      gcode.push('('+describe(path.bounds)+')');
      
      gcode.push(LaserON);
      //gcode.push('F' + settings.feedRate);
      // keep track of the current path being cut, as we may need to reverse it
      var localPath = [];
      for (var segmentIdx=0, segmentLength = path.length; segmentIdx<segmentLength; segmentIdx++) {
        //add transform
        var segment = path[segmentIdx].scale(ratio,true).add(transformVec,true);
        
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
        if (segmentIdx === segmentLength - 1 &&
            (segment.x !== startVec.x || segment.y !== startVec.y))
        {

          p+=settings.passWidth;
          if (p<settings.materialWidth) {
            // begin the cut by dropping the tool to the work
            gcode.push(['G0',
              'Z' + scaleNoDPI(settings.cutZ + p)
            ].join(' '));

            Array.prototype.push.apply(gcode, localPath.reverse());
          }
        }
      }
    }
    gcode.push(LaserOFF);
    // go safe
    gcode.push(['G0',
      'Z' + scaleNoDPI(settings.safeZ)
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
