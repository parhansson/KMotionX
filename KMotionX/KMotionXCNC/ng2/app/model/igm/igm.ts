/// <reference path="../../../typings/main/ambient/three/index.d.ts" />
import {GCodeVector} from '../vector';



export class IgmPath {

}

export class IgmObject {
  cmd: string
  type: string
  vectors: GCodeVector[] = []
  args: string[] = []
  bounds: BoundRect
  node: any

  constructor() {

  }
  start() {
    return this.vectors[0];
  }
  end() {
    return this.vectors[this.vectors.length - 1];
  }

  translate(translateVec: GCodeVector) {
    var subidx = this.vectors.length;
    while (subidx--) {
      var vec = this.vectors[subidx];
      vec.add(translateVec);
    }
  }
  scale(ratio: number) {
    var subidx = this.vectors.length;
    while (subidx--) {
      var vec = this.vectors[subidx];
      vec.scale(ratio);
    }
  }
  rotate() { //TODO
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
  rawLine:string[] = []
  constructor() {

  }

  public addRaw(raw:string){
    this.rawLine.push(raw)
  }
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
      var translateVec = new GCodeVector(-maxBounds.x, -maxBounds.y, 0);
      this.translateVectors(paths, translateVec);
    }

    console.info("Nr of Shapes after: ", paths.length);

    return paths;

  }

  translateVectors(paths: IgmObject[], translateVec: GCodeVector) {
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
    paths.sort(function (a, b) {
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
      if (o1.start().equals(o2.start()) && o1.end().equals(o2.end())) {
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
      var lastEnd = last.end()
      var nextStart = next.start();
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
  pointEquals(v1: GCodeVector, v2: GCodeVector, fractionalDigits: number) {
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
    var orderedPaths: IgmObject[] = [];
    var next = this.nearest(new GCodeVector(0, 0, 0), paths);
    orderedPaths.push(next);
    while (paths.length > 0) {
      next = this.nearest(next.end(), paths)
      orderedPaths.push(next)
    }

    paths.push.apply(paths, orderedPaths)
    return;


  }
  private nearest(point: GCodeVector, paths: IgmObject[]) {

    var dist = Infinity;
    var index = -1;
    var checkReversePath = true;

    for (var pathIdx = 0, pathLength = paths.length; pathIdx < pathLength; pathIdx++) {
      var path = paths[pathIdx]
      var pathStartPoint = path.vectors[0]
      var distanceSquared
      var startDS = pathStartPoint.distanceSquared(point)
      if (checkReversePath) {
        //check endpoint as well and reverse path if endpoint is closer
        var pathEndPoint = path.end()
        var endDS = pathEndPoint.distanceSquared(point)
        if (startDS < endDS) {
          distanceSquared = startDS
        } else {
          distanceSquared = endDS
          path.vectors.reverse()
        }

      } else {
        distanceSquared = startDS
      }

      if (distanceSquared < dist) {
        dist = distanceSquared
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
    return new GCodeVector(this.x, this.y, 0);
  }
  vec2() {
    return new GCodeVector(this.x2, this.y2, 0);
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




