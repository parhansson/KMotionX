import {Component,Inject,Injectable} from 'angular2/core';
import {LogComponent} from "./log.component"

interface StringToLogMap {
    [id: string]: LogComponent;
}
  
@Injectable()
export class LogService{ 
  
  private consoles: StringToLogMap = { };
  
  constructor(){
    
  }
  public registerConsole(console:LogComponent){
    this.consoles[console.consoleId] = console;
  }
  
  public clearLog(consoleId:string){
    this.consoles[consoleId].clearLog(); 
  }
  public logExist(id:string){
    return this.consoles[id] !== undefined; 
  }
  public logFragment(id:string, fragment:DocumentFragment){
    var con = this.consoles[id];
    if(con === undefined){
      console.log("Console %s is undefined",id);
    } else {
      con.logFragment(fragment);
    }
  }
  
  public log(id:string, message:string){
    var con = this.consoles[id];
    if(con === undefined){
      console.log("Console %s: %s",id, message);
    } else {
      this.logFragment(id,this.createLogPost(message,con.color));
    }
  }
  
  private createLogPost(text:string, color:string):DocumentFragment {
    var fragment=document.createDocumentFragment();
    var div = document.createElement('div');
    var sp = document.createElement('span');
    sp.style.color = color;
    sp.appendChild(document.createTextNode(text));
    div.appendChild(sp);
    fragment.appendChild(div);
    return fragment;

  }
  

}