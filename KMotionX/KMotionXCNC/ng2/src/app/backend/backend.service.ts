import { Observable } from 'rxjs/Rx';
import { FileResource, IFileBackend, Payload, DirList } from '../resources'

export abstract class BackendService implements IFileBackend {

  protected abstract onEvent(eventName: string, parameters?: any): Observable<any>;
  public abstract saveFile(name: string, content: ArrayBuffer | ArrayBufferView | Blob | string);
  public abstract loadFile(path: string): Observable<Payload>;
  public abstract listDir(path: string): Observable<DirList>;

  public setGCodeFile(file: string) {
    return this.onEvent('setGcodeFile', file);
  }

  public setMachineFile(file: string) {
    return this.onEvent('setMachineFile', file);
  }

  public jog(axis: number, speed: number) {
    return this.onEvent('jog', [axis, speed]);
  }

  public onFeedhold() {
    return this.onEvent('onFeedhold');
  }

  public onSimulate() {
    return this.onEvent('onSimulate');
  }

  public onHalt() {
    return this.onEvent('onHalt');
  }

  public onEmergencyStop() {
    return this.onEvent('onEmergencyStop');
  }

  public onCycleStart() {
    return this.onEvent('onCycleStart');
  }

  public onStep() {
    return this.onEvent('onStep');
  }

  public onReset() {
    return this.onEvent('onReset');
  }

  public onUpdateMotionParams() {
    return this.onEvent('onUpdateMotionParams');
  }

  public onInvokeAction(action: number) {
    return this.onEvent('onInvokeAction', action);
  }

  public onDoErrorMessage(message: string) {
    return this.onEvent('onDoErrorMessage', message);
  }

}
