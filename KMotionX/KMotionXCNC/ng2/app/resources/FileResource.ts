
import {KMXUtil} from '../util/KMXUtil'

export class Payload {
  value: string | ArrayBuffer = null
  contentType: string = null

  constructor(value?: string | ArrayBuffer, contentType?: string) {
    if (value) {
      this.value = value
    }
    if (contentType) {
      this.contentType = contentType
    }
  }
  /**
   * Not yet implemented
   */
  blob(): any { }
  /**
   * Attempts to return body as parsed `JSON` object, or raises an exception.
   */
  json(): any {
    return JSON.parse(this.text())
  }
  /**
   * Returns the body as a string, presuming `toString()` can be called on the response body.
   */
  text(): string {
    if (this.value instanceof String) {
      return this.value as string
    } else {
      return KMXUtil.ab2str(this.value as ArrayBuffer)
    }
  }
  /**
   * Not yet implemented
   */
  arrayBuffer(): ArrayBuffer {
    if (this.value instanceof ArrayBuffer) {
      return this.value as ArrayBuffer
    } else {
      return KMXUtil.str2ab(this.value as string)
    }
  }

}

export class FileResource {
  paths: string[]
  //TODO payload should be a complex type that 
  payload: Payload = null


  constructor(dir?: string, public file?: string) {
    if (dir) {
      this.dir = dir
    } else {
      this.dir = ''
    }

    if (!file) {
      this.file = ''
    }
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