import {Injectable,Inject,SkipSelf} from '@angular/core';
import {FileResource, Payload, IFileBackend, FileServiceToken, DirList} from '../resources'
import {Observable} from 'rxjs/Rx'

@Injectable()
export class TransformingFileService implements IFileBackend {
  constructor(@SkipSelf() @Inject(FileServiceToken) private fileBackend: IFileBackend) {

  }

  saveFile(name, content: ArrayBuffer | ArrayBufferView | Blob | string): Observable<number> {
    return this.fileBackend.saveFile(name, content);
  }

  loadFile(path: string): Observable<FileResource> {
    console.log("LoadFIle");
    return this.fileBackend.loadFile(path);
  }

  listDir(path: string): Observable<DirList> {
    console.log("ListDir");
    return this.fileBackend.listDir(path);
  }
}