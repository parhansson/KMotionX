import {Component, ViewChild} from '@angular/core';
import {LogComponent} from "../log/log.component";
import {BackendService} from '../backend/backend.service';
import {SocketService} from '../backend/socket.service';
import {KmxStatus} from '../backend/shared'
import {DroComponent} from './dro.component';
import {ThreeViewComponent} from './view.component';
import {UserButtonsComponent} from './user.buttons.component';
import {ControlButtonsComponent} from './control.buttons.component';
import {ScreenComponent} from "../screen.component"
import {StaticTransformer} from '../model/transformers'
import {FileResource, IFileObject} from '../backend/file'
import {AceEditorComponent} from '../editor/ace.editor.component'


@Component({
  selector: 'gcode-screen',
  directives: [
    LogComponent,
    DroComponent,
    ThreeViewComponent,
    UserButtonsComponent,
    ControlButtonsComponent,
    AceEditorComponent
  ],
  template: `
    <div class="row  fill">
      <div class="hidden-xs col-md-3">
        <kmx-dro></kmx-dro>
        <kmx-log title="Error log" consoleId="error" color="red"></kmx-log>
        <kmx-log title="Console" consoleId="console" color="blue"></kmx-log>
      </div>
      <div class="col-xs-9 col-md-6">
        <!-- WebGL rendering area -->
        <three-viewer class="threerenderer fill"></three-viewer>
      </div>
      <div class="col-xs-3 col-md-3">
        <control-buttons></control-buttons>
        <div>Editor line:{{kmxStatus.currentLine +1}}</div>
        <div>Interpreter line:{{kmxStatus.currentLine}}</div>
        <code-editor id="gcodeEditor" [resource]="resource" mode="gcode"></code-editor>
        <hr>
        <user-defined-buttons></user-defined-buttons>
      </div>
    </div>  
  `
})
export class GCodeScreenComponent extends ScreenComponent {
  @ViewChild(ThreeViewComponent)
  threeComp: ThreeViewComponent;
  @ViewChild(AceEditorComponent)
  editorComponent: AceEditorComponent;
  kmxStatus: KmxStatus
  resource: FileResource


  constructor(private backendService: BackendService,
    private socketService: SocketService,
    private staticTransformer: StaticTransformer) {
    super()
    this.kmxStatus = socketService.data;
    this.resource = new FileResource("");
    socketService.gcodeFileObservable.subscribe(gcodeFile => {
      this.resource.canonical = gcodeFile
    })

  }
  ngAfterViewInit() {
    this.staticTransformer.threeObservable.subscribe(data => this.threeComp.model = data)
    this.staticTransformer.gcodeSourceObservable.subscribe(data => this.editorComponent.textContent = data.text)
    this.editorComponent.onFileEventHandler = this.onOpenFile.bind(this)
  }

  private onOpenFile(file: IFileObject) {
    this.staticTransformer.transform(file.contentType, file.payload)
  }
}