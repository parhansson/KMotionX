import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { TabsModule } from 'ng2-bootstrap';
import { LaserCalculatorComponent } from './laser-calculator.component'
import { LaserScreenComponent } from './laser-screen.component'
import { BoxCreatorComponent } from './box-creator.component'
import { MaterialSettingsComponent } from './material-settings.component'
@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    TabsModule
  ],
  declarations: [
    LaserScreenComponent,
    LaserCalculatorComponent,
    BoxCreatorComponent,
    MaterialSettingsComponent
  ], // directives, components, and pipes owned by this NgModule
})
export class LaserModule {

}