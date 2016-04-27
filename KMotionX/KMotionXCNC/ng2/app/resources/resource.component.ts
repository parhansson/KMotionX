import {Component, Input, Output, EventEmitter} from 'angular2/core';
import {BackendService} from '../backend/backend.service'



@Component({
  selector: 'file-dialog',
  template: `
  <div class="modal" [style.display]="modalDisplay">
      <div class="modal-dialog">
        <div class="modal-content">       
          <div class="modal-header">
            <a class="close" (click)="toggle()">&times;</a>
            <h4>Open GCode</h4>
          </div>
          <div class="modal-body">
            <ul class="modal-file-list">
              <li class="button" (click)="selectFile(file)" *ngFor="#file of files">{{file.name}}</li>          
            </ul>
            <p>Open file by selecting or with drag and drop to editor from desktop</p>
          </div>
          <div class="modal-footer">
            <a class="btn" (click)="toggle()">Cancel</a>      
          </div>
        </div>
      </div>
      </div>
    `

})
export class ResourceComponent {
  @Input('dir') currentDir: string
  currentFile: string
  @Output() selectedFile = new EventEmitter()
  files = []
  showModal: boolean = false
  modalDisplay: string = "none"

  constructor(private backendService: BackendService) {

  }


  selectFile(file: any) {
    if (file.type === 4) {
      this.currentDir = this.currentDir + "/" + file.name
      this.listDir()
    } else {
      this.currentFile = file.name
      this.openFile();
    }
  }

  toggle() {

    this.showModal = !this.showModal;
    if (this.showModal) {
      this.modalDisplay = "block"
      this.listDir()
    } else {
      this.modalDisplay = "none"
    }
  }
  private openFile() {
    this.backendService.onOpenFile(this.currentDir + "/" + this.currentFile).subscribe(
      data => this.selectedFile.emit(data),
      err => console.error(err),
      () => this.toggle()
    );
  }
  private listDir() {
    this.backendService.onListDir(this.currentDir).subscribe(
      data => {
        this.files = data.json()
      },
      err => console.log(err)
    );

  }
}    