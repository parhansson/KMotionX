import { Injectable } from '@angular/core';
import { Subject, BehaviorSubject } from 'rxjs/Rx';
import { LogService, LogLevel } from '../log'
import { SerializedObject } from '../util';
import { KmxStatus, ControlMessagePayload, ControlMessage } from '../hal/kflop';
import { LogMessage, TextMessage } from './socket/messages'

import { FileResource } from '../resources'

import * as SocketWorker from '../../socket.worker';
//import SocketWorker from '../../socket.worker';

@Injectable()
export class SocketService {
  gcodeFileSubject: Subject<FileResource> = new BehaviorSubject<FileResource>(new FileResource())
  simulateSubject: Subject<boolean> = new BehaviorSubject<boolean>(true)
  data: KmxStatus = new KmxStatus()

  private socketWorker: Worker;

  constructor(private kmxLogger: LogService) {

    this.data.dro = [0, 0, 0]
    this.data.timeStamp = -1
    this.data.simulating = false
    this.data.currentLine = -1
    
    //this.socketWorker = new SocketWorker()
    this.socketWorker = new (SocketWorker as any)();
    this.socketWorker.onmessage = this.onWorkerMessage.bind(this);
    //does not seem to work, at least not in chrome
    //      window.onbeforeunload = function(){
    //        socketWorker.postMessage({command:'disconnect'})
    //      }
  }
  //Message type guards
  private isText(payload: SerializedObject<any>): payload is SerializedObject<TextMessage> {
    return payload.key === 'TextMessage';
  }

  private isControl(payload: SerializedObject<any>): payload is SerializedObject<ControlMessage> {
    return payload.key === 'ControlMessage';
  }

  private isStatus(payload: SerializedObject<any>): payload is SerializedObject<KmxStatus> {
    return payload.key === 'KmxStatus';
  }

  private isLog(payload: SerializedObject<any>): payload is SerializedObject<LogMessage> {
    return payload.key === 'LogMessage';
  }

  private onWorkerMessage(event: MessageEvent) {
    let payload = event.data as SerializedObject<any>
    if (this.isText(payload)) {
      this.onTextMessage(payload.object)
    } else if (this.isControl(payload)) {
      this.onControlMessage(payload.object);
    } else if (this.isStatus(payload)) {
      this.onStatusMessage(payload.object)
    } else if (this.isLog(payload)) {
      this.onLogMessage(payload.object);
    }
  }

  private acknowledge(id, ret) {
    this.socketWorker.postMessage({ command: 'acknowledge', id: id, ret: ret });
  }

  private onTextMessage(textMessage: TextMessage) {
    if (textMessage.message === 'WorkerReady') {
      let url = 'ws://' + window.location.host + '/ws';
      this.socketWorker.postMessage({ command: 'connect', url: url });
    }
  }

  private onStatusMessage(raw: KmxStatus) {
    //  console.log('simulating', this.data.simulating)
    // if(this.data.simulating !== raw.simulating){
    //   this.simulateSubject.next(this.data.simulating)
    // }
    if (this.data.simulating !== raw.simulating) {
      console.log(raw.simulating);
    }
    if (this.data.gcodeFileTimestamp !== raw.gcodeFileTimestamp || this.data.gcodeFile !== raw.gcodeFile) {
      //timestamp in StatusMessage to detect file modifications
      this.data.gcodeFile = raw.gcodeFile;
      let gcodeResource = new FileResource();
      gcodeResource.canonical = this.data.gcodeFile;
      this.gcodeFileSubject.next(gcodeResource)
    }

    this.data.copyFrom(raw)
  }

  private onControlMessage(obj: ControlMessage) {
    let payload = obj.payload
    let ret = undefined;
    switch (payload.type) {
      case 'STATUS': // 0: Non blocking callback. Called from the interpreter in different thread
        {
          this.kmxLogger.log('status', 'Line: ' + payload.line + ' - ' + payload.message);
          return
        }
      case 'COMPLETE':// 1: Non blocking callback. Called from the interpreter in different thread
        {
          this.kmxLogger.log('status', 'Done Line: ' + payload.line + ' Status: ' + payload.status + ' Sequence ' + payload.sequence + ' - ' + payload.message);
          return
        }
      case 'ERR_MSG':// 2: Non blocking callback
        {
          this.kmxLogger.log('error', payload.message);
          return
        }
      case 'CONSOLE':// 3: Non blocking callback, event though it has return value??
        {
          this.kmxLogger.log('console', payload.message);
          return
        }
      case 'USER': // 4: Blocking callback. Called from the interpreter in different thread
        {
          if (confirm('USR: ' + payload.message + ' ?')) {
            ret = 0; // zero is true for this callback
          } else {
            ret = 1;
          }
          break
        }
      case 'USER_M_CODE': // 5: Blocking callback. Called from the interpreter in different thread
        {
          if (confirm('Are you sure you want to continue after M' + payload.code + ' ?')) {
            ret = 0; // zero is true for this callback
          } else {
            ret = 1;
          }
          break
        }
      case 'MESSAGEBOX': // 6: Blocking callback. However there is no need to block OK only boxes.
        {
          alert(payload.message);
          ret = 0
          break
        }
    }

    if (obj.payload.block === true) {
      //only ack messages that require users answer here
      this.acknowledge(obj.id, ret);
    }
  }


  private onLogMessage(logMessage: LogMessage) {
    let message = logMessage.message
    let type = logMessage.type

    let prefixClass = ''
    if (type == LogLevel.INFO.valueOf()) {
      prefixClass = 'info'
    } else if (type == LogLevel.SEND.valueOf()) {
      prefixClass = 'outgoing'
    } else if (type as LogLevel == LogLevel.RECEIVE.valueOf()) {
      prefixClass = 'incomming'
    } else if (type as LogLevel == LogLevel.ERROR.valueOf()) {
      prefixClass = 'error'
    }

    this.kmxLogger.log('output', message, prefixClass);

  }
}