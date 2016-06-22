import { Injectable, Inject, SkipSelf} from '@angular/core';
import { Observable, Subject } from 'rxjs/Rx'
import { FileResource, Payload, IFileBackend, FileServiceToken } from '../resources'
import { SocketService } from '../backend/socket.service';
import { FileStore } from '../editor'
import { StaticTransformer } from '../model/transformers'

@Injectable()
export class TransformingFileStore implements FileStore {
  payloadSubject = new Subject<Payload>()
  textSubject = new Subject<string>();

  constructor( @Inject(FileServiceToken) private fileBackend: IFileBackend,
    private staticTransformer: StaticTransformer,
    private socketService: SocketService) {
    this.payloadSubject.subscribe(
      payload => {
        this.staticTransformer.transform(payload.contentType, payload.arrayBuffer())
      });
    this.staticTransformer.gcodeSubject.subscribe(
      gcode => {
        this.textSubject.next(gcode.text)
      });

    this.socketService.gcodeFileSubject.subscribe(gcodeFile => {
      this.load(gcodeFile);
    })

  }

  store(name: string, content: ArrayBuffer | ArrayBufferView | Blob | string) {
    this.fileBackend.saveFile(name, content)
  }

  load(file: FileResource) {
    //this.editorComponent.resource.canonical = file.canonical

    if (file.payload === null) {
      if (file.file) {
        var subscription = this.fileBackend.loadFile(file.canonical).subscribe(
          file => {
            this.payloadSubject.next(file.payload)
            //this.backendService.setGCodeFile
          },
          null,
          () => subscription.unsubscribe()
        );
      }
    } else {
      this.payloadSubject.next(file.payload)
    }

  }
}