import { IOPin } from './io-pin'
export class Connector {
  constructor(public name: string, public pins: IOPin[]) { }
}