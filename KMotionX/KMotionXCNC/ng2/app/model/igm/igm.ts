/// <reference path="../../../typings/main/ambient/three/index.d.ts" />

export class IgmObject {
  vectors: Vec2[] = []
  bounds: any
  node: any
  cmd: any

  constructor() {

  }
  first() {
    return this.vectors[0];
  }
  last() {
    return this.vectors[this.vectors.length - 1];
  }

  translate(translateVec: Vec2) {
    var subidx = this.vectors.length;
    while (subidx--) {
      var vec = this.vectors[subidx];
      vec.add(translateVec, false);
    }
  }
  scale(ratio: number) {
    var subidx = this.vectors.length;
    while (subidx--) {
      var vec = this.vectors[subidx];
      vec.scale(ratio, false);
    }
  }
  rotate(){ //TODO
    //transpose coordinates
    //vec.rotate(Math.PI/4);
    
  }
}
export interface LayerMap {
  [id: string]: IgmObject[];
}
// Intermediate Gcode Model
export class IGM {
  layers: LayerMap = {}; // sort by stroke color
  layerKeys: any[] = []; // layerKey is a mapping to add unnamed layers, layer will get a generated name
  textLayer = []; // textpaths
  unsupported = []  // Unsupported nodes
  constructor() { };


  public addToLayerObject(layerKey: any, obj: IgmObject, layerName?: string) {
    if (layerName === undefined) {
      layerName = this.layerKeys[layerKey];
      if (layerName === undefined) {
        var layerNumber = 0;//this.layerKeys.length;
        do {
          layerNumber++;
          layerName = 'layer' + layerNumber;

        } while (this.layers[layerName] !== undefined)
      }
    }
    //TODO check for renaming layers
    this.layerKeys[layerKey] = layerName;
    this.layers[layerName] = this.layers[layerName] || [];
    this.layers[layerName].push(obj);

  }
  get alllayers(): IgmObject[] {
    var all: IgmObject[] = [];

    for (var prop in this.layers) {
      // important check that this is objects own property 
      // not from prototype prop inherited
      if (this.layers.hasOwnProperty(prop)) {
        console.log('layerName', prop);
        var vectors = this.layers[prop];
        all = all.concat(vectors);
      }
    }
    return all;

  }

  public applyModifications(settings: any, ratio: number) {
    //cannot use this default value syntax for booleans
    settings.translateToOrigo = (typeof settings.translateToOrigo === 'boolean') ? settings.translateToOrigo : true;
    settings.removeOutline = (typeof settings.removeOutline === 'boolean') ? settings.removeOutline : false;
    settings.removeDuplicates = (typeof settings.removeDuplicates === 'boolean') ? settings.removeDuplicates : true;
    settings.removeSingularites = (typeof settings.removeSingularites === 'boolean') ? settings.removeSingularites : true;
    settings.fractionalDigits = settings.fractionalDigits || 3;
    var paths = this.alllayers;
    console.info("Nr of Shapes: ", paths.length);

    this.scaleVectors(paths, ratio);

    //Bounds are needed by removeDuplicates
    this.setBounds(paths);

    if (settings.removeSingularites) {
      var removed = this.removeSingularites(paths);
      console.info("Removed single points: ", removed);
    }

    // cut the inside parts first
    if (settings.removeDuplicates) {
      //This function will change the order of the paths
      var removed = this.removeDuplicates(paths);
      console.info("Removed duplicates: ", removed);
    }


    this.orderNearestNeighbour(paths);
    var joined = this.joinAdjacent(paths, settings.fractionalDigits);
    console.info("Joined adjacents: ", joined);
    this.setBounds(paths);

    var maxBounds = this.getMaxBounds(paths);

    if (settings.removeOutline) {
      //Some files has an outline. remove it if requested
      this.removeOutline(paths, maxBounds);
    }
    
    if (settings.translateToOrigo) {
      var translateVec = new Vec2(-maxBounds.x, -maxBounds.y);
      this.translateVectors(paths, translateVec);
    } 
    
    console.info("Nr of Shapes after: ", paths.length);

    return paths;

  }

  translateVectors(paths: IgmObject[], translateVec: Vec2) {
    var idx = paths.length;
    while (idx--) {
      var obj = paths[idx].translate(translateVec);
    }
  }

