
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { NgClass } from '@angular/common';
import { SlideSwitchComponent } from './slide-switch.component'


@NgModule({
  imports: [
    FormsModule
  ],
  exports: [SlideSwitchComponent],
  declarations: [SlideSwitchComponent], // directives, components, and pipes owned by this NgModule
})
export class SharedModule {

}