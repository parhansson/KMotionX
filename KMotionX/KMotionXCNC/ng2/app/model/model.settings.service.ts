import {Injectable} from '@angular/core';

export class SVGModelSettings {
  scale: number = 1
  cutZ: number = 0 //20,
  safeZ: number = 0//10
  unit: string = "mm"
  dpi: number = 72
  fractionalDigits: number = 3
  translateToOrigo: boolean = true
  removeOutline: boolean = false
  removeDuplicates: boolean = true
  removeSingularites: boolean = true
  initCode: string = ''
  feedRate: number = 250
  passes:number =  1;
  materialWidth:number = 10
  renderText:boolean = false
}

export class ModelSettings {
  svg = new SVGModelSettings()
  pdf = {
    page: 1
  }

}

@Injectable()
export class ModelSettingsService {
  settings: ModelSettings = new ModelSettings();

  constructor() {
    this.settings.svg.initCode = 'M100 P200 Q100'

  }

}



