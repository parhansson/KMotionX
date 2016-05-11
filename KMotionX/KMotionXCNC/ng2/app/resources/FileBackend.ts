import {Observable} from 'rxjs/Rx'
import {FileResource} from './FileResource' 

export interface IFileBackend {
  saveFile(name, content: ArrayBuffer | ArrayBufferView | Blob | string): Observable<number>;
  
  loadFile(path:string): Observable<FileResource>;
  
  listDir(path:string)
}

export abstract class FileBackend implements IFileBackend{
  
  abstract saveFile(name, content: ArrayBuffer | ArrayBufferView | Blob | string): Observable<number>;
  
  abstract loadFile(path:string): Observable<FileResource>;
  
  abstract listDir(path:string)
  
}