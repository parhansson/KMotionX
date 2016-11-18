var chokidar = require('chokidar');
var fs = require('fs');

var watcher = chokidar.watch('./app', {
  //ignored: /^\./, 
  ignored: '**/*.ts',
  persistent: true,
  });

watcher
  .on('add', function(path) {
    copy(path);
    console.log('sync> File', path, 'has been added');
    })
  .on('change', function(path) {
    copy(path);
    console.log('sync> File', path, 'has been changed');})
  .on('unlink', function(path) {console.log('sync> File', path, 'has been removed');})
  .on('error', function(error) {console.error('sync> Error happened', error);})

function copy(source){
  copyFile(source,"dist/"+source,function(value){
    if(value){
      console.log('sync> File', value);
    }
  })
}  
  
function copyFile(source, target, cb) {

  var rd = fs.createReadStream(source);
  rd.on("error", function(err) {
    done("Read: " + err);
  });
  var wr = fs.createWriteStream(target);
  wr.on("open", function(ex) {
    rd.pipe(wr);
  });
  wr.on("error", function(err) {
    done("Write: " +err);
  });
  wr.on("close", function(ex) {
    done();
  });  

  function done(err) {
    if(cb){
      cb(err);
    }
  }
}