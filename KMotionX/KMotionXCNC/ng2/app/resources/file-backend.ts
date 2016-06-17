import {OpaqueToken} from '@angular/core'
import {Observable} from 'rxjs/Rx'
import {FileResource} from './file-resource'

export const FileServiceToken = new OpaqueToken("fileservice");

export interface FileEntry {
  name: string,
  type: number
}

export interface DirList {
  dir: string;
  files: FileEntry[]
}

export interface IFileBackend {
  saveFile(name, content: ArrayBuffer | ArrayBufferView | Blob | string): Observable<number>;

  loadFile(path: string): Observable<FileResource>;

  listDir(path: string): Observable<DirList>;
}

// export abstract class FileBackend implements IFileBackend {

//   abstract saveFile(name, content: ArrayBuffer | ArrayBufferView | Blob | string): Observable<number>;

//   abstract loadFile(path: string): Observable<FileResource>;

//   abstract listDir(path: string): Observable<DirList>;

// }