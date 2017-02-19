
import { NgModule } from '@angular/core';
import { enableProdMode } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpModule } from '@angular/http';
import { RouteReuseStrategy } from '@angular/router';
import { TabsModule, DropdownModule } from 'ng2-bootstrap';
import { routing, appRoutingProviders } from './kmx.routing';
import { BackendService } from './backend/backend.service';
import { KFlopBackendService } from './backend/kflop/kflop.backend.service';
import { LogService } from './log';
import { KmxComponent } from './kmx.component'
import { SocketService } from './backend/socket.service'
import { SettingsService } from './settings/settings.service'
import { ModelSettingsService } from './model/model.settings.service';
import { StaticTransformer } from './model/transformers'
import { FileServiceToken } from './resources'
import { SharedModule } from './shared/shared.module'
import { FormsModule } from '@angular/forms';
import { GCodeModule } from './gcode/gcode.module'
import { CCodeModule } from './ccode/ccode.module'
import { LogModule } from './log/log.module';
import { EditorModule } from './editor/editor.module';
import { ResourceModule } from './resources/resource.module'
import { SettingsModule } from './settings/settings.module'
import { DebugModule } from './debug/debug.module'
import { LaserModule } from './laser/laser.module'
import { CustomReuseStrategy } from './route-reuse.strategy'
@NgModule({
  imports: [
    BrowserModule,
    HttpModule,
    TabsModule.forRoot(),
    DropdownModule.forRoot(),
    FormsModule,

    SharedModule,
    DebugModule,
    ResourceModule,
    EditorModule,
    GCodeModule,
    CCodeModule,
    LogModule,
    LaserModule,
    SettingsModule,
    routing
    //Ng2BootstrapModule
  ],
  declarations: [
    KmxComponent,
  ], // directives, components, and pipes owned by this NgModule
  providers: [
    appRoutingProviders,
    SocketService,
    LogService,
    SettingsService,
    ModelSettingsService,
    StaticTransformer,
    { provide: BackendService, useClass: KFlopBackendService },
    { provide: FileServiceToken, useExisting: BackendService },
    { provide: RouteReuseStrategy, useClass: CustomReuseStrategy }
  ], // additional providers
  bootstrap: [KmxComponent],
})
export class KmxAppModule {

}