import { Routes, RouterModule } from '@angular/router';
import { ModuleWithProviders } from '@angular/core';
import {LaserCalculatorComponent} from './laser';
import {GCodeScreenComponent} from './gcode';
import {CCodeScreenComponent} from './ccode';
import {DebugScreenComponent} from './debug';
import {SettingsScreenComponent} from './settings';


const appRoutes: Routes = [
    { path: 'gcode',  
      component: GCodeScreenComponent,
      //outlet : 'main'
      //, useAsDefault: true,
    },
    { path: 'ccode',  
      component: CCodeScreenComponent },
    { path: 'laser-calc', 
      component: LaserCalculatorComponent },
    { path: 'settings',
       component: SettingsScreenComponent },
    { path: 'debug',  
      component: DebugScreenComponent },
    { path: '',
      component: GCodeScreenComponent }
];

export const appRoutingProviders: any[] = [

];

export const routing: ModuleWithProviders = RouterModule.forRoot(appRoutes);