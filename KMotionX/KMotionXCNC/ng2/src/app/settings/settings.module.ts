import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { TabsModule } from 'ng2-bootstrap/tabs';
import { MCodeValueComponent } from './mcode-value.component'
import { SettingsScreenComponent } from './settings-screen.component'
import { TransformerSettingsComponent } from './transformer-settings.component'

@NgModule({
  imports: [CommonModule,
    FormsModule,
    TabsModule
  ],
  declarations: [
    MCodeValueComponent,
    SettingsScreenComponent,
    TransformerSettingsComponent
  ], // directives, components, and pipes owned by this NgModule
})
export class SettingsModule {

}