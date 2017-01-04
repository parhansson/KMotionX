import { BlockPart } from './block-part'
import { Word } from './word'

export class WordParameters extends BlockPart<Word[]> {
  constructor() {
    super([])
  }
}