
import { NgModule }         from '@angular/core';
import { provide }          from '@angular/core';
import { enableProdMode }   from '@angular/core';
import { BrowserModule }    from '@angular/platform-browser';
import { HttpModule }       from '@angular/http';
import { TabsModule }       from 'ng2-bootstrap/ng2-bootstrap';
//import { Ng2BootstrapModule } from 'ng2-bootstrap/ng2-bootstrap';
import { ROUTER_PROVIDERS } from '@angular/router-deprecated';

import {BackendService}   from './backend/backend.service';
import {KFlopBackendService}   from './backend/kflop/kflop.backend.service';
import {LogService}       from './log';
import {KmxComponent}     from './kmx.component'
import {SocketService}    from './backend/socket.service'
import {SettingsService}    from './settings/settings.service'
import {ModelSettingsService} from './model/model.settings.service';
import {StaticTransformer} from './model/transformers'
import {FileServiceToken} from './resources'
import './global' //Force loding globals such as three and three-trackballcontrols

@NgModule({
  imports: [
    BrowserModule,
    HttpModule,
    TabsModule 
    //Ng2BootstrapModule
    ],
  declarations: [KmxComponent], // directives, components, and pipes owned by this NgModule
  providers: [
    ROUTER_PROVIDERS,
    SocketService,
    LogService,
    SettingsService,
    ModelSettingsService,
    StaticTransformer,
    provide(BackendService, { useClass: KFlopBackendService }),
    provide(FileServiceToken, { useExisting: BackendService })], // additional providers
    bootstrap: [KmxComponent],
})
export class KmxAppModule {

}