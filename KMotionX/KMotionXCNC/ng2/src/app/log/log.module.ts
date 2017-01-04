import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { LogComponent } from './log.component'
@NgModule({
  imports: [
    CommonModule
  ],
  exports: [LogComponent],
  declarations: [
    LogComponent
  ], // directives, components, and pipes owned by this NgModule
})
export class LogModule {

}