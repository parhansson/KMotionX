import { Component } from '@angular/core';
import { SettingsService, Machine } from './settings.service';
import { MCodeValueComponent } from './mcode-value.component';
import { ModelSettings, ModelSettingsService } from '../model/model.settings.service';
import { TransformerSettingsComponent } from './transformer-settings.component'

@Component({
    selector: 'settings-screen',
    templateUrl: './settings-screen.html'
})
export class SettingsScreenComponent {
    machine: Machine
    transformerSettings: ModelSettings

    constructor(private settingsService: SettingsService,
        private modelSettingsService: ModelSettingsService) {

        settingsService.subject.subscribe(machine => this.machine = machine);
        this.transformerSettings = modelSettingsService.settings;
    }

    save() {
        this.settingsService.save()
    }
}