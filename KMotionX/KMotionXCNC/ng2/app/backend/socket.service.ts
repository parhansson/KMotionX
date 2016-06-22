import { Injectable } from '@angular/core';
import { Subject ,BehaviorSubject } from 'rxjs/Rx';
import { LogService, LogLevel } from "../log"
import { KMXUtil } from '../util/kmxutil';
import { KmxStatus, Message, StatusMessage, LogMessage, TextMessage, JsonMessage } from './shared'
import { FileResource } from '../resources'
//import {LogLevel} from '../kmx';
//import {LogLevel} from 'kmxlog/log.level';

@Injectable()
export class SocketService {
  gcodeFileSubject: Subject<FileResource> = new BehaviorSubject<FileResource>(new FileResource())
  simulateSubject: Subject<boolean> = new BehaviorSubject<boolean>(true)
  data: KmxStatus = new KmxStatus()
  private socketHandlers = {

    STATUS: this.statusHandler,// 0: Non blocking callback. Called from the interpreter in different thread
    COMPLETE: this.completeHandler,// 1: Non blocking callback. Called from the interpreter in different thread
    ERR_MSG: this.errorMessageHandler,// 2: Non blocking callback
    CONSOLE: this.consoleHandler,// 3: Non blocking callback, event though it has return value??
    USER: this.userHandler,// 4: Blocking callback. Called from the interpreter in different thread
    USER_M_CODE: this.userMCodeHandler,// 5: Blocking callback. Called from the interpreter in different thread
    MESSAGEBOX: this.messageHandler// 6: Blocking callback. However there is no need to block OK only boxes.
  }
  private socketWorker: Worker;

  constructor(private kmxLogger: LogService) {

    this.data.interpreting = false
    this.data.feedHold = false
    this.data.dro = [0, 0, 0]
    this.data.timeStamp = -1
    this.data.connected = false
    this.data.simulating = false
    this.data.currentLine = -1
    this.data.gcodeFile = ""
    this.data.machineSettingsFile = ""

    var url = 'ws://' + window.location.host + '/ws';
    //KMXUtil.getSingletonWorker("dist/app/backend/socket.loader.js", this.workerMessage.bind(this))
    KMXUtil.getSingletonWorker("dist/app/backend/WorkerBootstrap.js", this.workerMessage.bind(this))
      .then(
      (worker) => this.socketWorker = worker,
      (reason) => console.error(reason)
      );


    //does not seem to work, at least not in chrome
    //      window.onbeforeunload = function(){
    //        socketWorker.postMessage({command:'disconnect'})
    //      }
  }



  workerMessage(event: MessageEvent) {
    //messageCount++;
    //if(messageCount%5 == 0){
    //console.info("Messages received", messageCount);
    //}
    var message = event.data as Message<any>;
    if (message.isText) {
      if(message.message === 'WorkerReady')
      var url = 'ws://' + window.location.host + '/ws';
      this.socketWorker.postMessage({ command: 'connect', url: url });
      return;
    } else if (message.isJson) {
      var obj = message.message;
      if (obj.KMX) {
        return;
      }
      var handler = this.socketHandlers[obj.payload.type].bind(this);// || _this.defaultHandler;
      var ret = handler(obj.payload);
      if (obj.payload.block === true) {
        //only ack messages that require users answer here
        this.acknowledge(obj.id, ret);
      }
    } else if (message.isStatus) {
      var raw = (message as StatusMessage).message

      //  console.log("simulating", this.data.simulating)
      // if(this.data.simulating !== raw.simulating){
      //   this.simulateSubject.next(this.data.simulating)
      // }
      if (this.data.simulating !== raw.simulating) {
        console.log(raw.simulating);
      }
      if (this.data.gcodeFile !== raw.gcodeFile) {
        this.data.gcodeFile = raw.gcodeFile;
	      let gcodeResource = new FileResource();
        gcodeResource.canonical = this.data.gcodeFile;
        this.gcodeFileSubject.next(gcodeResource)
      }

      this.data.copyFrom(raw)
    } else if (message.isLog) {
      let logMessage = message as LogMessage
      this.logHandler(message.message, logMessage.type);
    }
  }

  public acknowledge(id, ret) {
    this.socketWorker.postMessage({ command: 'acknowledge', id: id, ret: ret });
  }

  statusHandler(payload) {
    this.kmxLogger.log('status', 'Line: ' + payload.line + " - " + payload.message);
  }
  completeHandler(payload) {
    this.kmxLogger.log('status', 'Done Line: ' + payload.line + " Status: " + payload.status + " Sequence " + payload.sequence + " - " + payload.message);
  }
  errorMessageHandler(payload) {
    this.kmxLogger.log('error', payload.message);
  }
  consoleHandler(payload) {
    this.kmxLogger.log('console', payload.message);
  }

  userHandler(payload) {
    if (confirm('USR: ' + payload.message + ' ?')) {
      return 0; // zero is true for this callback
    } else {
      return 1;
    }
  }
  userMCodeHandler(payload) {
    if (confirm('Are you sure you want to continue after M' + payload.code + ' ?')) {
      return 0; // zero is true for this callback
    } else {
      return 1;
    }
  }

  messageHandler(payload) {
    alert(payload.message);
    return 0;
  }

  logHandler(message, type: LogLevel) {
    if (this.kmxLogger.logExist('output') || true) {
      var style = '';
      var fragment = document.createDocumentFragment();
      var div = document.createElement('div');
      fragment.appendChild(div);
      if (type == LogLevel.NONE) {

      } else {
        var sp = document.createElement('span');
        if (type == LogLevel.SEND) {
          sp.style.color = 'green';
          sp.appendChild(document.createTextNode("SENT: "));
        } else if (type == LogLevel.RECEIVE) {
          sp.style.color = 'blue';
          sp.appendChild(document.createTextNode("RECEIVED: "));
        } else if (type == LogLevel.ERROR) {
          sp.style.color = 'red';
          sp.appendChild(document.createTextNode("ERROR: "));
        } else if (type == LogLevel.PING) {
          sp.style.color = 'blue';
          sp.appendChild(document.createTextNode("PING..."));
        }
        div.appendChild(sp);
      }
      div.appendChild(document.createTextNode(message));

      this.kmxLogger.logFragment('output', fragment);
    } else {
      if (type == LogLevel.NONE) {
        console.info('---', message);
      } else if (type == LogLevel.SEND) {
        console.info('SENT', message);
      } else if (type == LogLevel.RECEIVE) {
        console.info('RECEIVED', message);
      } else if (type == LogLevel.ERROR) {
        console.error('ERROR', message);
      } else if (type == LogLevel.PING) {
        console.info('PING...', message);
      }

    }
  }
}