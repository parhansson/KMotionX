import {Component, Inject} from '@angular/core';
import {SettingsService, Machine} from './settings.service';
import {MCodeComponent} from './mcode.component';
import {ScreenComponent} from "../screen.component"
import {ModelSettings, ModelSettingsService} from '../model/model.settings.service';

@Component({
    selector: 'transformer-settings',
    template: `
<h3 class="settings-header">Import settings</h3>
      <form class="form">
        <div class="row">
          <div class="col-xs-12 col-sm-8 col-sm-offset-2">
            <div class="form-group">
              <label>Pdf import page number</label>
              <input class="form-control" [(ngModel)]="transformerSettings.pdf.page" min="1" type="number" step="any" />
            </div>
            <div class="form-group">
              <label>SVG scale</label>
              <input class="form-control" [(ngModel)]="transformerSettings.svg.scale" min="0" type="number" step="any" />
            </div>
            <div class="form-group">
              <label>SVG DPI</label>
              <input class="form-control" [(ngModel)]="transformerSettings.svg.dpi" min="1" type="number" step="any" />
            </div>
            <div class="form-group">
              <label>Fractional digits (resolution)</label>
              <input class="form-control" [(ngModel)]="transformerSettings.svg.fractionalDigits" min="0" type="number" step="any" />
            </div>
            <div class="form-group">
              <label>Init code</label>
              <input class="form-control" [(ngModel)]="transformerSettings.svg.initCode" type="text" />
            </div>
            <div class="form-group">
              <label>Feed rate</label>
              <input class="form-control" [(ngModel)]="transformerSettings.svg.feedRate" min="1" type="number" step="any" />
            </div>
            <div class="checkbox">
              <label>
                <input type="checkbox" [(ngModel)]="transformerSettings.svg.removeOutline" /> Remove outline (not complete)
              </label>
            </div>
            <div class="checkbox">
              <label>
                <input type="checkbox" [(ngModel)]="transformerSettings.svg.removeDuplicates" /> Remove duplicates (not complete, might remove too much)
              </label>
            </div>
            <div class="checkbox">
              <label>
                <input type="checkbox" [(ngModel)]="transformerSettings.svg.removeSingularites" /> Remove single points
              </label>
            </div>
            <div class="checkbox">
              <label>
                <input type="checkbox" [(ngModel)]="transformerSettings.svg.translateToOrigo" /> Move to origo
              </label>
            </div>
          </div>
        </div>
      </form>    
    
    
    `
})
export class TransformerSettingsComponent{
    transformerSettings: ModelSettings
    constructor(private settingsService: SettingsService, private modelSettingsService: ModelSettingsService) {
        this.transformerSettings = modelSettingsService.settings
    }

    save() {
     
    }
}