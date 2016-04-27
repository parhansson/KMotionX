import {Component,Inject} from 'angular2/core';
import {RouteConfig,ROUTER_DIRECTIVES} from 'angular2/router';

import {LaserCalculatorComponent} from './laser/laser.calculator.component';
import {GCodeScreenComponent} from './gcode/gcode.screen.component';
import {CCodeScreenComponent} from './ccode/ccode.screen.component';
import {DebugScreenComponent} from './debug/debug.screen.component';
import {SettingsScreenComponent} from './settings/settings.screen.component';
import {SocketService} from './backend/socket.service'
import {PersistentRouterOutlet} from './PersistentRouterOutlet'
@Component({
    selector: 'kmx-app',
    directives: [ROUTER_DIRECTIVES, PersistentRouterOutlet],
    templateUrl:'dist/app/kmx.component.html'
})
@RouteConfig([
  {path:'/gcode',       name: 'GCodeScreen',      component: GCodeScreenComponent, useAsDefault:true},
  {path:'/ccode',       name: 'CCodeScreen',      component: CCodeScreenComponent},
  {path:'/laser-calc',  name: 'LaserCalculator',  component: LaserCalculatorComponent},
  {path:'/settings',    name: 'SettingsScreen',   component: SettingsScreenComponent},
  {path:'/debug',       name: 'DebugScreen',      component: DebugScreenComponent}
  
])
export class KmxComponent { 
    constructor(socketService:SocketService){

    }
}