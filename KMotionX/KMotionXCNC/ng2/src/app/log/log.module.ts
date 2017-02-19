import { NgModule } from '@angular/core';
import { FormsModule }   from '@angular/forms';
import { CommonModule } from '@angular/common';
import { DropdownModule } from 'ng2-bootstrap';
import { LogComponent } from './log.component'
@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    DropdownModule
  ],
  exports: [LogComponent],
  declarations: [
    LogComponent
  ], // directives, components, and pipes owned by this NgModule
})
export class LogModule {

}