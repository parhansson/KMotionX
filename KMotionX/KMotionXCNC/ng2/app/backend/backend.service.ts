import {Injectable} from 'angular2/core';
import {Http, Response} from 'angular2/http';
import {Observable} from 'rxjs/Observable';
import {Observer} from 'rxjs/Observer';

@Injectable()
export class BackendService {
  constructor(private http: Http) { }


  load(file): Observable<Response> {
    return this.http.get(file, {
      url: file,
      search: 'nocache' + new Date().getTime()
    });

  }

  save(name, content) {
    //       //http://uncorkedstudios.com/blog/multipartformdata-file-upload-with-angularjs
    //       var fd = new FormData();
    //       var blob = new Blob([content], { type: 'plain/text', endings: 'transparent' });
    //       fd.append('file', blob, name);
    // 
    //       this.http.post("/upload", fd, {
    //           transformRequest: angular.identity,
    //           headers: {'Content-Type': undefined}
    //       })
    //       .success(function(response){
    //         //alert(response);
    //       })
    //       .error(function(){
    //         alert("Error saving file " + name);
    //       });
  }
  public onOpenFile(path): Observable<any> {

    let loadobserver: Observer<any>;
    let observable = new Observable(observer => loadobserver = observer)
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
