import {Component, Inject, ViewChild} from '@angular/core';
import {AceDirective} from "../editor/ace.directive";
import {LogComponent} from "../log/log.component";
import {BackendService} from '../backend/backend.service';
import {SocketService} from '../backend/socket.service';
import {KmxStatus} from '../backend/shared'
import {DroComponent} from './dro.component';
import {ModelSettingsService} from '../model/model.settings.service';
import {ThreeViewComponent} from './view.component';
import {UserButtonsComponent} from './user.buttons.component';
import {ScreenComponent} from "../screen.component"
import {ResourceComponent, FilePathComponent, FileResource, FileDropZone}    from '../resources/resource.component'
import {StaticTransformer} from '../model/transformers'



@Component({
  selector: 'gcode-screen',
  directives: [
    AceDirective,
    LogComponent,
    DroComponent,
    ThreeViewComponent,
    UserButtonsComponent,
    ResourceComponent,
    FilePathComponent,
    FileDropZone
  ],
  templateUrl: 'dist/app/gcode/gcode.screen.html'
})
export class GCodeScreenComponent extends ScreenComponent {
  @ViewChild(ThreeViewComponent)
  threeComp: ThreeViewComponent;
  gcodetext: string
  intStatus: KmxStatus
  resource: FileResource


  constructor(private backendService: BackendService,
    //private modelSettings: ModelSettingsService, 
    private socketService: SocketService,
    private staticTransformer: StaticTransformer) {
    super()
    this.intStatus = socketService.data;
    this.staticTransformer.threeObservable.subscribe(data => this.onThree(data))
    this.staticTransformer.gcodeSourceObservable.subscribe(data => this.gcodetext = data.text)
    this.resource = new FileResource("", '');
    socketService.gcodeFileObservable.subscribe(gcodeFile => {
      this.resource.canonical = gcodeFile
    })
  }

  onFeedhold() {
    this.backendService.onFeedhold();
  }

  onHalt() {
    this.backendService.onHalt();
  }

  onCycleStart() {
    this.backendService.onCycleStart()
  }

  onReset() {
    this.backendService.onReset();
  }

  onStep() {
    this.backendService.onStep();
  }
  onEmergencyStop() {
    this.backendService.onEmergencyStop();
  }
  onSave() {
    this.backendService.save(this.resource.canonical, this.gcodetext)
  }
  onOpenFile(event) {
    this.staticTransformer.transform(event.contentType, event.payload)
  }

  private onThree(data: any) {
    console.log(data)
    var view = this.threeComp.viewer;
    view.modelObject.add(data);
    view.updateFn();
    //this.threeComp.requestTick();
  }
}