  scaleVectors(paths: IgmObject[], ratio: number) {
    var idx = paths.length;
    while (idx--) {
      paths[idx].scale(ratio)
    }
  }

  setBounds(paths: IgmObject[]) {
    var idx = paths.length;
    while (idx--) {
      var bounds = new BoundRect();
      var igmObj = paths[idx];
      var vectors = igmObj.vectors;
      if (vectors === undefined) {
        console.info("what", idx);
      }
      var subidx = vectors.length;
      while (subidx--) {
        var vec = vectors[subidx];
        bounds.include(vec);
      }
      igmObj.bounds = bounds;
    }
  }

  public getMaxBounds(paths: IgmObject[]) {
    var maxBounds = new BoundRect();
    var idx = paths.length;
    while (idx--) {
      var igmObj = paths[idx];
      var vectors = igmObj.vectors;
      maxBounds.include(igmObj.bounds.vec1());
      maxBounds.include(igmObj.bounds.vec2());
    }
    return maxBounds
  }

  removeDuplicates(paths: IgmObject[]) {
    var removed = 0;
    paths.sort(function(a, b) {
      // sort by area
      var aArea = a.bounds.area(); //TODO area needs to count zero with as 1
      var bArea = b.bounds.area();
      var result = aArea - bArea;
      if (result == 0) {
        var avec = a.vectors[0];
        var bvec = b.vectors[0];
        //TODO Experimental only, Need to compare whole path not just first point
        //and reverse path
        result = avec.x - bvec.x;
        if (result == 0) {
          result = avec.y - bvec.y;
        }
      }
      return result;
    });

    var idx = paths.length;
    while (idx-- > 1) {
      //TODO Experimental only, Need to compare whole path not just start and end point
      var o1 = paths[idx];
      var o2 = paths[idx - 1];
      if (o1.first().equal(o2.first()) && o1.last().equal(o2.last())) {
        removed++
        paths.splice(idx, 1);
      }
    }
    return removed
  }

  removeSingularites(paths: IgmObject[]) {
    var removed = 0;
    var idx = paths.length;
    while (idx--) {
      if (paths[idx].vectors.length == 1) {
        removed++
        paths.splice(idx, 1);
      };
    }
    return removed
  }
  removeOutline(paths: IgmObject[], maxBounds) {
    //TODO Find object with the same size as maxbounds.
    //currently this just asumes the largest object is first
    paths.pop();
  }


  joinAdjacent(paths: IgmObject[], fractionalDigits: number) {
    var joined = 0;
    if (paths.length < 2) {
      return;
    }
    var idx = 0;
    var last = paths[idx++];
    while (idx < paths.length) {
      var next = paths[idx];
      var lastEnd = last.last()
      var nextStart = next.first();
      //console.info(lastEnd, nextStart);
      if (this.pointEquals(lastEnd, nextStart, fractionalDigits)) {
        last.vectors.push.apply(last.vectors, next.vectors);
        paths.splice(idx, 1);
        joined++;
      } else {
        last = next;
        idx++
      }
    }
    return joined;
  }
  pointEquals(v1: Vec2, v2: Vec2, fractionalDigits: number) {
    return (
      v1.x.toFixed(fractionalDigits) === v2.x.toFixed(fractionalDigits) &&
      v1.y.toFixed(fractionalDigits) === v2.y.toFixed(fractionalDigits)
    );
  }

  orderNearestNeighbour(paths: IgmObject[]) {

    //These are the steps of the algorithm:
    //
    //  start on an arbitrary vertex as current vertex.
    //  find out the shortest edge connecting current vertex and an unvisited vertex V.
    //  set current vertex to V.
    //  mark V as visited.
    //  if all the vertices in domain are visited, then terminate.
    //  Go to step 2.
    var orderedPaths:IgmObject[] = [];
    var next = this.nearest(new Vec2(0, 0), paths);
    orderedPaths.push(next);
    while (paths.length > 0) {
      next = this.nearest(next.last(), paths);
      orderedPaths.push(next);
    }

    paths.push.apply(paths, orderedPaths);
    return;


  }
  private nearest(point: Vec2, paths: IgmObject[]) {

    var dist = Infinity;
    var index = -1;
    var checkReversePath = true;

    for (var pathIdx = 0, pathLength = paths.length; pathIdx < pathLength; pathIdx++) {
      var path = paths[pathIdx];
      var pathStartPoint = path.vectors[0];
      var distanceSquared;
      var startDS = pathStartPoint.distanceSquared(point);
      if (checkReversePath) {
        //check endpoint as well and reverse path if endpoint is closer
        var pathEndPoint = path.last()
        var endDS = pathEndPoint.distanceSquared(point);
        if (startDS < endDS) {
          distanceSquared = startDS;
        } else {
          distanceSquared = endDS;
          path.vectors.reverse();
        }

      } else {
        distanceSquared = startDS;
      }

      if (distanceSquared < dist) {
        dist = distanceSquared;
        index = pathIdx;
      }

    }
    return paths.splice(index, 1)[0];
  }

}



