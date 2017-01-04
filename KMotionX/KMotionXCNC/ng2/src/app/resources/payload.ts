
import { KMXUtil } from '../util/kmxutil'

export class Payload {
  value: string | ArrayBuffer = null
  contentType: string = null
  name: string = null

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