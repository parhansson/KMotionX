import { Injectable, Inject } from '@angular/core';
import { Observable, Subject } from 'rxjs/Rx'
import { FileStoreToken, FileStore } from './file-store';
import {
  FileResource,
  IFileBackend,
  FileServiceToken } from '../resources';

@Injectable()
export class DefaultFileStore implements FileStore {
  textSubject = new Subject<string>();

  constructor( @Inject(FileServiceToken) private fileBackend: IFileBackend) {

  }

  store(name: string, content: ArrayBuffer | ArrayBufferView | Blob | string) {
    this.fileBackend.saveFile(name, content)
  }

  load(file: FileResource) {
    if (file.payload) {
      //Drop imported file
      this.textSubject.next(file.payload.text());
    } else {
      //Selected in file dialog
      //TODO do i need to unsubsbrive
      if(file.file){
        let subscription = this.fileBackend.loadFile(file.canonical).subscribe(
          file => {
            this.textSubject.next(file.payload.text());
          },
          null,
          () => subscription.unsubscribe()
        );
      }
    }
  }
}