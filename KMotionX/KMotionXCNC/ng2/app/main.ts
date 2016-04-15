/// <reference path="../node_modules/angular2/typings/browser.d.ts" />
// above needed due to some bug when resolving typings

import {bootstrap}        from 'angular2/platform/browser'
import {BackendService}   from './backend/backend.service';
import {LogService}       from './log/log.service';
import {KmxComponent}     from './kmx.component'
import {SocketService}    from './backend/socket.service'
import {SettingsService}    from './settings/settings.service'
import {ModelSettingsService} from './model/model.settings.service';
import {HTTP_PROVIDERS}   from 'angular2/http';
import {ROUTER_PROVIDERS} from 'angular2/router';
import {provide}           from 'angular2/core';
import {LocationStrategy,
  HashLocationStrategy} from 'angular2/router';

bootstrap(KmxComponent, [
  HTTP_PROVIDERS,
  ROUTER_PROVIDERS,
  SocketService,
  BackendService,
  LogService,
  SettingsService,
  ModelSettingsService
  //,
  //provide(LocationStrategy,{useClass: HashLocationStrategy})]);
  //provide(LocationStrategy,{})
]);