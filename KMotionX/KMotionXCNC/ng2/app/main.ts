

// Ahead of Time compile
//import {platformBrowser} from '@angular/platform-browser';
//import {KmxAppModuleNgFactory} from './kmx_module.ngfactory';
//platformBrowser().bootstrapModuleFactory(MyAppModuleNgFactory);
//platformBrowser().bootstrapModuleFactory(KmxAppModuleNgFactory);

// JIT compile long form
import { KmxAppModule } from './kmx.module';
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
platformBrowserDynamic().bootstrapModule(KmxAppModule);
