

import { Injectable } from '@angular/core';
import { Observer, Observable, Subject, AsyncSubject } from 'rxjs/Rx';
import { KMXUtil } from '../util/kmxutil';
import { ModelTransformer } from './transformer/model.transformer';


@Injectable()
export class TransformerService {
  transformers: ModelTransformer<any, any>[] = [];

  constructor() {

  }
  register(transformer: ModelTransformer<any, any>) {
    this.transformers.push(transformer);
  }

  matchType(mimeType: string) {
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

    let subject = new Subject<any>()
    var transformer = this.matchName(transformerName);
    if (transformer !== null) {
      return transformer.execute(source, subject);
    }
    subject.error('Named transformer "' + transformerName + '" not found')
    subject.complete()
  }
  transcode(mime: string, source: any): Subject<any> {
    let subject = new AsyncSubject<any>()
    var transformer = this.matchType(mime);
    if (transformer !== null) {
      transformer.execute(source, subject).subscribe(result => this.transcode(transformer.outputMime, result))
      /*
      var resultPromise = transformer.execute(source,subject);
      if (transformer.outputMime !== "application/x-gcode") {
        return resultPromise.then(
          (result) => {
            return this.transcode(transformer.outputMime, result);
          });
      }
      return resultPromise;
      */
    }

    if (typeof source === 'string') {
      //asume gcode text do not transform
      subject.next(source);
    } else if (typeof source === 'ArrayBuffer') {
      //gcode file do not transform
      subject.next(KMXUtil.ab2str(source));
    } else {
      subject.error("Unsupported source: " + (typeof source));
    }
    subject.complete()

    return subject

  }

}




