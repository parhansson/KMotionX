
import { GCodeVector, GCodeCurve3 } from '../vector';
import { GCodeTransformer, State } from './gcode.transformer'
//Copyright (c) 2014 par.hansson@gmail.com
class ExtendedGCodeVector extends GCodeVector {
  e = 0
  extruding = false
}
class ThreeShapeData {
  material: THREE.LineBasicMaterial
}
export class Gcode2ThreeTransformer extends GCodeTransformer<THREE.Geometry, THREE.Group>{
  // Create the final Object3d to add to the scene
  // interpolateColor = new THREE.Color(0x080808);
  // positionColor = new THREE.Color(0xAAAAFF);
  // lineMaterial = new THREE.LineBasicMaterial({
  //   opacity: 0.6,
  //   transparent: true,
  //   linewidth: 1,
  //   vertexColors: THREE.FaceColors
  // });
  private interpolateShapeData: ThreeShapeData = {

    material: new THREE.LineBasicMaterial({
      opacity: 0.6,
      transparent: true,
      linewidth: 1,
      color: 0x080808
    })
  }
  private moveShapeData: ThreeShapeData = {
    material: new THREE.LineBasicMaterial({
      opacity: 0.6,
      transparent: true,
      linewidth: 1,
      color: 0xAAAAFF
    })

  }


  constructor(disableWorker?: boolean) { super(disableWorker) }
  protected createOutput() {
    const output = new THREE.Group()
    output.name = 'GCODE';
    return output
  }

  protected createShapeType() {
    const data = this.getShapeData()
    const lineGeometry = new THREE.Geometry();
    const shape = new THREE.Line(lineGeometry, data.material);
    shape.userData = { lineNo: this.state.lineNo }
    this.output.add(shape);
    //console.log("new line");
    return lineGeometry;
  }

  private getShapeData() {
    switch (this.state.moveGroup.code) {
      case ('G0'):
        return this.moveShapeData
      case ('G1'):
        return this.interpolateShapeData
      case ('G2'):
        return this.interpolateShapeData
      case ('G3'):
        return this.interpolateShapeData
    }
  }

  protected addLineShapePosition(newPosition: GCodeVector, geometry: THREE.Geometry) {
    geometry.vertices.push(new THREE.Vector3(newPosition.x, newPosition.y, newPosition.z));
    //let color = getShapeData()
    //geometry.colors.push(color);
    return newPosition;
  }
  protected createCurveShape(curve: GCodeCurve3, geometry: THREE.Geometry) {
    var vectors = curve.getPoints(50);
    for (let point of vectors) {
      geometry.vertices.push(new THREE.Vector3(point.x, point.y, point.z));
    }
    //let color = getShapeData()
    // new THREE.Vector3().fromAttribute
    // for (var i = 0; i < vectors.length; i++) {
    //   geometry.colors.push(color);
    // }
  }

  //use for 3dprinter files
  //Need a way to rename axis to use E axis as extrude parameter
  private createLinePrinter(args, position: ExtendedGCodeVector, newPosition: ExtendedGCodeVector, geometry: THREE.Geometry) {

    newPosition.e = args.E !== undefined ? args.E : position.e,
      //TODO doesn't work as expected due to changing feedrate in the middle of line.
      newPosition.extruding = (newPosition.e - position.e) > 0;
    if (newPosition.extruding) {
      var color = new THREE.Color(newPosition.extruding ? 0xBBFFFF : 0xFF00FF);
      geometry.vertices.push(new THREE.Vector3(position.x, position.y, position.z));
      geometry.vertices.push(new THREE.Vector3(newPosition.x, newPosition.y, newPosition.z));
      geometry.colors.push(color);
      geometry.colors.push(color);
    }
    return newPosition;
  }
}
