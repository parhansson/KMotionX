import {Injectable} from '@angular/core';
import {Http, Response} from '@angular/http';
import {Observable} from 'rxjs/Observable';
import {Observer} from 'rxjs/Observer';
import {IFileObject} from '../resources/resource.component'

@Injectable()
export class BackendService {
  constructor(private http: Http) { }


  load(file): Observable<Response> {
    return this.http.get(file, {
      url: file,
      search: 'nocache' + new Date().getTime()
    });

  }

  save(name, content: ArrayBuffer | ArrayBufferView | Blob | string) {
    let progressObserver: any;
    //progress: number = 0;
    let progress$ = new Observable<number>(observer => { progressObserver = observer })
    //TODO Safari does not support File constructor. 
    let url: string = "/upload", files: File[] = []
    files.push(new File([content], name))
     
    
    //return new Promise((resolve, reject) => {
      let formData: FormData = new FormData(),
        xhr: XMLHttpRequest = new XMLHttpRequest();

      for (let i = 0; i < files.length; i++) {
        formData.append("file"+i, files[i], files[i].name);
      }

      xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
          if (xhr.status === 200) {
            //resolve(JSON.parse(xhr.response));
            progressObserver.complete()
          } else {
            //reject(xhr.response);
            progressObserver.error(xhr.response);
          }
        }
      };

      //FileUploadService.setUploadUpdateInterval(500);

      xhr.upload.onprogress = (event) => {
        let progress = Math.round(event.loaded / event.total * 100);
        progressObserver.next(progress);
      };

      xhr.open('POST', url, true);
      xhr.send(formData);
   // });
    progress$.subscribe(data=>console.log(data +"%"))
    return progress$
  }
  public onOpenFile(path): Observable<IFileObject> {

    let loadobserver: Observer<IFileObject>;
    let observable = new Observable<IFileObject>(observer => loadobserver = observer)
    let url = "/api/kmx/" + 'openFile';
    let data = { "params": path };
    let oReq = new XMLHttpRequest();
    oReq.open("POST", url, true);
    oReq.responseType = "arraybuffer";

    oReq.onload = (oEvent) => {
      let arrayBuffer = oReq.response; // Note: not oReq.responseText
      if (arrayBuffer) {
        //application/pdf
        loadobserver.next({ contentType: oReq.getResponseHeader("Content-Type"), payload: arrayBuffer })
        loadobserver.complete()
      }
    };

    oReq.send(JSON.stringify(data));


    //var obs = this.http.post(url, JSON.stringify(data));
    return observable;
    //return this.onEvent('openFile', { "params": path });
  }
  public onListDir(path) {
    return this.onEvent('listDir', { "params": path });
  }
  public onLoadGlobalFile(type, file) {
    return this.onEvent('loadGlobalFile', { "params": [type, file] });
  }
  public jog(axis: number, speed: number) {
    return this.onEvent('jog', { "params": [axis, speed] });
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
  public onInvokeAction(action) {
    return this.onEvent('onInvokeAction', { "params": [action] });
  }
  public onDoErrorMessage(message) {
    return this.onEvent('onDoErrorMessage', { "params": message });
  }

  private onEvent(eventName: string, msg?: any) {
    var url = "/api/kmx/" + eventName;
    var data = msg || {};
    var obs = this.http.post(url, JSON.stringify(data));
    obs.subscribe(
      data => { },
      err => console.error('There was an error on event: ' + eventName, err),
      () => console.log(eventName + ' Complete')
    );
    return obs;
  }

}
