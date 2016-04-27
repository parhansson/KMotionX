
import {Injectable} from 'angular2/core';
import {KMXUtil} from '../util/kmxutil';


export abstract class ModelTransformer {
  inputMime: string[]
  outputMime: string
  name: string  
  abstract execute(source: any): Promise<any>
}

@Injectable()
export class TransformerService {
  transformers: ModelTransformer[] = [];

  constructor(private transformerSettings) {

  }
  register(transformer:ModelTransformer) {
    this.transformers.push(transformer);
  }

  matchType(mimeType:string) {
    for (var i = 0; i < this.transformers.length; i++) {
      if (this.transformers[i].inputMime.indexOf(mimeType) > -1) {
        return this.transformers[i];
      }
    }
    return null;
  }
  matchName(name) {
    for (var i = 0; i < this.transformers.length; i++) {
      if (this.transformers[i].name === name) {
        return this.transformers[i];
      }
    }
    return null;
  }
  transformNamed(transformerName, source) {

    var transformer = this.matchName(transformerName);
    if (transformer !== null) {
      return transformer.execute(source);
    }
    return Promise.reject('Named transformer "' + transformerName + '" not found');
  }
  transcode(mime:string, source:any): Promise<any> {

    var transformer = this.matchType(mime);
    if (transformer !== null) {
      var resultPromise = transformer.execute(source);
      if (transformer.outputMime !== "application/x-gcode") {
        return resultPromise.then(
          (result) => {
            return this.transcode(transformer.outputMime, result);
          });
      }
      return resultPromise;
    }

    return new Promise<any>((resolve, reject) => {

      if (typeof source === 'string') {
        //gcode text do not transform
        resolve(source);
      } else if (typeof source === 'ArrayBuffer') {
        //gcode file do not transform
        resolve(KMXUtil.ab2str(source));
      } else {
        reject("Unsupported source: " + (typeof source));
      }
    })


  }

}




