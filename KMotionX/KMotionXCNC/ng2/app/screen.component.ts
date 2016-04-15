import {ComponentInstruction, CanReuse, OnReuse, RouteParams} from 'angular2/router';

export class ScreenComponent implements CanReuse,
  OnReuse {
  constructor() {

  }

  routerCanReuse(next: ComponentInstruction, prev: ComponentInstruction) {
    return true;
  }
  routerOnReuse(next: ComponentInstruction, prev: ComponentInstruction) {

  }

}