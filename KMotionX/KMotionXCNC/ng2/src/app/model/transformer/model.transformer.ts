import { Observer } from 'rxjs/Rx';

export abstract class ModelTransformer<Source, Target> {
  inputMime: string[]
  outputMime: string
  name: string
  abstract execute(source: Source, targetObserver: Observer<Target>)//: Observable<Target>
}