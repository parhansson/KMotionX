
import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { LogModule } from '../log/log.module'
import { DebugScreenComponent } from './debug-screen.component'
import { DigitalIOComponent, ConnectorComponent } from './digital-io.component'


@NgModule({
  imports: [
    CommonModule,
    LogModule
  ],
  declarations: [
    DebugScreenComponent,
    ConnectorComponent,
    DigitalIOComponent
  ], // directives, components, and pipes owned by this NgModule
})
export class DebugModule {

}