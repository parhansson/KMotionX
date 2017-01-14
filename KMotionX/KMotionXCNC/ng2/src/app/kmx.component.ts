import { Component, Inject } from '@angular/core';

import { SocketService } from './backend/socket.service'
import { KmxStatus } from './hal/kflop'
import { BackendService } from './backend/backend.service'

@Component({
    selector: 'kmx-app',
    templateUrl: './kmx.component.html',
    styles: [`.router-link-active { background-color: #AAA; }`]
})
export class KmxComponent {
    intStatus: KmxStatus
    constructor(//private router:Router,
        private socketService: SocketService, private backend: BackendService) {
        this.intStatus = this.socketService.data
        //socketService.simluateObservable.subscribe(()=>this.intStatus = this.socketService.data)
    }
    onSimulate() {
        this.backend.onSimulate()
    }
}