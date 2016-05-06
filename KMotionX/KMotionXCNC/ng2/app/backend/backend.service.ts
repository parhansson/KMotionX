import {Observable} from 'rxjs/Observable';
import {IFileObject} from './file'

export abstract class BackendService {

  public abstract save(name, content: ArrayBuffer | ArrayBufferView | Blob | string): Observable<number>
  
  public abstract onOpenFile(path): Observable<IFileObject>
  
  protected abstract onEvent(eventName: string, parameters?: any) :Observable<any>
    
  public onListDir(path:string) {
    return this.onEvent('listDir', path);
  }
  public onLoadGlobalFile(type:number, file:string) {
    return this.onEvent('loadGlobalFile', [type, file]);
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
  public onInvokeAction(action:number) {
    return this.onEvent('onInvokeAction', action);
  }
  public onDoErrorMessage(message:string) {
    return this.onEvent('onDoErrorMessage', message);
  }

}
