export class ControlMessagePayload {
  readonly block: boolean
  readonly type: 'STATUS' | 'COMPLETE' | 'ERR_MSG' | 'CONSOLE' | 'USER' | 'USER_M_CODE' | 'MESSAGEBOX'
  readonly line: number
  readonly status: number
  readonly sequence: number
  readonly code: number
  readonly message: string

}
export class ControlMessage {
  readonly id: number
  readonly payload: ControlMessagePayload

  constructor(message: any) {
    this.id = message.id
    this.payload = message.payload
  }
}