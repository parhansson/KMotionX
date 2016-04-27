/// <reference path="../../typings/main/ambient/ace/index.d.ts" />
import {Component,Directive,EventEmitter,ElementRef} from 'angular2/core';

@Directive({
    selector: '[aceEditor]',
    inputs: [
        "text",
        "mode",
        "theme"
    ],
    outputs: [
        "textChanged"
    ]
})
export class AceDirective { 
    private editor;
    public textChanged: EventEmitter<string>;

    set text(s: string) {
        this.editor.setValue(s);
        this.editor.clearSelection();
        //this.editor.focus();
    }
    set theme(t:string){
      this.editor.setTheme("ace/theme/"+ t);
    }
    set mode(m:string){
      this.editor.getSession().setMode("ace/mode/" + m);
    }

    constructor(elementRef: ElementRef) {
        this.textChanged = new EventEmitter<string>();

        let el = elementRef.nativeElement;
        el.classList.add("editor");
        //el.style.height = "250px";
        //el.style.width = "300px";
        el.style.height = "100%";
        el.style.width = "100%";
        
        this.editor = ace.edit(el);
        this.editor.resize(true);
        this.editor.setTheme("ace/theme/chrome");
        

        this.editor.on("change", (e) => {
            this.textChanged.next(this.editor.getValue());
        });
    }
}