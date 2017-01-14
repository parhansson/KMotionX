export class LimitBuffer<T> {

  private _events: T[] = [];
  private _bufferSize: number;

  constructor(bufferSize: number = Number.POSITIVE_INFINITY) {
    this._bufferSize = bufferSize < 1 ? 1 : bufferSize;
  }

  getEvents() {
    return this._trimBufferThenGetEvents();
  }

  setBufferSize(bufferSize: number) {
    this._bufferSize = bufferSize < 1 ? 1 : bufferSize;
    this._trimBufferThenGetEvents();
  }

  prune(): void {
    this._events.splice(0, this._events.length)
  }

  add(value: T): void {
    this._events.push(value);
    this._trimBufferThenGetEvents();
  }


  private _trimBufferThenGetEvents(): T[] {
    const _bufferSize = this._bufferSize;
    const _events = this._events;

    let eventsCount = _events.length;
    let spliceCount = 0;

    if (eventsCount > _bufferSize) {
      spliceCount = eventsCount - _bufferSize;
      _events.splice(0, spliceCount);
    }

    return _events;
  }
}
