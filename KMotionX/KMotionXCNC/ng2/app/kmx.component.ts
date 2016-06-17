import {Component, Inject} from '@angular/core';
//import {RouteConfig, ROUTER_DIRECTIVES} from '@angular/router-deprecated';
import {RouteConfig, Router} from '@angular/router-deprecated';
import {RouterLink} from '@angular/router-deprecated/src/directives/router_link';

import {LaserCalculatorComponent} from './laser';
import {GCodeScreenComponent} from './gcode';
import {CCodeScreenComponent} from './ccode';
import {DebugScreenComponent} from './debug';
import {SettingsScreenComponent} from './settings';
import {SocketService} from './backend/socket.service'
import {KmxStatus} from './backend/shared'
import {BackendService} from './backend/backend.service'
import {SlideSwitchComponent} from './shared/slide-switch.component'


import {PersistentRouterOutlet} from './PersistentRouterOutlet'
@Component({
    moduleId: module.id,
    selector: 'kmx-app',
    directives: [SlideSwitchComponent,PersistentRouterOutlet, RouterLink/*, ROUTER_DIRECTIVES*/],
    templateUrl: './kmx.component.html',
    styles: [`.router-link-active { background-color: #AAA; }`]
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
    constructor(private router:Router,private socketService: SocketService, private backend: BackendService) {
        this.intStatus = this.socketService.data
        //socketService.simluateObservable.subscribe(()=>this.intStatus = this.socketService.data)


    }
    onSimulate() {
        this.backend.onSimulate()
    }
}