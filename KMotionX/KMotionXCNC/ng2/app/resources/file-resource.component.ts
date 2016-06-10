import {Directive, Component, Input, Output, EventEmitter, ElementRef} from '@angular/core';
import {FileResource} from './file-resource'
import {FileBackend} from './file-backend'
import {DropZoneDirective} from './drop-zone.directive'

@Component({
  selector: 'file-path',
  template: `
      <div>
        <span *ngFor="let part of resource?.paths; let index = index" (click)="openIndex(index)" >{{part}}/</span><span>{{resource.file}}</span>
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

@Component({
  selector: 'file-dialog',
  directives: [FilePathComponent, DropZoneDirective],
  template: `
  <div class="modal" [style.display]="modalDisplay">
      <div class="modal-dialog">
        <div class="modal-content">       
          <div class="modal-header">
            <a class="close" (click)="hide()">&times;</a>
            <h4>Open file</h4>
          </div>
          <div class="modal-body" file-dropzone (dropped)="setFile($event)">
            <file-path [resource]="resource" (changed)="listDir()" ></file-path>
            <ul class="modal-file-list">
              <li class="button" (click)="selectFile(file)" *ngFor="let file of files">{{file.name}}</li>          
            </ul>
            <p>Open file by selecting or with drag and drop to editor from desktop</p>
          </div>
          <div class="modal-footer">
            <a class="btn" (click)="hide()">Cancel</a>      
          </div>
        </div>
      </div>
      </div>
    `

})
export class ResourceComponent {
  @Input() resource: FileResource
  @Input() loadOnSelect: boolean = false
  @Output() selectedFile = new EventEmitter<FileResource>()

  private files = []
  private showModal: boolean = false
  private modalDisplay: string = "none"

  constructor(private fileBackend: FileBackend) {

  }


  selectFile(file: any) {
    if (file.type === 4) {
      if (file.name === "..") {
        this.resource.up(1)
      } else if (file.name === ".") {
        //Do nothing, same directory
      } else {
        this.resource.append(file.name)
      }
      this.listDir()
    } else {
      this.resource.file = file.name
      this.openFile()
    }
  }

  show() {
    this.showModal = true;
    this.modalDisplay = "block"
    this.listDir()
  }
  hide() {
    this.showModal = false;
    this.modalDisplay = "none"
  }

  save(content: string) {
    //this should not be done here
    this.fileBackend.saveFile(this.resource.canonical, content)
  }

  saveAs(content: string) {
    //TODO implement
    console.warn("Save as not yet implemented")
  }
  private setFile(file: FileResource) {
    this.selectedFile.emit(file)
    this.hide()
  }
  openFile() {
    //TODO there should be an option to actually load file here or let subscriber do it
    //however if file is dropped on dialog, what the heck should we do then
    //store it in FileResource payload
    //make FileResource payload an object that can be of different types 
    //ArrayBuffer, File Blob String etc,  and store 
    if(this.loadOnSelect){
      this.fileBackend.loadFile(this.resource.canonical).subscribe(
        data => this.selectedFile.emit(data),
        err => console.error(err),
        () => this.hide()
      )
    } else {
      this.selectedFile.emit(this.resource)
      this.hide()
    }
  }
  private listDir() {
    this.fileBackend.listDir(this.resource.dir).subscribe(
      data => {
        this.files = data.files
        this.resource.dir = data.dir
      },
      err => console.log(err)
    )

  }
}   