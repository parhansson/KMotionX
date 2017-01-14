export class IOPin {
  state: boolean = false
  output: boolean = true
  constructor(public name: string, public description: string, public number?: number) { }
}
