import {Injectable} from 'angular2/core';
import {Http,Response} from 'angular2/http';
import { Observable } from 'rxjs/Observable';
//module kmx {
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

  public onListDir(path) {
    return this.onEvent('listDir', { "params": path });
  }
  public onLoadGlobalFile(type, file) {
    this.onEvent('loadGlobalFile', { "params": [type, file] });
  }
  public jog(axis: number, speed: number) {
    this.onEvent('jog', { "params": [axis, speed] });
  }
  public onFeedhold() {
    this.onEvent('onFeedhold');
  }
  public onSimulate() {
    this.onEvent('onSimulate');
  }
  public onHalt() {
    this.onEvent('onHalt');
  }
  public onEmergencyStop() {
    this.onEvent('onEmergencyStop');
  }
  public onCycleStart() {
    this.onEvent('onCycleStart');
  }
  public onStep() {
    this.onEvent('onStep');
  }
  public onReset() {
    this.onEvent('onReset');
  }
  public onUpdateMotionParams() {
    this.onEvent('onUpdateMotionParams');
  }
  public onInvokeAction(action) {
    this.onEvent('onInvokeAction', { "params": [action] });
  }
  public onDoErrorMessage(message) {
    this.onEvent('onDoErrorMessage', { "params": message });
  }

  private onEvent(eventName: string, msg?: any) {
    var url = "/api/kmx/" + eventName;
    var data = msg || {};
    var o = this.http.post(url, JSON.stringify(data))
      //return o.map(res => res.json())
      .subscribe(
      data => console.log(data),
      err => this.logError(err),
      () => console.log('Random Quote Complete')
      );
    console.log(url);
  }

  private logError(err) {
    console.error('There was an error: ', err);
  }

}
    
//}