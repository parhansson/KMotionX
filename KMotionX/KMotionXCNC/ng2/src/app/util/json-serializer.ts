import {SerializedObject} from './serialized-object'
//http://cloudmark.github.io/Json-Mapping/

export class JsonSerializer {

  serialize<T>(data: T): SerializedObject<T> {
    return {
      key: this.getKey(data),
      object: data
    }
  }

  getKey(object: any) {
    return object.constructor.name
  }
  // deserialize(object:SerializedObject):any{
  //   object.object.prototype = SerializedObject.constructor
  // }
}
