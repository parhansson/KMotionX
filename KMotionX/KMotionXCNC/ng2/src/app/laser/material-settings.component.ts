import { Component, Inject, Input, Output } from '@angular/core';
import { ModelSettingsService, ModelSettings } from '../model/model.settings.service';
@Component({
  selector: 'material-settings',
  templateUrl: './material-settings.component.html'
})
export class MaterialSettingsComponent {
  modelSettings:ModelSettings
  constructor(private modelSettingsService: ModelSettingsService) {
    this.modelSettings = modelSettingsService.settings
  }
}