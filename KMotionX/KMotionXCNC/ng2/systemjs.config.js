/**
 * System configuration for Angular 2 samples
 * Adjust as necessary for your application needs.
 * Override at the last minute with global.filterSystemConfig (as plunkers do)
 */
(function (global) {

  // map tells the System loader where to look for things
  var map = {
    '@angular': 'node_modules/@angular',
    'app': 'dist', // map app to dist folder

    //loading three from modules
    'three': 'node_modules/three/build',
    'three-trackballcontrols': 'node_modules/three/examples/js/controls',

    // removed when loading from bundle
    'rxjs': 'node_modules/rxjs',
    'ng2-bootstrap': 'node_modules/ng2-bootstrap',

    //needed by ng2-bootstrap datepicker
    'moment': 'node_modules/moment/moment.js',
  };
  var meta = {
    // meaning [baseURL]/vendor/angular.js when no other rules are present
    // path is normalized using map and paths configuration

    'node_modules/three/examples/js/controls/TrackballControls.js': {
      format: 'global', //format: 'cjs', // load this module as a CommonJS
      deps: ['three'] // dependencies to load before this module
    },
    'node_modules/three/build/three.js': {
      format: 'global', // load this module as a global
      //exports: 'THREE', // the global property to take as the module value
    }
  }
  // packages tells the System loader how to load when no filename and/or no extension
  var packages = {
    "app": { main: 'app/main.js', defaultExtension: 'js' }, //format: 'register', 

    'three': { main: 'three.js', defaultExtension: 'js' },
    'three-trackballcontrols': { main: 'TrackballControls.js', defaultExtension: 'js' },
    // removed when loading from bundle
    'ng2-bootstrap': { defaultExtension: 'js' }, //"ng2-bootstrap": { format: 'register', defaultExtension: 'js' },
    'rxjs': { defaultExtension: 'js' },
  };

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
    'dist/app/ccode',
    'dist/app/debug',
    'dist/app/editor',
    'dist/app/gcode',
    'dist/app/laser',
    'dist/app/log',
    'dist/app/model/gcode',
    'dist/app/model/transformer',
    'dist/app/resources',
    'dist/app/settings',
  ];

  // add package entries for angular packages in the form '@angular/common': { main: 'index.js', defaultExtension: 'js' }
  packageNames.forEach(function (pkgName) {
    packages[pkgName] = { main: 'index.js', defaultExtension: 'js' };
  });

  var config = {
    map: map,
    packages: packages,
    meta: meta
  }

  // filterSystemConfig - index.html's chance to modify config before we register it.
  if (global.filterSystemConfig) { global.filterSystemConfig(config); }

  System.config(config);

})(this);