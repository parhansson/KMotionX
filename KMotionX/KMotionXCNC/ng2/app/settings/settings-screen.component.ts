import {Component} from '@angular/core';
import {TAB_DIRECTIVES} from 'ng2-bootstrap/ng2-bootstrap';
import {SettingsService, Machine} from './settings.service';
import {MCodeValueComponent} from './mcode-value.component';
import {ScreenComponent} from "../screen.component"
import {ModelSettings, ModelSettingsService} from '../model/model.settings.service';
import {TransformerSettingsComponent} from './transformer-settings.component'

@Component({
    moduleId: module.id,
    selector: 'settings-screen',
    directives: [TAB_DIRECTIVES, MCodeValueComponent, TransformerSettingsComponent],
    templateUrl: './settings-screen.html'
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