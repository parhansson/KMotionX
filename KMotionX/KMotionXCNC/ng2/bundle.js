var SystemBuilder = require('systemjs-builder');
var argv = require('yargs').argv;
var builder = new SystemBuilder();

builder.loadConfig('./systemjs.config.js')
  .then(function(){
	  var outputFile = argv.prod ? 'dist/kmxcnc.min.js' : 'dist/kmxcnc.js';
	  return builder.bundle //builder.buildStatic 
    ('app', outputFile, {
      encodeNames:false,
      //sourceMaps: true,
		  minify: argv.prod,
		  mangle: argv.prod,
		  rollup: argv.prod
	  });
  })
  .then(function(){
	  console.log('bundle built successfully!');
  });