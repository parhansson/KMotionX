import {Component, Inject, ViewChild} from 'angular2/core';
import {AceDirective} from "../editor/ace.directive";
import {LogComponent} from "../log/log.component";
import {BackendService} from '../backend/backend.service';
import {SocketService} from '../backend/socket.service';
import {DroComponent} from './dro.component';
import {Svg2Igm} from '../model/svg/svgreader';
import {IGM, GCodeSource} from '../model/igm/igm';
import {igm2gcode} from '../model/igm/igm2gcode';
import {ModelSettingsService} from '../model/model.settings.service';
import {Gcode2Three} from '../model/gcode/gcode2three';
import {PDF2SVG} from '../model/pdf/pdf2svg';
import {Observable} from 'rxjs/Observable';
import {Observer} from 'rxjs/Observer';
import {ThreeViewComponent} from './view.component';
import {UserButtonsComponent} from './user.buttons.component';
import {ScreenComponent} from "../screen.component"
import {ResourceComponent}    from '../resources/resource.component'
import {KMXUtil}    from '../util/KMXUtil'
import {StaticTransformer} from '../model/transformers'



@Component({
  selector: 'gcode-screen',
  directives: [AceDirective, LogComponent, DroComponent, ThreeViewComponent, UserButtonsComponent, ResourceComponent],
  templateUrl: 'dist/app/gcode/gcode.screen.html'
})
export class GCodeScreenComponent extends ScreenComponent {
  @ViewChild(ThreeViewComponent)
  threeComp: ThreeViewComponent;
  gcodetext: string
  currentDir: string
  editorContentName: string
  intStatus: any
  staticTransformer:StaticTransformer

  //ngAfterViewInit() {
  // threeComp is set
  //console.log("Hejja", this.threeComp)
  //}

  constructor(private backendService: BackendService, private modelSettings: ModelSettingsService, socketService: SocketService) {
    super()
    this.staticTransformer =  new StaticTransformer(modelSettings)
    console.log("new GCodeScreenComponent()");
    this.intStatus = socketService.data;
    this.currentDir = "./gcode"
    this.editorContentName = "test.pdf"
    this.staticTransformer.threeObservable.subscribe(data => this.onThree(data))
    this.staticTransformer.gcodeSourceObservable.subscribe(data => this.gcodetext = data.text)
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