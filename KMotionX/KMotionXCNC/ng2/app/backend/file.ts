export interface IFileObject {
  contentType: string
  payload: ArrayBuffer
}

export class FileResource {
  paths: string[]

  constructor(dir?: string, public file?: string) {
    this.dir = dir
  }
  set dir(dir: string) {
    this.paths = dir.split("/")
  }
  get dir() {
    return this.paths.join("/")
  }
  append(path: string) {
    this.paths.push(path)
  }

  get canonical() {
    return this.dir + "/" + this.file
  }
  set canonical(file) {
    let parts = file.split("/");
    this.file = parts.splice(parts.length - 1)[0]
    this.paths = parts
  }

  up(count: number) {
    this.paths = this.paths.splice(0, this.paths.length - count);
  }
  goto(index: number) {
    this.paths = this.paths.splice(0, index);
  }
}