var Unit = {
  inch: "inch",
  mm: "mm"
}


export class GCodeSource {
  lines: string[];
  text: string;
  constructor(gcode: string[] | string) {
    if (Array.isArray(gcode)) {
      this.lines = gcode;
      this.text = gcode.join('\n');;
    } else {
      this.text = gcode;
      this.lines = gcode.split('\n');;
    }

  }

}

export class GCodeVector {
  X = 0
  Y = 0
  Z = 0
  A = 0
  B = 0
  C = 0
  constructor() {
  }
}

export class GCodeCurve3 {
  aX: number
  aY: number
  xRadius: number
  yRadius: number
  aStartAngle: number
  aEndAngle: number
  aClockwise: boolean
  height: number
  sZ: number
  constructor(startPoint: GCodeVector, endPoint: GCodeVector, args, clockWise) {


    var centerX = startPoint.X + args.I;
    var centerY = startPoint.Y + args.J;
    var centerZ = startPoint.Z + args.K; // Helical not correct implemented yet, i guess...
    var startAngle;
    var endAngle;
    var radius = Math.sqrt((args.I * args.I) + (args.J * args.J));  //should use pythagoras
    if (startPoint.X == endPoint.X && startPoint.Y == endPoint.Y) {
      //full circle
      startAngle = 0;
      endAngle = Math.PI * 2;
    } else {
      startAngle = -Math.atan2(endPoint.Y - centerY, endPoint.X - centerX);
      endAngle = Math.atan2(args.J, args.I);
    }

    //console.info("Curve ax, ay, radius, startangle, endangle",aX, aY, radius,aStartAngle,  aEndAngle);

    this.init(
      centerX, centerY,                  // ax, aY
      radius, radius,           // xRadius, yRadius
      startAngle, endAngle,  // aStartAngle, aEndAngle
      clockWise                     // aClockwise
    );
    this.sZ = startPoint.Z;
    this.height = endPoint.Z - startPoint.Z;

  }

  private init(aX, aY, xRadius, yRadius, aStartAngle, aEndAngle, aClockwise) {
    //Kept this function since it is the same as the THREE.EllipseCurve
    this.aX = aX;
    this.aY = aY;

    this.xRadius = xRadius;
    this.yRadius = yRadius;

    this.aStartAngle = aStartAngle;
    this.aEndAngle = aEndAngle;

    this.aClockwise = aClockwise;

  };

  getPoint(t, h) {

    var deltaAngle = this.aEndAngle - this.aStartAngle;

    if (deltaAngle < 0) deltaAngle += Math.PI * 2;
    if (deltaAngle > Math.PI * 2) deltaAngle -= Math.PI * 2;

    var angle;

    if (this.aClockwise === true) {

      angle = this.aEndAngle + (1 - t) * (Math.PI * 2 - deltaAngle);

    } else {

      angle = this.aStartAngle + t * deltaAngle;

    }

    var vector = new THREE.Vector3();

    vector.x = this.aX + this.xRadius * Math.cos(angle);
    vector.y = this.aY + this.yRadius * Math.sin(angle);
    vector.z = this.sZ + h;
    return vector;

  };



  getPoints(divisions) {

    if (!divisions) divisions = 5;

    var d, pts: THREE.Vector3[] = [];

    var h = 0;
    var hdelta = this.height / (divisions + 1);

    for (d = 0; d <= divisions; d++) {
      pts.push(this.getPoint(d / divisions, h));
      h += hdelta;
    }
    //console.info("ARC height sz ez delta",this.height, this.sZ, h-hdelta, hdelta);
    return pts;

  };

}

