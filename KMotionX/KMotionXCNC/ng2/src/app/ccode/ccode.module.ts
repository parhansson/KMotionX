
import { NgModule }         from '@angular/core';
import { CommonModule }     from '@angular/common';
import { TabsModule }       from 'ng2-bootstrap';
import { EditorModule }     from '../editor/editor.module';
import { ResourceModule }   from '../resources/resource.module'
import { CCodeScreenComponent } from './ccode-screen.component'
import { CCodeEditorComponent } from './ccode-editor.component'
@NgModule({
  imports: [CommonModule,
    ResourceModule,
    EditorModule,
    TabsModule,
    ],
  declarations: [CCodeScreenComponent,
  CCodeEditorComponent
  ], // directives, components, and pipes owned by this NgModule
})
export class CCodeModule {

}