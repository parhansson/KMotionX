import { Injectable, Inject, SkipSelf } from '@angular/core';
import { Observable, Subject } from 'rxjs/Rx'
import { FileResource, Payload, IFileBackend, FileServiceToken } from '../resources'
import { BackendService } from '../backend/backend.service'
import { FileStore } from '../editor'
import { StaticTransformer } from '../model/transformers'

@Injectable()
export class TransformingFileStore implements FileStore {
  payloadSubject = new Subject<Payload>()
  textSubject = new Subject<string>();

  constructor( @Inject(FileServiceToken) private fileBackend: BackendService,
    private staticTransformer: StaticTransformer
  ) {
    this.payloadSubject.subscribe(
      payload => {
        this.staticTransformer.transform(payload.contentType || payload.name, payload.arrayBuffer())
      });
    this.staticTransformer.gcodeSubject.subscribe(
      gcode => {
        this.textSubject.next(gcode.text)
      });
  }

  store(name: string, content: ArrayBuffer | ArrayBufferView | Blob | string) {
    this.fileBackend.saveFile(name, content)
  }

  load(file: FileResource | Payload) {
    if (file instanceof Payload) {
      this.payloadSubject.next(file)
    } else {
      if (file.file) {
        this.fileBackend.setGCodeFile(file.canonical);
      }
    }

  }
}