export class BoundRect {
  x = Infinity;
  y = Infinity;
  x2 = -Infinity;
  y2 = -Infinity

  constructor() { };

  scale(ratio) {
    this.x = this.x * ratio;
    this.y = this.y * ratio;
    this.x2 = this.x2 * ratio;
    this.y2 = this.y2 * ratio;
    return this;
  }

  vec1() {
    return new Vec2(this.x, this.y);
  }
  vec2() {
    return new Vec2(this.x2, this.y2);
  }

  include(vec) {
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

  area() {
    return this.height() * this.width();
  }

  height() {
    var height = this.y2 - this.y;
    return height;
  }

  width() {
    var width = this.x2 - this.x;
    return width;
  }

}




// ### Common / Math / Vec2

// A 2D column vector
export class Vec2 {
  x: number = 0;
  y: number = 0;

  constructor(x, y) {
    if (x) {
      this.x = x;
    }

    if (y) {
      this.y = y;
    }
  };


  // set x and y
  // TODO: mark as dirty for other calculations
  set(x, y) {
    this.x = x;
    this.y = y;
    return this;
  }

  // reset x and y to zero
  // TODO: mark as dirty for other calcs
  zero() {
    this.x = 0;
    this.y = 0;
    return this;
  }

  // negate the values of this vector and return a new Vec2
  negate(returnNew: boolean) {
    if (returnNew) {
      return new Vec2(-this.x, -this.y);
    } else {
      this.x = -this.x;
      this.y = -this.y;
      return this;
    }
  }

  // Add the incoming `vec2` vector to this vector
  // TODO: mark dirty for other calcs
  add(vec2: Vec2, returnNew: boolean) {
    if (!returnNew) {
      this.x += vec2.x;
      this.y += vec2.y;
      return this;
    } else {
      // Return a new vector if `returnNew` is truthy
      return new Vec2(
        this.x + vec2.x,
        this.y + vec2.y
      );
    }
  }

  // Subtract the incoming `vec2` from this vector
  // TODO: mark dirty for other calcs
  subtract(vec2: Vec2, returnNew: boolean) {
    if (!returnNew) {
      this.x -= vec2.x;
      this.y -= vec2.y;
      return this;
    } else {
      // Return a new vector if `returnNew` is truthy
      return new Vec2(
        this.x - vec2.x,
        this.y - vec2.y
      );
    }
  }

  // Multiply this vector by the incoming `vec2`
  // TODO: mark dirty for other calcs
  multiply(vec2: Vec2, returnNew: boolean) {
    var x, y;
    if (vec2.x !== undefined) {
      x = vec2.x;
      y = vec2.y;

      // Handle incoming scalars
    } else {
      x = y = vec2;
    }

    if (!returnNew) {
      this.x *= x;
      this.y *= y;
      return this;
    } else {
      return new Vec2(
        this.x * x,
        this.y * y
      );
    }
  }

  // Rotate this vector. Accepts a `Rotation` or angle in radians.
  //
  // Passing a truthy `inverse` will cause the rotation to
  // be reversed.
  //
  // If `returnNew` is truthy, a new
  // `Vec2` will be created with the values resulting from
  // the rotation. Otherwise the rotation will be applied
  // to this vector directly, and this vector will be returned.
  rotate(r: number, inverse: boolean, returnNew: boolean) {
    var
      x = this.x,
      y = this.y,
      rx: number, cos: number, sin: number, ry: number;

    var inverseFactor = (inverse) ? -1 : 1;

    //if (r.s !== undefined) {
    //  sin = r.s;
    //  cos = r.c;
    //} else {
    sin = Math.sin(r);
    cos = Math.cos(r)
    //}

    var
      x = this.x,
      y = this.y,
      rx = cos * x - (inverseFactor * sin) * y,
      ry = (inverseFactor * sin) * x + cos * y;

    if (returnNew) {
      return new Vec2(rx, ry);
    } else {
      this.set(rx, ry);
      return this;
    }
  }

  // Calculate the length of this vector (the norm)
  // TODO: used cached value if available
  length() {
    var x = this.x, y = this.y;
    return Math.sqrt(x * x + y * y);
  }

