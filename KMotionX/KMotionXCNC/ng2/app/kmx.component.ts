import {Component, Inject} from '@angular/core';
//import {RouteConfig, ROUTER_DIRECTIVES} from '@angular/router-deprecated';
import {RouteConfig} from '@angular/router-deprecated';
import {RouterLink} from '@angular/router-deprecated/src/directives/router_link';

import {LaserCalculatorComponent} from './laser/laser.calculator.component';
import {GCodeScreenComponent} from './gcode/gcode.screen.component';
import {CCodeScreenComponent} from './ccode/ccode.screen.component';
import {DebugScreenComponent} from './debug/debug.screen.component';
import {SettingsScreenComponent} from './settings/settings.screen.component';
import {SocketService} from './backend/socket.service'
import {KmxStatus} from './backend/shared'

import {BackendService} from './backend/backend.service'

import {PersistentRouterOutlet} from './PersistentRouterOutlet'
@Component({
    selector: 'kmx-app',
    directives: [PersistentRouterOutlet, RouterLink/*, ROUTER_DIRECTIVES*/],
    templateUrl: 'dist/app/kmx.component.html'
})
@RouteConfig([
    { path: '/gcode', name: 'GCodeScreen', component: GCodeScreenComponent, useAsDefault: true },
    { path: '/ccode', name: 'CCodeScreen', component: CCodeScreenComponent },
    { path: '/laser-calc', name: 'LaserCalculator', component: LaserCalculatorComponent },
    { path: '/settings', name: 'SettingsScreen', component: SettingsScreenComponent },
    { path: '/debug', name: 'DebugScreen', component: DebugScreenComponent }

])
export class KmxComponent {
    intStatus: KmxStatus
    constructor(private socketService: SocketService, private backend: BackendService) {
        this.intStatus = this.socketService.data
        //socketService.simluateObservable.subscribe(()=>this.intStatus = this.socketService.data)


    }
    onSimulate() {
        this.backend.onSimulate()
    }
}