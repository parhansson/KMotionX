import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { AceDirective } from './ace.directive';
import { AceEditorComponent } from './ace-editor.component';
import { ResourceModule } from '../resources/resource.module'
@NgModule({
  imports: [
    CommonModule,
    ResourceModule
  ],
  exports: [AceEditorComponent],
  declarations: [
    AceDirective,
    AceEditorComponent
  ], // directives, components, and pipes owned by this NgModule
})
export class EditorModule {

}