import {Component,Inject} from 'angular2/core';
import {SettingsService} from './settings.service';
import {MCodeComponent} from './mcode.component';
import {ScreenComponent} from "../screen.component"

//import {Tab,Tabs} from '../tabs';
import { TAB_DIRECTIVES } from 'ng2-bootstrap/ng2-bootstrap';

@Component({
    selector : 'settings-screen',
    directives: [TAB_DIRECTIVES,MCodeComponent],
    templateUrl:'dist/app/settings/settings.screen.html'
})
export class SettingsScreenComponent extends ScreenComponent{ 
    machine = {};
    transformerSettings = {pdf:{},svg:{}};
    constructor(settingsService:SettingsService){
      super()
      this.machine = settingsService.machine;
    }
    

}