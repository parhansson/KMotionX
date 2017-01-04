import { Component, Input } from '@angular/core';
import { Action } from './settings.service'

@Component({
    selector: 'kmx-mcode-value',
    templateUrl: './mcode-value.html'
})
export class MCodeValueComponent {

    @Input() userButton = false;
    @Input() action: Action;

    actionOptions = [
        //{action:-1,name:'Reserved', visible:[false,false,false,false,false,false],paramNames:['','','','','','']},
        { action: 0, name: 'None', visible: [false, false, false, false, false, false], paramNames: ['', '', '', '', '', ''] },
        { action: 1, name: 'Set a bit high or low', visible: [true, true, false, false, false, false], paramNames: ['Set bit', 'to', '', '', '', ''] },
        { action: 2, name: 'Set two bits either high or low', visible: [true, true, true, true, false, false], paramNames: ['Set bit', 'to', ' and bit', 'to', '', ''] },
        { action: 3, name: 'Output a value to a DAC', visible: [true, true, true, true, true, false], paramNames: ['Set DAC: ', 'scale: ', 'offset: ', 'min: ', 'max: ', ''] },
        { action: 4, name: 'Run a KMotion User C program', visible: [true, true, false, false, false, true], paramNames: ['Thread', 'VAR', '', '', '', 'File'] },
        { action: 5, name: 'Run a KMotion User C program wait til finished', visible: [true, true, false, false, false, true], paramNames: ['Thread', 'VAR', '', '', '', 'File'] },
        { action: 6, name: 'Run a KMotion User C program wait til finished, resync positions', visible: [true, true, false, false, false, true], paramNames: ['Thread', 'VAR', '', '', '', 'File'] },
        { action: 7, name: 'Run a Windows program wait til finished', visible: [false, false, false, false, false, true], paramNames: ['', '', '', '', '', 'File'] },
        { action: 8, name: 'Callback to the User Application', visible: [false, false, false, false, false, false], paramNames: ['', '', '', '', '', ''] },
        { action: 9, name: 'Wait/Hold until a bit is high or low', visible: [true, true, false, false, false, false], paramNames: ['Wait bit', 'till', '', '', '', ''] }
    ];

    constructor() {

    }


}