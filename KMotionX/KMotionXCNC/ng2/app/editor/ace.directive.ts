
import {Component, Directive, EventEmitter, ElementRef} from '@angular/core';

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
    public editor: AceAjax.Editor;
    public textChanged: EventEmitter<string>;

    set text(s: string) {
        this.editor.setValue(s);
        this.editor.clearSelection();
        //this.editor.focus();
    }
    get text(){
        return this.editor.getValue()
    }
    set theme(theme: string) {
        if (theme) {
            this.editor.setTheme("ace/theme/" + theme);
        }
    }
    set mode(mode: string) {
        if (mode) {
            this.editor.getSession().setMode("ace/mode/" + mode);
        }
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


        this.editor.addEventListener("change", (e) => {
            console.log("changed: ", e)
            this.textChanged.next(this.editor.getValue());
        });
    }
}