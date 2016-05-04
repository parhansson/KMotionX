/**
 * System configuration for Angular 2 samples
 * Adjust as necessary for your application needs.
 * Override at the last minute with global.filterSystemConfig (as plunkers do)
 */
(function (global) {

  // map tells the System loader where to look for things
  var map = {
    //'app': 'app', // 'dist',
    'rxjs': 'node_modules/rxjs',
    '@angular': 'node_modules/@angular',
    //moment: 'node_modules/moment/moment.js', //needed by ng2-bootstrap datepicker
  };

  // packages tells the System loader how to load when no filename and/or no extension
  var packages = {
    "dist": { format: 'register', defaultExtension: 'js' },
    //'app':                        { main: 'main.js',  defaultExtension: 'js' },
    'rxjs': { defaultExtension: 'js' },
  };

        /*
        ,
        meta: {
          'node_modules/three-trackballcontrols/index.js': {
            format: 'cjs',
            deps: ['./node_modules/three/three.js']
          }
        }
        */

  var packageNames = [
    '@angular/common',
    '@angular/compiler',
    '@angular/core',
    '@angular/http',
    '@angular/platform-browser',
    '@angular/platform-browser-dynamic',
    '@angular/router-deprecated',
    //'@angular/testing',
    //'@angular/upgrade',
  ];

  // add package entries for angular packages in the form '@angular/common': { main: 'index.js', defaultExtension: 'js' }
  packageNames.forEach(function (pkgName) {
    packages[pkgName] = { main: 'index.js', defaultExtension: 'js' };
  });

  var config = {
    map: map,
    packages: packages
  }

  // filterSystemConfig - index.html's chance to modify config before we register it.
  if (global.filterSystemConfig) { global.filterSystemConfig(config); }

  System.config(config);

})(this);