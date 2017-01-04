import { BlockPart } from './block-part'
import { Word } from './word'

export class Block {
  line: number
  parts: BlockPart<string | Word[]>[] = []

  constructor(public text: string) { }

}


