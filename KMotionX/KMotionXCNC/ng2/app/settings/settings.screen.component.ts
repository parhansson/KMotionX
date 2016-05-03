import {Component,Inject} from 'angular2/core';
import {SettingsService,Machine} from './settings.service';
import {MCodeComponent} from './mcode.component';
import {ScreenComponent} from "../screen.component"
import {ModelSettings, ModelSettingsService} from '../model/model.settings.service';

//import {Tab,Tabs} from '../tabs';
import { TAB_DIRECTIVES } from 'ng2-bootstrap/ng2-bootstrap';

@Component({
    selector : 'settings-screen',
    directives: [TAB_DIRECTIVES,MCodeComponent],
    templateUrl:'dist/app/settings/settings.screen.html'
})
export class SettingsScreenComponent extends ScreenComponent{ 
    machine:Machine
    transformerSettings:ModelSettings
    constructor(private settingsService:SettingsService, private modelSettingsService:ModelSettingsService){
      super()
      this.machine = settingsService.machine
      this.transformerSettings = modelSettingsService.settings
    }
    
    save(){
        this.settingsService.save()
    }
}