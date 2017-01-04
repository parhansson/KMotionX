import { NgModule }                 from '@angular/core';
import { CommonModule }             from '@angular/common';
import { FormsModule }              from '@angular/forms';
import { LaserCalculatorComponent } from './laser-calculator.component'
@NgModule({
  imports: [
    CommonModule,
    FormsModule
  ],
  declarations: [
    LaserCalculatorComponent
  ], // directives, components, and pipes owned by this NgModule
})
export class LaserModule {

}