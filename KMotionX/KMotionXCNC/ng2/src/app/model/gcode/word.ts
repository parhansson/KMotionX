import { BlockPart } from './block-part'

export abstract class Word extends BlockPart<string> {
  constructor(public literal: string, public address: number) {
    super(literal + address)
  }
}