import {Injectable} from '@angular/core';
export class ModelSettings {
  svg = {
    scale: 1,
    cutZ: 0, //20,
    safeZ: 0,//10,
    unit: "mm",
    dpi: 72,
    fractionalDigits: 2,
    translateToOrigo: true,
    removeOutline: false,
    removeDuplicates: true,
    removeSingularites: true,
    initCode: 'M100 P200 Q100',
    feedRate: 250

  }
  pdf = {
    page: 1
  }

}

@Injectable()
export class ModelSettingsService {
  settings: ModelSettings = new ModelSettings();

  constructor() {
    this.settings = {
      svg: {
        scale: 1,
        cutZ: 0, //20,
        safeZ: 0,//10,
        unit: "mm",
        dpi: 72,
        fractionalDigits: 2,
        translateToOrigo: true,
        removeOutline: false,
        removeDuplicates: true,
        removeSingularites: true,
        initCode: 'M100 P200 Q100',
        feedRate: 250

      },
      pdf: {
        page: 1
      }
    }
  }

}



