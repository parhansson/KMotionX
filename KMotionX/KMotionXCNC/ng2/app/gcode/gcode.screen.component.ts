import {Component, Inject, ContentChild, ViewChild} from 'angular2/core';
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
import {Observable} from 'rxjs/Observable';
import {Observer} from 'rxjs/Observer';
import {ThreeViewComponent} from './view.component';
import {UserButtonsComponent} from './user.buttons.component';
import {ScreenComponent} from "../screen.component"

@Component({
  selector: 'gcode-screen',
  directives: [AceDirective, LogComponent, DroComponent, ThreeViewComponent, UserButtonsComponent],
  templateUrl: 'dist/app/gcode/gcode.screen.html'
})
export class GCodeScreenComponent extends ScreenComponent{
  @ContentChild(ThreeViewComponent)
  threeComp: ThreeViewComponent;
  gcodetext: string
  ngAfterContentInit() {
    // threeComp is set
    console.log("Hejja", this.threeComp)
  }

  intStatus: any
  constructor(private backendService: BackendService, private modelSettings: ModelSettingsService, socketService: SocketService) {
    super()
    console.log("new GCodeScreenComponent()");
    this.intStatus = socketService.data;
  }

  onFeedhold() {
    this.backendService.onFeedhold();
  }

  onHalt() {
    this.backendService.onHalt();
  }

  onCycleStart() {
    this.backendService.onCycleStart();
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

  onOpenFile() {
    this.backendService.load("test.svg").subscribe(
      data => this.parseXML(data),
      err => console.error(err),
      () => console.log('Random Quote Complete')
    );
  }
  setThree(tv: ThreeViewComponent) {
    this.threeComp = tv;
  }
  parseXML(source: any) {
    if (true/*window.DOMParser*/) {
      var parser = new DOMParser();
      // clean off any preceding whitespace
      //not sure if this is really needed
      var svgstring = source._body.replace(/^[\n\r \t]/gm, '');
      var doc: any = null;
      doc = <SVGElement>(parser.parseFromString(svgstring, 'image/svg+xml').documentElement as any);
      var igm: IGM = new Svg2Igm().transform(doc, null);
      //igm.alllayers;
      //console.log(igm.alllayers);
      //console.log(igm.applyModifications(this.modelSettings.settings.svg, 1));
      //console.log(igm.applyModifications(this.modelSettings.settings.svg, 1));
      var gcode = new GCodeSource(new igm2gcode().transform(igm, {}));
      this.gcodetext = gcode.text;
      console.log(gcode)
      var transformobserver: Observer<any>;
      var observable = new Observable(observer => transformobserver = observer)
      observable.subscribe(
        data => this.onThree(data),
        err => console.error(err),
        () => console.log('testa')
      );

      //               .startWith({})
      //               .share();

      new Gcode2Three().transform(transformobserver, gcode, true);
    } else {
      //console.error("DOMParser not supported. Update your browser");
    }
    //return null;     
  }
  private onThree(data: any) {
    console.log(data)
    var view = this.threeComp.viewer;
    view.modelObject.add(data);
    view.updateFn();
    //this.threeComp.requestTick();
  }
}