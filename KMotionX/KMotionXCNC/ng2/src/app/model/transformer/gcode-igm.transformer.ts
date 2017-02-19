
import { IGM, IgmObject, GCodeSource, BoundRect } from '../igm';
import { GCodeVector, GCodeCurve3 } from '../vector';
import { GCodeTransformer, State } from './gcode.transformer'
//Copyright (c) 2014 par.hansson@gmail.com


export class Gcode2IgmTransformer extends GCodeTransformer<IgmObject, IGM>{
 
  constructor(disableWorker?: boolean) { super(disableWorker) }
  protected createOutput() {
    return new IGM()
  }

  protected createShapeType() {
    const shape = new IgmObject();
    //shape.userData = { lineNo: this.state.lineNo }
    this.output.addToLayerObject('layer1',shape)
    return shape;
  }

  protected addLineShapePosition(newPosition: GCodeVector, shape: IgmObject) {
    shape.vectors.push(newPosition);
  }
  protected createCurveShape(curve: GCodeCurve3, shape: IgmObject) {
    const vectors = curve.getPoints(50);
    for (let point of vectors) {
      shape.vectors.push(point);
    }
  }

}
