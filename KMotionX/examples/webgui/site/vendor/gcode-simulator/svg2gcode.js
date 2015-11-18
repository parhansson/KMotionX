if (!Array.prototype.last){
    Array.prototype.last = function(){
        return this[this.length - 1];
    };
};
if (!Array.prototype.first){
  Array.prototype.first = function(){
      return this[0];
  };
};
function svg2gcode(svg, settings) {
  settings = settings || {};
  settings.passes = settings.passes || 1;
  settings.materialWidth = settings.materialWidth || 6;
  settings.passWidth = settings.materialWidth/settings.passes;
  settings.scale = settings.scale || 1;
  settings.cutZ = settings.cutZ || 0; // cut z
  settings.safeZ = settings.safeZ || 0;   // safe z
  settings.feedRate = settings.feedRate || 1400;
  settings.seekRate = settings.seekRate || 800;
  settings.bitWidth = settings.bitWidth || 1; // in mm
  settings.unit = settings.unit || "mm";
  settings.fractionalDigits = settings.fractionalDigits || 3; //Thousands will do 
  settings.translateToOrigo = settings.translateToOrigo || true;
  settings.removeOutline = settings.removeOutline || false;
  settings.removeDuplicates = settings.removeDuplicates || true;
  settings.removeSingularites = settings.removeSingularites || true;
  //settings.initCode
  //settings.dpi = settings.dpi || null;
  
  var dpiScale = 1;
  var mmPerInch = 25.4;
  
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
  
  scaleVectors(paths, ratio);
  
  //Bounds are needed by removeDuplicates
  setBounds(paths);
  
  if(settings.removeSingularites){
    removeSingularites(paths);
  }
  
  // cut the inside parts first
  if(settings.removeDuplicates){
    //This function will change the order of the paths
    removeDuplicates(paths);
  }

  
  orderNearestNeighbour(paths);
  joinAdjacent(paths,settings.fractionalDigits);
  setBounds(paths);
  
  var maxBounds = getMaxBounds(paths);
  
  if(settings.removeOutline){
    //Some files has an outline. remove it if requested
    removeOutline(paths, maxBounds);
  }
  
  var translateVec = settings.translateToOrigo ? new Vec2(-maxBounds.x,-maxBounds.y) : new Vec2(0,0);
  
  console.info("Nr of Shapes after: ", paths.length);
  var LaserON = 'M3 (laser on)';
  var LaserOFF = 'M5 (laser off)';
  //var LaserON = '(BUF,SetBitBuf14)';
  //var LaserOFF = '(BUF,ClearBitBuf14)';
  //G20 Inch units
  //G21 mm units
  var gcode = new GCodeOutput(true);
  
  function GCodeOutput(filter){
      this.code = [];
      this.filter = filter;
      this.push = push;
      
      function push(gc){
        var line
        //TODO handle other types as well
        if(Array.isArray(gc)){
          line = gc.join(' ');
        } else {
          line = gc;
        }
        if(this.filter){
          if(this.code[this.code.length-1] !== line){
            this.code.push(line);
          }
        } else {
          this.code.push(line);
        }
      }
  }
  
  function describe(rect){
    return 'Width: ' + format(rect.width()) + ' Height: ' + format(rect.height()) +' Area: '+ format(rect.area());
  }
  
  gcode.push('('+describe(maxBounds)+')');
  
  gcode.push('G90'); //Absolute Coordinates
  if(settings.unit == "mm"){
    gcode.push('G21');
  } else if(settings.unit == "in"){
    gcode.push('G22');
  }
  if(settings.initCode){
    gcode.push(settings.initCode); 
    
  }
  //gcode.push('G1 Z' + scaleNoDPI(settings.safeZ), 'M4');
  gcode.push('F' + settings.feedRate);
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
    ]);

    for (var p = settings.passWidth; p<=settings.materialWidth; p+=settings.passWidth) {
      gcode.push('(Forward pass depth '+p+')');
      // begin the cut by dropping the tool to the work
      gcode.push([
        'G0',
        'Z' + scaleNoDPI(settings.cutZ + p)
      ]);
      
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
              gcode.push('(Reverse pass depth '+p+')');
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
  return gcode.code;
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
  this.vec1 = vec1;
  this.vec2 = vec2;
  
  function scale(ratio){
    this.x = this.x*ratio;
    this.y = this.y*ratio;
    this.x2 = this.x2*ratio;
    this.y2 = this.y2*ratio;
    return this;      
  }
  
  function vec1(){
    return new Vec2(this.x,this.y);
  }
  function vec2(){
    return new Vec2(this.x2,this.y2);
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

function scaleVectors(paths, ratio){
  var idx = paths.length;
  while(idx--) {
    var vectors = paths[idx]; 
    var subidx = vectors.length;
    while(subidx--) {
      var vec = vectors[subidx];
      //scale
      vec.scale(ratio);
      //transpose coordinates
      //vec.rotate(Math.PI/4);
    }
  }   
}

function setBounds(paths){
  var idx = paths.length;
  while(idx--) {
    var bounds = new BoundRect();
    var vectors = paths[idx];
    if(vectors === undefined){
      console.info("what", idx);
    }
    var subidx = vectors.length;
    while(subidx--) {
      var vec = vectors[subidx];
      bounds.include(vec);
    }
    vectors.bounds = bounds;
  }   
}

function getMaxBounds(paths){
  var maxBounds = new BoundRect();
  var idx = paths.length;
  while(idx--) {
    var vectors = paths[idx]; 
    maxBounds.include(vectors.bounds.vec1());
    maxBounds.include(vectors.bounds.vec2());
  }
  return maxBounds
}

function removeDuplicates(paths){
  
  paths.sort(function(a, b) {
    // sort by area
    var aArea = a.bounds.area(); //TODO area needs to count zero with as 1
    var bArea = b.bounds.area();
    var result = aArea-bArea;
    if(result == 0){
      var avec = a[0];
      var bvec = b[0];
      //TODO Experimental only, Need to compare whole path not just first point
      result = avec.x - bvec.x;
      if(result == 0){
        result = avec.y - bvec.y;
      }
    }
    return result;
  });
  
  var idx = paths.length;
  while(idx-- > 1) {
    //TODO Experimental only, Need to compare whole path not just start and end point
    var p1 = paths[idx];
    var p2 = paths[idx-1];
    if(p1.first().equal(p2.first()) && p1.last().equal(p2.last())){
      paths.splice(idx,1);
    }
  }
}

function removeSingularites(paths){
  var idx = paths.length;
  while(idx--) {
    if(paths[idx].length == 1){
      paths.splice(idx,1);
    };
  }
}
function removeOutline(paths, maxBounds){
  //TODO Find object with the same size as maxbounds.
  //currently this just asumes the largest object is first
  paths.pop();      
}


function joinAdjacent(paths,fractionalDigits){
  if(paths.length < 2){
    return;
  }
  var idx = 0;
  var last = paths[idx++];
  while(idx < paths.length){
    var next = paths[idx];
    var lastEnd =last[last.length-1];
    var nextStart = next[0];
    //console.info(lastEnd, nextStart);
    if(pointEquals(lastEnd, nextStart,fractionalDigits)){
      last.push.apply(last, next);
      paths.splice(idx,1);
    } else {
      last = next;
      idx++
    }
  }
}
function pointEquals(v1,v2,fractionalDigits){
  return (
      v1.x.toFixed(fractionalDigits) === v2.x.toFixed(fractionalDigits) &&
      v1.y.toFixed(fractionalDigits) === v2.y.toFixed(fractionalDigits)
  );
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
    var checkReversePath = true;
    
    for (var pathIdx = 0, pathLength = paths.length; pathIdx < pathLength; pathIdx++) {
      var path = paths[pathIdx];
      var pathStartPoint = path[0];
      var distanceSquared;
      var startDS = pathStartPoint.distanceSquared(point);
      if(checkReversePath){
        //check endpoint as well and reverse path if endpoint is closer
        var pathEndPoint = path[path.length-1];
        var endDS = pathEndPoint.distanceSquared(point);
        if(startDS < endDS){
          distanceSquared = startDS;
        } else {
          distanceSquared = endDS;
          path.reverse();
        }
        
      } else {
        distanceSquared = startDS;
      }
      
      if(distanceSquared < dist){
        dist = distanceSquared;
        index = pathIdx;
      }
      
    }
    return paths.splice(index,1)[0];
  }
}
