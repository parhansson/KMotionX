import { Directive, Component, Input, Output, EventEmitter, ElementRef, Inject } from '@angular/core';
import { FileResource } from './file-resource'
import { IFileBackend, FileServiceToken, FileEntry } from './file-backend'
import { DropZoneDirective } from './drop-zone.directive'
import { FilePathComponent } from './file-path.component'
import { Payload } from './payload'

@Component({
  selector: 'file-dialog',
  template: `
  <div class="modal" [style.display]="modalDisplay">
      <div class="modal-dialog">
        <div class="modal-content">       
          <div class="modal-header">
            <a class="close" (click)="hide()">&times;</a>
            <h4>Open file</h4>
          </div>
          <div class="modal-body" file-dropzone (dropped)="setFileResource($event)">
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
export class FileDialogComponent {
  @Input() resource: FileResource
  @Output() selectedFile = new EventEmitter<FileResource | Payload>()

  private files: FileEntry[] = []
  private showModal: boolean = false
  private modalDisplay: string = 'none'

  constructor( @Inject(FileServiceToken) private fileBackend: IFileBackend) {

  }


  selectFile(file: FileEntry) {
    if (file.type === 4) {
      if (file.name === '..') {
        this.resource.up(1)
      } else if (file.name === '.') {
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
    this.modalDisplay = 'block'
    this.listDir()
  }
  hide() {
    this.showModal = false;
    this.modalDisplay = 'none'
  }

  saveAs(content: string) {
    //TODO implement
    console.warn('Save as not yet implemented')
  }
  onPayload(payload: Payload) {
    //Dropped file
    console.log('Imported file')
    this.selectedFile.emit(payload)
  }
  protected setFileResource(file: FileResource) {
    this.selectedFile.emit(file)
    this.hide()
  }
  openFile() {
    this.setFileResource(this.resource);
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