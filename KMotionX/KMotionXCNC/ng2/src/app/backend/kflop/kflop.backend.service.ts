import { Injectable } from '@angular/core';
import { Http, Response } from '@angular/http';
import { Observable, Observer, Subject, AsyncSubject } from 'rxjs/Rx';
import { FileResource, Payload, IFileBackend, DirList } from '../../resources'
import { BackendService } from '../backend.service'


@Injectable()
export class KFlopBackendService extends BackendService implements IFileBackend {
  constructor(private http: Http) { super() }

  public listDir(path: string): Observable<DirList> {
    return this.onEvent('listDir', path);
  }

  saveFile(name: string, content: ArrayBuffer | ArrayBufferView | Blob | string) {
    let url: string = '/upload'
    let progressObserver: Observer<number>;
    //progress: number = 0;
    let progress$ = new Observable<number>(observer => { progressObserver = observer })
    let formData: FormData = new FormData()

    if (File.constructor === Function) {
      let files: File[] = []
      files.push(new File([content], name))
      for (let i = 0; i < files.length; i++) {
        formData.append('file' + i, files[i], files[i].name);
      }
    } else {
      //Some browsers (Safari) does not support File constructor. 
      var blob = new Blob([content], { type: 'plain/text', endings: 'transparent' });
      formData.append('file', blob, name);

    }

    //return new Promise((resolve, reject) => {

    let xhr: XMLHttpRequest = new XMLHttpRequest();
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
    progress$.subscribe(data => console.log(data + '%'))
    return progress$
  }

  loadFile(path: string): Observable<Payload> {

    let url = '/api/kmx/' + 'openFile';
    let data = { 'params': path };

    let builtin = false;
    if (builtin) {
      return this.http.post(url, JSON.stringify(data))
        .map((res: Response) => { 
          return new Payload(res.arrayBuffer(), res.headers.get('Content-Type'))
        })

    } else {

      let observable = new AsyncSubject<Payload>()
      let oReq = new XMLHttpRequest();
      oReq.open('POST', url, true);
      oReq.responseType = 'arraybuffer';

      oReq.onload = (oEvent) => {
        let arrayBuffer = oReq.response as ArrayBuffer; // Note: not oReq.responseText
        if (arrayBuffer) {
          observable.next(new Payload(arrayBuffer, oReq.getResponseHeader('Content-Type')))
          observable.complete()
        }
      };

      oReq.send(JSON.stringify(data));
      return observable;
    }


    //return this.onEvent('openFile', { 'params': path });
  }

  protected onEvent(eventName: string, parameters?: any) {
    let url = '/api/kmx/' + eventName;
    let payload: any
    if (parameters === undefined) {
      payload = JSON.stringify({});
    } else {
      payload = JSON.stringify({ params: parameters });
    }
    let obs = this.http.post(url, payload).map((res: Response) => res.json())
    obs.subscribe(
      data => { },
      err => console.error('There was an error on event: ' + eventName, err),
      () => console.log(eventName + ' Complete')
    );
    return obs;
  }

}
