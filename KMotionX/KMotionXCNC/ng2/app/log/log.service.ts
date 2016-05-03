import {Component, Inject, Injectable} from 'angular2/core';
import {LogComponent} from "./log.component"

interface StringToConsoleMap {
  [id: string]: LogComponent;
}
interface StringToLogMap {
  [id: string]: DocumentFragment[];
}

@Injectable()
export class LogService {

  private consoles: StringToConsoleMap = {};
  private logs: StringToLogMap = {};

  constructor() {

  }
  public registerConsole(console: LogComponent) {
    this.consoles[console.consoleId] = console;
    if (this.logs[console.consoleId] !== undefined) {
      console.logFragment(this.logs[console.consoleId])
      this.logs[console.consoleId] = []

    }
  }

  public clearLog(consoleId: string) {
    this.consoles[consoleId].clearLog();
  }
  public logExist(id: string) {
    return this.consoles[id] !== undefined;
  }
  public logFragment(id: string, fragment: DocumentFragment) {
    var con = this.consoles[id];
    if (con === undefined) {
      if (this.logs[id] === undefined) {
        this.logs[id] = []
      }
      this.logs[id].push(fragment)
      //console.log("Console %s is undefined",id);
    } else {
      con.logFragment(fragment);
    }
  }

  public log(id: string, message: string) {
    var con = this.consoles[id];
    let color = con === undefined ? 'black' : con.color
    this.logFragment(id, this.createLogPost(message, color));
  }

  private createLogPost(text: string, color: string): DocumentFragment {
    var fragment = document.createDocumentFragment();
    var div = document.createElement('div');
    var sp = document.createElement('span');
    sp.style.color = color;
    sp.appendChild(document.createTextNode(text));
    div.appendChild(sp);
    fragment.appendChild(div);
    return fragment;

  }


}