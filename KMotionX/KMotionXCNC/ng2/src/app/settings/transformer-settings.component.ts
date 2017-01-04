import { Component, Inject } from '@angular/core';
import { SettingsService, Machine } from './settings.service';
import { ModelSettings, ModelSettingsService } from '../model/model.settings.service';

@Component({
  selector: 'transformer-settings',
  templateUrl: 'transformer-settings.html'
})
export class TransformerSettingsComponent {
  transformerSettings: ModelSettings
  constructor(private settingsService: SettingsService, private modelSettingsService: ModelSettingsService) {
    this.transformerSettings = modelSettingsService.settings
  }

  save() {

  }
}