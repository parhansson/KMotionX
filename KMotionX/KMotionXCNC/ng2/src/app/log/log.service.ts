import { Component, Inject, Injectable } from '@angular/core';
import { LogComponent } from './log.component'
import { LogSubject } from './log-subject'

interface LogIdToLogSubjectMap {
  [id: string]: LogSubject<LogMessage>;
}
export class LogMessage {
  constructor(public message: string, public styleClass?: string) {
  }
}

@Injectable()
export class LogService {

  private logs: LogIdToLogSubjectMap = {}

  constructor() {

  }

  public getLogSubject(logId: string) {
    let subject = this.logs[logId];
    if (subject === undefined) {
      subject = this.logs[logId] = new LogSubject<LogMessage>();
    }
    return subject;
  }

  public clearLog(logId: string) {
    this.logs[logId].prune();
  }

  public logExist(logId: string) {
    return this.logs[logId] !== undefined;
  }

  public log(consoleId: string, message: string, styleClass?:string) {
    this.getLogSubject(consoleId).next(new LogMessage(message, styleClass));
  }
}