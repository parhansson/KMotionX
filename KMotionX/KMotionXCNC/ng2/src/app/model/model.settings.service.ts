import { Injectable } from '@angular/core';
export class Material {
  name: string
  ppi: string
  speed: string
  passes: number

}
export class SVGModelSettings {
  scale: number = 1
  cutZ: number = 0 //20,
  safeZ: number = 0//10
  unit: string = 'mm'
  //unit: string = 'in'
  dpi: number = 72
  fractionalDigits: number = 3
  translateToOrigo: boolean = true
  removeOutline: boolean = false
  removeDuplicates: boolean = true
  removeSingularites: boolean = true
  initCode: string = ''
  feedRate: number = 250
  passes: number = 1;
  materialWidth: number = 10
  renderText: boolean = false
}

export class ModelSettings {
  svg = new SVGModelSettings()
  pdf = {
    page: 1
  }
  materials: Material[] = [
    { name: '2mm polysterene', ppi: 'M100 P900 Q100', speed: 'F250', passes: 1 },
    { name: '3mm acryl', ppi: 'M100 P1000 Q100', speed: 'F200', passes: 1 },
    { name: '6mm acryl', ppi: 'M100 P2200 Q100 ', speed: 'F90', passes: 1 },
    { name: '4mm plywood', ppi: 'M100 P300 Q100 ', speed: 'F250', passes: 1 },
    { name: '10mm furu P900 ner till P600', ppi: 'M100 P900 Q100', speed: 'F50', passes: 1 },
    { name: '3mm plywood johans dyra', ppi: 'M100 P700 Q100', speed: 'F150', passes: 3 }

  ]

}

@Injectable()
export class ModelSettingsService {
  settings: ModelSettings = new ModelSettings();

  constructor() {
    this.settings.svg.initCode = 'M100 P200 Q100'

  }

}



