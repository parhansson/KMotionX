import {
  Component,
  Inject,
  Input,
  ElementRef,
  OnInit,
  OnDestroy,
  ViewChild,
  AfterViewChecked
} from '@angular/core';
import { Observable, Subscription } from 'rxjs'
import { LogService, LogMessage } from './log.service'
import { LogSubject } from './log-subject'
import { LimitBuffer } from '../util'

@Component({
  selector: 'kmx-log',
  templateUrl: './log.component.html'
})
export class LogComponent implements OnInit, OnDestroy, AfterViewChecked {
  @Input()
  title: string

  @Input()
  consoleId: string

  @Input()
  logLimit: number

  @Input()
  autoscroll: boolean = true
  logs: LogMessage[] = []
  private buffer = new LimitBuffer<LogMessage>()

  @ViewChild('scrollContainer')
  private scrollContainer: ElementRef
  private logSubject: LogSubject<LogMessage>
  private subscription: Subscription

  constructor(private logService: LogService) {
    this.logLimit = 200;
  }
  //http://stackoverflow.com/questions/35232731/angular2-scroll-to-bottom-chat-style
  ngOnInit() {
    this.logSubject = this.logService.getLogSubject(this.consoleId)
    this.logLimit = this.logLimit < 1 ? 1 : this.logLimit;
    this.updateLimit()
    this.subscribe()
    //this.logService.log(this.consoleId, 'Test log ' + this.consoleId);
  }

  updateLimit() {
    this.logSubject.setBufferSize(this.logLimit)
    this.trimBufferToSize(this.logLimit)
  }

  ngAfterViewChecked() {
    //TODO this is called all the time in this view
    if (this.autoscroll) {
      this.scrollToBottom();
    }
  }
  scrollToBottom(): void {
    let container = this.scrollContainer.nativeElement;
    container.scrollTop = container.scrollHeight;
  }

  ngOnDestroy() {
    this.subscription.unsubscribe();
  }

  prune() {
    this.trimBufferToSize(0)
    this.logSubject.prune();
  }
  private pruneLocal() {
    this.logs.splice(0, this.logs.length)
  }
  private subscribe() {
    this.subscription = this.logSubject.subscribe(logMessage => {
      this.logs.push(logMessage)
      this.trimBufferToSize(this.logLimit)
    });
  }

  private trimBufferToSize(_bufferSize:number)  {
    const _events = this.logs;

    let eventsCount = _events.length;
    let spliceCount = 0;

    if (eventsCount > _bufferSize) {
      spliceCount = eventsCount - _bufferSize;
      _events.splice(0, spliceCount);
    }
  }

}