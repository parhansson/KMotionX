
import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ControlButtonsComponent } from './control-buttons.component';
import { DroComponent } from './dro.component';
import { GCodeScreenComponent } from './gcode-screen.component';
import { UserButtonsComponent } from './user-buttons.component';
import { ThreeViewComponent } from './view.component';
import { GCodeEditorComponent } from './gcode-editor.component'
import { LogModule } from '../log/log.module';
import { EditorModule } from '../editor/editor.module';
import { ResourceModule } from '../resources/resource.module'
@NgModule({
  imports: [CommonModule,
    LogModule,
    ResourceModule,
    EditorModule,
  ],
  declarations: [
    GCodeScreenComponent,
    ControlButtonsComponent,
    DroComponent,
    GCodeEditorComponent,
    UserButtonsComponent,
    ThreeViewComponent
  ], // directives, components, and pipes owned by this NgModule
})
export class GCodeModule {

}