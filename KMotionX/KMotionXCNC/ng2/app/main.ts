
import {bootstrap}        from '@angular/platform-browser-dynamic'
import {HTTP_PROVIDERS}   from '@angular/http';
import {ROUTER_PROVIDERS} from '@angular/router-deprecated';
import {provide}           from '@angular/core';
import {enableProdMode} from '@angular/core';
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

//enableProdMode();

//import {LocationStrategy, HashLocationStrategy} from '@angular/platform-browser-dynamic';

bootstrap(KmxComponent, [
  HTTP_PROVIDERS,
  ROUTER_PROVIDERS,
  SocketService,
  LogService,
  SettingsService,
  ModelSettingsService,
  StaticTransformer,
  provide(BackendService, { useClass: KFlopBackendService }),
  provide(FileServiceToken, { useExisting: BackendService })
  //,
  //provide(LocationStrategy,{useClass: HashLocationStrategy})]);
  //provide(LocationStrategy,{})
]);