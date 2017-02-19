import { Observer } from 'rxjs/Rx';
import { ModelSettingsService, ModelSettings } from '../model.settings.service';
import { ModelTransformer } from './model.transformer';
import { IGM, IgmObject } from '../igm';
import { GCodeVector } from '../vector';
//import { DxfParser } from 'dxf-parser'

import { KMXUtil } from '../../util'
export class Dxf2IgmTransformer extends ModelTransformer<ArrayBuffer | string, IGM> {
  constructor() {
    super()

  }
  execute(source: ArrayBuffer | string, targetObserver: Observer<IGM>) {
    let fileText;
    if (source instanceof ArrayBuffer) {
      fileText = KMXUtil.ab2str(source);
    } else {
      fileText = source
    }
    let model = new IGM();
    let parser = new DxfParser();
    try {
      let dxf = parser.parseSync(fileText);
      for (let entity of dxf.entities) {
        if (entity.type === 'LWPOLYLINE' || entity.type === 'LINE' || entity.type === 'POLYLINE') {
          model.addToLayerObject(entity.layer, this.scale(this.doLine(entity)))
        } else if (entity.type === 'CIRCLE' || entity.type === 'ARC') {
          model.addToLayerObject(entity.layer, this.scale(this.doArc(entity)))
        }
      }
      console.log(dxf);
      targetObserver.next(model)
    } catch (err) {
      return console.error(err.stack);
    }
  }
  scale(shape: IgmObject): IgmObject {
    //Read $MEASUREMENT Sets drawing units: 0 = English; 1 = Metric
    //shape.scale(?)
    return shape;
  }
  doArc(entity): IgmObject {
    let geometry: THREE.CircleGeometry, circle: THREE.Line;

    geometry = new THREE.CircleGeometry(entity.radius, 32, entity.startAngle, entity.angleLength);
    geometry.vertices.shift();
    let object = new IgmObject();
    for (let v of geometry.vertices) {
      object.vectors.push(new GCodeVector(v.x, v.y, v.z))
    }
    object.translate(new GCodeVector(entity.center.x, entity.center.y, entity.center.z))
    return object;
  }
  doLine(entity): IgmObject {
    let object = new IgmObject();
    for (let v of entity.vertices) {
      object.vectors.push(new GCodeVector(v.x, v.y, v.z))
    }
    //TODO
    /*
    for (let i = 0; i < entity.vertices.length; i++) {
      let v = entity.vertices[i]

      if (v.bulge) {
        let bulge = v.bulge;
        let startPoint = v;
        let endPoint = i + 1 < entity.vertices.length ? entity.vertices[i + 1] : object.vectors[0];

        //let bulgeGeometry = new THREE.BulgeGeometry(startPoint, endPoint, bulge);

        //object.vectors.push.apply(object.vectors, bulgeGeometry.vertices);
        object.vectors.push(new GCodeVector(v.x, v.y, v.z))
        object.vectors.push(new GCodeVector(endPoint.x, endPoint.y, endPoint.z))
      } else {
        object.vectors.push(new GCodeVector(v.x, v.y, v.z))
      }
  }
*/
    //Close shapes
    if (entity.shape) {
      let startPoint = object.vectors[0]
      object.vectors.push(new GCodeVector(startPoint.x, startPoint.y, startPoint.z));
    }
    return object
  }
}