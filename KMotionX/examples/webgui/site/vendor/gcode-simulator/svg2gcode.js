function svg2gcode(svg, settings) {
  settings = settings || {};
  settings.passes = settings.passes || 1;
  settings.materialWidth = settings.materialWidth || 6;
  settings.passWidth = settings.materialWidth/settings.passes;
  settings.scale = settings.scale || 1;
  settings.cutZ = settings.cutZ || 0; // cut z
  settings.safeZ = settings.safeZ || 0;   // safe z
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
  var translateVec = new Vec2(0,0);
  
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
  
  console.info("Nr of Shapes: ", paths.length);
  
  var idx = paths.length;
  var maxBounds = new BoundRect();
  while(idx--) {
    var subidx = paths[idx].length;
    var bounds = new BoundRect();

    // find lower and upper bounds
    while(subidx--) {
      var vec = paths[idx][subidx];
      
      //scale
      vec.scale(ratio);
      //transpose coordinates
      //vec.rotate(Math.PI/4);

      bounds.include(vec);
      maxBounds.include(vec);
    }

    paths[idx].bounds = bounds;
  }
  
  translateVec.x = -maxBounds.x;
  translateVec.y = -maxBounds.y;
  
  
  function describe(rect){
    return 'Width: ' + format(rect.width()) + ' Height: ' + format(rect.height()) +' Area: '+ format(rect.area());
  }

  if(false){
    // cut the inside parts first
    paths.sort(function(a, b) {
      // sort by area    
      return (a.bounds.area() < b.bounds.area()) ? -1 : 1;
    });
  }

  orderNearestNeighbour(paths);
  console.info("Nr of Shapes after: ", paths.length);
  var LaserON = 'M3 (laser on)';
  var LaserOFF = 'M5 (laser off)';
  //var LaserON = '(BUF,SetBitBuf14)';
  //var LaserOFF = '(BUF,ClearBitBuf14)';
  //G20 Inch units
  //G21 mm units
  var gcode = [];
  gcode.push('('+describe(maxBounds)+')');
  
  gcode.push('G90'); //Absolute Coordinates
  if(settings.unit == "mm"){
    gcode.push('G21');
  } else if(settings.unit == "in"){
    gcode.push('G22');
  }
  gcode.push('M100 P400 Q100'); 
  //gcode.push('G1 Z' + scaleNoDPI(settings.safeZ), 'M4');
  gcode.push('F' + settings.seekRate);
  gcode.push('G0 Z' + scaleNoDPI(settings.safeZ));
  for (var pathIdx = 0, pathLength = paths.length; pathIdx < pathLength; pathIdx++) {
    var path = paths[pathIdx];
    //add scale and transform
    var startVec = path[0].add(translateVec,true);
    
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
      gcode.push('(Forward pass '+p+')');
      // begin the cut by dropping the tool to the work
      gcode.push([
        'G0',
        'Z' + scaleNoDPI(settings.cutZ + p)
      ].join(' '));
      
      gcode.push('('+describe(path.bounds)+')');
      
      gcode.push(LaserON);

      // keep track of the current path being cut, as we may need to reverse it
      var localPath = [];
      for (var segmentIdx=0, segmentLength = path.length; segmentIdx<segmentLength; segmentIdx++) {
        //add transform
        var segment = path[segmentIdx].add(translateVec,true);
        
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
          
          if(segment.x !== startVec.x || segment.y !== startVec.y)
          {
            
            p+=settings.passWidth;
            if (p<=settings.materialWidth) {
              gcode.push('(Reverse pass '+p+')');
              // begin the cut by dropping the tool to the work
              gcode.push(['G0',
                          'Z' + scaleNoDPI(settings.cutZ + p)
                          ].join(' '));
              
              Array.prototype.push.apply(gcode, localPath.reverse());
            }
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

function BoundRect(){
  this.x = Infinity;
  this.y = Infinity;
  this.x2 = -Infinity;
  this.y2 = -Infinity
  
  this.area = area;
  this.height = height;
  this.width = width;
  this.include = include;
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

function orderNearestNeighbour(paths){
  
//These are the steps of the algorithm:
//
//  start on an arbitrary vertex as current vertex.
//  find out the shortest edge connecting current vertex and an unvisited vertex V.
//  set current vertex to V.
//  mark V as visited.
//  if all the vertices in domain are visited, then terminate.
//  Go to step 2.
  var orderedPaths = [];
  var next = nearest(new Vec2(0,0), paths);
  orderedPaths.push(next);
  while(paths.length > 0){
    next = nearest(next[next.length-1], paths);
    orderedPaths.push(next);      
  }
  
  paths.push.apply(paths, orderedPaths);
  return;
  
  function nearest(point, paths){
    
    var dist = Infinity;
    var index = -1;
    for (var pathIdx = 0, pathLength = paths.length; pathIdx < pathLength; pathIdx++) {
      var path = paths[pathIdx];
      //var pathEndPoint = path[path.length-1];
      var pathStartPoint = path[0];
      
      var distanceSquared = pathStartPoint.distanceSquared(point);
      if(distanceSquared < dist){
        dist = distanceSquared;
        index = pathIdx;
      }
      
    }
    return paths.splice(index,1)[0];
  }
}
