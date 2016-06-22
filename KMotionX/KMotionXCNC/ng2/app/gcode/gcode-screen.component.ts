import {Component, ViewChild, provide, SkipSelf, Inject} from '@angular/core';
import {Subject} from 'rxjs/Rx'
import {LogComponent} from "../log/log.component";
import {BackendService} from '../backend/backend.service';
import {SocketService} from '../backend/socket.service';
import {KmxStatus} from '../backend/shared'
import {DroComponent} from './dro.component';
import {ThreeViewComponent} from './view.component';
import {UserButtonsComponent} from './user-buttons.component';
import {ControlButtonsComponent} from './control-buttons.component';
import {ScreenComponent} from "../screen.component"
import {StaticTransformer} from '../model/transformers'
import {FileResource, Payload, FileServiceToken, IFileBackend} from '../resources'
import {SettingsService, Machine} from '../settings/settings.service'
import {GCodeEditorComponent} from './gcode-editor.component'

@Component({
  selector: 'gcode-screen',
  directives: [
    LogComponent,
    DroComponent,
    ThreeViewComponent,
    UserButtonsComponent,
    ControlButtonsComponent,
    GCodeEditorComponent
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
        <gcode-editor></gcode-editor>
        <hr>
        <user-defined-buttons></user-defined-buttons>
      </div>
    </div>  
  `
})
export class GCodeScreenComponent extends ScreenComponent {
  @ViewChild(ThreeViewComponent)
  threeComp: ThreeViewComponent;
  @ViewChild(GCodeEditorComponent)
  editorComponent: GCodeEditorComponent;
  kmxStatus: KmxStatus

  constructor(private backendService: BackendService,
    private socketService: SocketService,
    private settingsService: SettingsService,
    private staticTransformer: StaticTransformer) {
    super()
    this.kmxStatus = socketService.data;

  }
  ngAfterViewInit() {
    this.staticTransformer.threeSubject.subscribe(data => this.threeComp.model = data)
    this.settingsService.subject.subscribe((machine) => this.renderMachineObject(machine))
  }

  private machineBounds: THREE.Object3D = null;
  private machineBackground: THREE.Object3D = null;

  renderMachineObject(machine: Machine) {


    if (this.machineBounds != null) {
      this.threeComp.auxiliaryGroup.remove(this.machineBounds);
    }
    if (this.machineBackground != null) {
      this.threeComp.auxiliaryGroup.remove(this.machineBackground);
    }
    var x = machine.dimX;
    var y = machine.dimY;
    var z = machine.dimZ;
    this.machineBounds = this.createMachineBounds(x, y, z);
    this.machineBackground = this.renderBackground(x, y, z);
    this.threeComp.auxiliaryGroup.add(this.machineBounds);
    this.threeComp.auxiliaryGroup.add(this.machineBackground);
    this.threeComp.requestTick()

  }
  private createMachineBounds(x, y, z) {

    var mesh = new THREE.Mesh(new THREE.BoxGeometry(x, y, z));
    mesh.matrixWorld.makeTranslation(x / 2, y / 2, z / 2);
    var edges = new THREE.EdgesHelper(mesh, 0x0000ff);
    (edges.material as any).linewidth = 1;
    return edges;

  }

  private renderBackground(x, y, z) {

    console.log("why are we here twice?")
    var texture = new THREE.TextureLoader().load('/settings/textures/bghoneym.jpg');


    // assuming you want the texture to repeat in both directions:
    //texture.wrapS = THREE.RepeatWrapping; 
    //texture.wrapT = THREE.RepeatWrapping;

    // how many times to repeat in each direction; the default is (1,1),
    //   which is probably why your example wasn't working
    //texture.repeat.set( 4, 4 ); 

    var material = new THREE.MeshBasicMaterial({
      map: texture,
      side: THREE.DoubleSide,
      transparent: true,
      opacity: 0.3
    });
    var geometry = new THREE.PlaneGeometry(x, y);
    var mesh = new THREE.Mesh(geometry, material);
    //mesh.matrixWorld.makeTranslation(10,10,0);
    mesh.position.x = x / 2;
    mesh.position.y = y / 2;
    return mesh;

  }

}