  // Get the length squared. For performance, use this instead of `Vec2#length` (if possible).
  // TODO: use cached value if available
  lengthSquared() {
    var x = this.x, y = this.y;
    return x * x + y * y;
  }

  // Return the distance betwen this `Vec2` and the incoming vec2 vector
  // and return a scalar
  distance(vec2: Vec2) {
    // TODO: prime candidate for optimizations
    return this.subtract(vec2, true).length();
  }

  distanceSquared(vec2: Vec2) {
    var c = this.subtract(vec2, true);
    return c.lengthSquared();
  }

  // Convert this vector into a unit vector.
  // Returns the length.
  normalize() {
    var length = this.length();

    // Don't bother normalizing a vector with a length ~0
    if (length < Number.MIN_VALUE) {
      return 0;
    }

    // Collect a ratio to shrink the x and y coords
    var invertedLength = 1 / length;

    // Convert the coords to be greater than zero
    // but smaller than or equal to 1.0
    this.x *= invertedLength;
    this.y *= invertedLength;

    return length;
  }

  // Determine if another `Vec2`'s components match this ones
  equal(v: Vec2) {
    if (v !== undefined) {
      return (
        this.x === v.x &&
        this.y === v.y
      );
    } else {
      return false;
    }
  }
  equalPoints(x: number, y: number) {
    return (
      this.x === x &&
      this.y === y
    )
  }

  // Return a new `Vec2` that contains the absolute value of
  // each of this vector's parts
  abs() {
    return new Vec2(
      Math.abs(this.x),
      Math.abs(this.y)
    );
  }

  // Return a new `Vec2` consisting of the smallest values
  // from this vector and the incoming
  //
  // When returnNew is truthy, a new `Vec2` will be returned
  // otherwise the minimum values in either this or `v` will
  // be applied to this vector.
  min(v: Vec2, returnNew: boolean) {
    var
      tx = this.x,
      ty = this.y,
      vx = v.x,
      vy = v.y,
      x = tx < vx ? tx : vx,
      y = ty < vy ? ty : vy;

    if (returnNew) {
      return new Vec2(x, y);
    } else {
      this.x = x;
      this.y = y;
      return this;
    }
  }

  // Return a new `Vec2` consisting of the largest values
  // from this vector and the incoming
  //
  // When returnNew is truthy, a new `Vec2` will be returned
  // otherwise the minimum values in either this or `v` will
  // be applied to this vector.
  max(v: Vec2, returnNew: boolean) {
    var
      tx = this.x,
      ty = this.y,
      vx = v.x,
      vy = v.y,
      x = tx > vx ? tx : vx,
      y = ty > vy ? ty : vy;

    if (returnNew) {
      return new Vec2(x, y);
    } else {
      this.x = x;
      this.y = y;
      return this;
    }
  }

  // Clamp values into a range.
  // If this vector's values are lower than the `low`'s
  // values, then raise them.  If they are higher than
  // `high`'s then lower them.
  //
  // Passing returnNew as true will cause a new Vec2 to be
  // returned.  Otherwise, this vector's values will be clamped
  clamp(low, high, returnNew: boolean) {
    var ret = this.min(high, true).max(low, true)
    if (returnNew) {
      return ret;
    } else {
      this.x = ret.x;
      this.y = ret.y;
      return this;
    }
  }

  // Ensure this vector contains finite values
  isValid() {
    return isFinite(this.x) && isFinite(this.y);
  }

  // Get the skew vector such that dot(skew_vec, other) == cross(vec, other)
  skew() {
    // Returns a new vector.
    return new Vec2(-this.y, this.x);
  }

  scale(scale: number, returnNew?: boolean) {
    if (returnNew) {
      return new Vec2(this.x * scale, this.y * scale);
    } else {
      this.x = this.x * scale;
      this.y = this.y * scale;
      return this;
    }

  }

  transpose(returnNew: boolean) {
    if (returnNew) {
      return new Vec2(this.y, this.x);
    } else {
      var tx = this.x;
      var ty = this.y;
      this.x = ty;
      this.y = tx;
      return this;
    }
  }

  matrixApply(mat: number[]) {
    this.x = mat[0] * this.x + mat[2] * this.y + mat[4]
    this.y = mat[1] * this.x + mat[3] * this.y + mat[5]
    return this;
  }


}