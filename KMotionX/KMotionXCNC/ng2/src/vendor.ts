// Angular
import '@angular/platform-browser';
import '@angular/platform-browser-dynamic';
import '@angular/core';
import '@angular/common';
import '@angular/forms';
import '@angular/http';
import '@angular/router';
// RxJS
import 'rxjs';
// Other vendors for example jQuery, Lodash or Bootstrap
// You can import js, ts, css, sass, ...
//import 'opentype.js'
import * as opentype from 'opentype.js'
import 'pdfjs-dist/build/pdf.js'

import 'bootstrap/dist/css/bootstrap.min.css'
import 'bootstrap/dist/css/bootstrap-theme.min.css'
import '../public/css/style.css'

//THREE is modular but not the TrackballControls example
require('script-loader!three/build/three.js');
require('script-loader!three/examples/js/controls/TrackballControls.js');
//Modular ace implementation
import 'brace';
import 'brace/mode/c_cpp';
import 'brace/mode/gcode';
import 'brace/theme/chrome';
import 'brace/ext/settings_menu';