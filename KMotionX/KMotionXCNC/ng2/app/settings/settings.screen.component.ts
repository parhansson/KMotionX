import {Component} from '@angular/core';
import {SettingsService, Machine} from './settings.service';
import {MCodeComponent} from './mcode.component';
import {ScreenComponent} from "../screen.component"
import {ModelSettings, ModelSettingsService} from '../model/model.settings.service';
import {TAB_DIRECTIVES} from 'ng2-bootstrap/ng2-bootstrap';
import {TransformerSettingsComponent} from './TransformerSettingsComponent'

@Component({
    selector: 'settings-screen',
    directives: [TAB_DIRECTIVES, MCodeComponent, TransformerSettingsComponent],
    templateUrl: 'dist/app/settings/settings.screen.html'
})
export class SettingsScreenComponent extends ScreenComponent {
    machine: Machine
    transformerSettings: ModelSettings
    constructor(private settingsService: SettingsService, private modelSettingsService: ModelSettingsService) {
        super()
        this.machine = settingsService.machine
        this.transformerSettings = modelSettingsService.settings
    }

    save() {
        this.settingsService.save()
    }
}