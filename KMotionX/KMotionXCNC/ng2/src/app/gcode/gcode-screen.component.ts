import { Component, ViewChild, SkipSelf, Inject } from '@angular/core';
import { Subject } from 'rxjs/Rx'
import { LogComponent } from '../log/log.component';
import { BackendService } from '../backend/backend.service';
import { SocketService } from '../backend/socket.service';
import { KmxStatus } from '../hal/kflop'
import { DroComponent } from './dro.component';
import { ThreeViewComponent } from './view.component';
import { UserButtonsComponent } from './user-buttons.component';
import { ControlButtonsComponent } from './control-buttons.component';
import { StaticTransformer } from '../model/transformers'
import { FileResource, Payload, FileServiceToken, IFileBackend } from '../resources'
import { SettingsService, Machine } from '../settings/settings.service'
import { GCodeEditorComponent } from './gcode-editor.component'


@Component({
  selector: 'gcode-screen',
  template: `
    <div class="row  fill">
      <div class="hidden-xs col-md-3">
        <kmx-dro></kmx-dro>
        <kmx-log title="Error log" consoleId="error"></kmx-log>
        <kmx-log title="Console" consoleId="console"></kmx-log>
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
export class GCodeScreenComponent {
  @ViewChild(ThreeViewComponent)
  threeComp: ThreeViewComponent;
  @ViewChild(GCodeEditorComponent)
  editorComponent: GCodeEditorComponent;
  kmxStatus: KmxStatus

  constructor(private backendService: BackendService,
    private socketService: SocketService,
    private settingsService: SettingsService,
    private staticTransformer: StaticTransformer) {
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
    let x = machine.dimX;
    let y = machine.dimY;
    let z = machine.dimZ;
    this.machineBounds = this.createMachineBounds(x, y, z);
    this.machineBackground = this.renderBackground(x, y, z);
    this.threeComp.auxiliaryGroup.add(this.machineBounds);
    this.threeComp.auxiliaryGroup.add(this.machineBackground);
    this.threeComp.requestTick()

  }
  private createMachineBounds(x, y, z) {

    let boxGeom = new THREE.BoxGeometry(x, y, z);
    boxGeom.translate(x / 2, y / 2, z / 2);
    let material = new THREE.LineBasicMaterial({ color: 0x000000, linewidth: 1, opacity: 0.5 });
    let edges = new THREE.LineSegments(
      new THREE.EdgesGeometry(boxGeom as any, undefined),
      material);
    return edges;
  }

  private renderBackground(x, y, z) {

    console.log('debounce?', x, y, z)
    var texture = new THREE.TextureLoader().load('/settings/textures/bghoneym.jpg');


    // assuming you want the texture to repeat in both directions:
    //texture.wrapS = THREE.RepeatWrapping; 
    //texture.wrapT = THREE.RepeatWrapping;

    // how many times to repeat in each direction; the default is (1,1),
    //   which is probably why your example wasn't working
    //texture.repeat.set( 4, 4 ); 

    let material = new THREE.MeshBasicMaterial({
      map: texture,
      side: THREE.DoubleSide,
      transparent: true,
      opacity: 0.3
    });
    let geometry = new THREE.PlaneGeometry(x, y);
    geometry.translate(x / 2, y / 2, 0);
    let mesh = new THREE.Mesh(geometry, material);
    return mesh;

  }

}