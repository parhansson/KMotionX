import {Directive, Component, Input, Output, EventEmitter, ElementRef} from '@angular/core';
import {BackendService} from '../backend/backend.service'
import {IFileObject, FileResource} from '../backend/file'



@Directive({
  selector: "[file-dropzone]",
  host: {
    '(dragover)': 'processDragOverOrEnter($event)',
    '(dragenter)': 'processDragOverOrEnter($event)',
    '(dragleave)': 'processDragExit($event)',
    '(drop)': 'processDrop($event)'
  }

})
export class FileDropZone {
  @Output() dropped = new EventEmitter<IFileObject>()

  /*
      var validMimeTypes = attrs.fileDropzone;
*/
  element: HTMLElement
  constructor(element: ElementRef) {
    this.element = element.nativeElement
  }

  processDrop(event:DragEvent) {
    var file:File
    if (event != null) {
      event.preventDefault();
    }
    this.highlight(null)
    file = (event.dataTransfer as DataTransfer).files.item(0);

    let reader = new FileReader();
    reader.addEventListener("load",(evt) => {
      if (this.checkSize(file.size) && this.isTypeValid(file.type)) {
        console.log(typeof ((evt as any).target.result))
        this.dropped.emit({ contentType: file.type, payload: (evt as any).target.result })
      }
    })

    reader.readAsArrayBuffer(file)
    return false;
  }
  
  processDragExit(event:DragEvent){
    if (event != null) {
      event.preventDefault();
    }
    this.highlight(null)
    event.dataTransfer.effectAllowed = 'none';
    return false
  }
  
  processDragOverOrEnter(event:DragEvent) {
    if (event != null) {
      event.preventDefault();
    }
    this.highlight('lightgray')
    event.dataTransfer.effectAllowed = 'copy';
    return false;
  }

  private highlight(color: string) {
    this.element.style.backgroundColor = color;
  }
  
  private checkSize(size) {
    /*
    var _ref;
    if (((_ref = attrs.maxFileSize) === (void 0) || _ref === '') || (size / 1024) / 1024 < attrs.maxFileSize) {
      return true;
    } else {
      alert("File must be smaller than " + attrs.maxFileSize + " MB");
      return false;
    }
    */
    return true;
  }

  private isTypeValid(type) {
    /*
    if ((validMimeTypes === (void 0) || validMimeTypes === '') || validMimeTypes.indexOf(type) > -1) {
      return true;
    } else {
      alert("Invalid file type.  File must be one of following types " + validMimeTypes);
      return false;
    }
    */
    return true;
  }


}



@Component({
  selector: 'file-path',
  template: `
      <div>
        <span *ngFor="let part of resource?.paths; let index = index" (click)="openIndex(index)" >{{part}}/</span>
        <!--<span>{{resource.file}}</span>-->
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
  directives: [FilePathComponent, FileDropZone],
  template: `
  <div class="modal" [style.display]="modalDisplay">
      <div class="modal-dialog">
        <div class="modal-content">       
          <div class="modal-header">
            <a class="close" (click)="close()">&times;</a>
            <h4>Open GCode</h4>
          </div>
          <div class="modal-body" file-dropzone (dropped)="setFile($event)">
            <file-path [resource]="resource" (changed)="listDir()" ></file-path>
            <ul class="modal-file-list">
              <li class="button" (click)="selectFile(file)" *ngFor="let file of files">{{file.name}}</li>          
            </ul>
            <p>Open file by selecting or with drag and drop to editor from desktop</p>
          </div>
          <div class="modal-footer">
            <a class="btn" (click)="close()">Cancel</a>      
          </div>
        </div>
      </div>
      </div>
    `

})
export class ResourceComponent {
  @Input('resource') resource: FileResource
  @Output() selectedFile = new EventEmitter<IFileObject>()

  private files = []
  private showModal: boolean = false
  private modalDisplay: string = "none"

  constructor(private backendService: BackendService) {

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

  open() {
    this.showModal = true;
    this.modalDisplay = "block"
    this.listDir()
  }

  save(content:string) {
    this.backendService.save(this.resource.canonical, content)
  }
  
  saveAs(content:string) {
    //TODO implement
    console.warn("Save as not yet implemented")
  }
  close() {
    this.showModal = false;
    this.modalDisplay = "none"
  }
  private setFile(file: IFileObject) {
    this.selectedFile.emit(file)
    this.close()
  }
  private openFile() {
    this.backendService.onOpenFile(this.resource.canonical).subscribe(
      data => this.selectedFile.emit(data),
      err => console.error(err),
      () => this.close()
    )
  }
  private listDir() {
    this.backendService.onListDir(this.resource.dir).subscribe(
      data => {
        this.files = data.files
        this.resource.dir = data.dir
      },
      err => console.log(err)
    )

  }
}    