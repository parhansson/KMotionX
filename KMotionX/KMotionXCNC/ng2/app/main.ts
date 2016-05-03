
import {bootstrap}        from '@angular/platform-browser-dynamic'
import {BackendService}   from './backend/backend.service';
import {LogService}       from './log/log.service';
import {KmxComponent}     from './kmx.component'
import {SocketService}    from './backend/socket.service'
import {SettingsService}    from './settings/settings.service'
import {ModelSettingsService} from './model/model.settings.service';
import {StaticTransformer} from './model/transformers'
import {HTTP_PROVIDERS}   from '@angular/http';
import {ROUTER_PROVIDERS} from '@angular/router-deprecated';
import {provide}           from '@angular/core';
import {enableProdMode} from '@angular/core';

enableProdMode();
//import {LocationStrategy, HashLocationStrategy} from '@angular/platform-browser-dynamic';

bootstrap(KmxComponent, [
  HTTP_PROVIDERS,
  ROUTER_PROVIDERS,
  SocketService,
  BackendService,
  LogService,
  SettingsService,
  ModelSettingsService,
  StaticTransformer
  //,
  //provide(LocationStrategy,{useClass: HashLocationStrategy})]);
  //provide(LocationStrategy,{})
]);