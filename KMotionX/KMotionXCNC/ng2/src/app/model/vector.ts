
export interface MoveArcArguments {
  I?: number
  J?: number
  K?: number
  R?: number
}
export interface MoveAngularArguments {
  A?: number
  B?: number
  C?: number
}
export interface MoveArguments extends MoveAngularArguments, MoveArcArguments {
  X?: number
  Y?: number
  Z?: number


}

export class GCodeVector {

  constructor(public x?: number, public y?: number, public z?: number, public a?: number, public b?: number, public c?: number) {
    this.x = x || 0
    this.y = y || 0
    this.z = z || 0
    this.a = a || 0
    this.b = b || 0
    this.c = c || 0

  }

  scale(scale: number) {

    this.x = this.x * scale;
    this.y = this.y * scale;
    this.z = this.z * scale;
    return this;
  }

  add(v: GCodeVector) {
    this.x += v.x;
    this.y += v.y;
    this.z += v.z;

    return this;
  }

  equals(v: GCodeVector) {

    return ((v.x === this.x) && (v.y === this.y) && (v.z === this.z));

  }

  distanceSquared(v: GCodeVector) {

    var dx = this.x - v.x;
    var dy = this.y - v.y;
    var dz = this.z - v.z;

    return dx * dx + dy * dy + dz * dz;

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
  constructor(startPoint: GCodeVector, endPoint: GCodeVector, args: MoveArcArguments, clockWise) {


    var centerX = startPoint.x + (args.I || 0);
    var centerY = startPoint.y + (args.J || 0);
    // centerZ is only used in other planes
    var centerZ = startPoint.z + (args.K || 0); //TODO Helical not correct implemented yet, i guess...
    var startAngle;
    var endAngle;
    var radius = Math.sqrt((args.I * args.I) + (args.J * args.J));  //should use pythagoras
    if (startPoint.x == endPoint.x && startPoint.y == endPoint.y) {
      //full circle
      startAngle = 0;
      endAngle = Math.PI * 2;
    } else {
      startAngle = -Math.atan2(endPoint.y - centerY, endPoint.x - centerX);
      endAngle = Math.atan2(args.J, args.I);
    }

    //console.info("Curve ax, ay, radius, startangle, endangle",aX, aY, radius,aStartAngle,  aEndAngle);

    this.init(
      centerX, centerY,                  // ax, aY
      radius, radius,           // xRadius, yRadius
      startAngle, endAngle,  // aStartAngle, aEndAngle
      clockWise                     // aClockwise
    );
    this.sZ = startPoint.z;
    this.height = endPoint.z - startPoint.z;

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

  getPoint(t: number, h: number) {

    let deltaAngle = this.aEndAngle - this.aStartAngle;

    if (deltaAngle < 0) deltaAngle += Math.PI * 2;
    if (deltaAngle > Math.PI * 2) deltaAngle -= Math.PI * 2;

    let angle;

    if (this.aClockwise === true) {

      angle = this.aEndAngle + (1 - t) * (Math.PI * 2 - deltaAngle);

    } else {

      angle = this.aStartAngle + t * deltaAngle;

    }

    const vector = new GCodeVector();

    vector.x = this.aX + this.xRadius * Math.cos(angle);
    vector.y = this.aY + this.yRadius * Math.sin(angle);
    vector.z = this.sZ + h;
    return vector;

  };



  getPoints(divisions: number) {

    if (!divisions) divisions = 5;

    let d: number, pts: GCodeVector[] = [];

    let h = 0;
    let hdelta = this.height / (divisions + 1);

    for (d = 0; d <= divisions; d++) {
      pts.push(this.getPoint(d / divisions, h));
      h += hdelta;
    }
    //console.info("ARC height sz ez delta",this.height, this.sZ, h-hdelta, hdelta);
    return pts;

  };

}