import {Component, Inject,Input,ElementRef} from '@angular/core';
import {LogService} from './log.service'

@Component({
  selector:'kmx-log',
    template: `
    <button class="btn btn-primary btn_wide" (click)="clearLog()">{{title}} (Clear)</button>
    <div class="status_message status_small"></div>
  `
  //,providers:[LogService]
})

export class LogComponent{ 
  @Input()
  title:string;
  
  id:string;
  @Input()
  color:string;
    
  constructor(private logService:LogService,private elementRef: ElementRef){

  }
  
  @Input()  
  set consoleId(id:string){
    this.id = id;
    this.logService.registerConsole(this);
    //this.logService.log(this.consoleId,'Testa loggen ' + this.consoleId);
  }
  get consoleId(){
    return this.id;
  }
  
  clearLog(){
    console.log('Clear log' + this.consoleId);
       var element = this.elementRef.nativeElement.children[1];
       while (element.firstChild) {
        element.removeChild(element.firstChild);
       }
  }
  
  logFragment(fragment:DocumentFragment | DocumentFragment[]){
      //console.log(fragment);
      var node = this.elementRef.nativeElement.children[1];
      if(fragment instanceof Array){
        for(let f of fragment){
          node.appendChild(f);
        }
      } else {
        node.appendChild(fragment);
      }
      node.scrollTop = node.scrollHeight;
  }

}