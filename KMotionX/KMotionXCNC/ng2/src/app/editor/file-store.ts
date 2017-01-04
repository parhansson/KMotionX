import { OpaqueToken } from '@angular/core';
import { Observable, Subject } from 'rxjs/Rx'
import { FileResource, Payload } from '../resources'

export const FileStoreToken = new OpaqueToken('editor-file-store');

export interface FileStore {
  textSubject: Subject<string>
  store(name: string, content: ArrayBuffer | ArrayBufferView | Blob | string);
  load(file: FileResource | Payload)
}