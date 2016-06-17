import {Directive, Component, Input, Output, EventEmitter, ElementRef} from '@angular/core';
import {FileResource} from './file-resource'
@Component({
  selector: 'file-path',
  template: `
      <div>
        <span *ngFor="let part of resource.paths; let index = index" (click)="openIndex(index)" >{{part}}/</span><span>{{resource.file}}</span>
      </div>
    `

})
export class FilePathComponent {
  @Output() changed = new EventEmitter()
  @Input() resource: FileResource
  constructor() {

  }
  openIndex(index: number) {
    this.resource.goto(index + 1);
    this.changed.emit(this.resource);
  }
}