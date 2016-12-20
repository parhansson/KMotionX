export class KMXUtil {
  static createAnchor = {}


  static ab2str(buf: ArrayBuffer) {
    var arr = new Uint8Array(buf)
    var str = '';
    for (var i = 0, l = arr.length; i < l; i++)
      str += String.fromCharCode(arr[i]);
    return str;
    //Call stack too deep on certain browsers 
    //return String.fromCharCode.apply(null, new Uint8Array(buf)); //Uint16Array
    //Solution to this can be found in PDF.js
  }

  static str2ab(str: string) {
    var buf = new ArrayBuffer(str.length * 2); // 2 bytes for each char
    var bufView = new Uint16Array(buf);
    for (var i = 0, strLen = str.length; i < strLen; i++) {
      bufView[i] = str.charCodeAt(i);
    }
    return buf;
  }

  static injectScript(source, loadedCondition) {
    return new Promise(function (resolve, reject) {
      if (loadedCondition === true) {
        //TODO check if script tag is present instead of external loaded condition 
        resolve('Script already loaded:' + source);
      } else {
        var script = document.createElement('script');
        script.type = 'text/javascript';
        script.async = true;
        script.onload = function () {
          // remote script has loaded
          resolve('Script loaded:' + source);
        };
        script.onerror = function () { reject(Error('Load script failed: ' + source)); }
        script.src = source;
        document.getElementsByTagName('head')[0].appendChild(script);
      }


    });
  }

  static getSingletonWorker(workerScript, messageHandler) {
    return new Promise<Worker>(function (resolve, reject) {
      var worker = KMXUtil.workers[workerScript];
      if (worker === undefined) {
        try {
          worker = new Worker(workerScript);;
        } catch (error) {
          reject(Error(error));
        }
        KMXUtil.workers[workerScript] = worker;
      }
      //This needs to be set every time. Need to figure out why 
      worker.onmessage = messageHandler;
      resolve(worker);
    }.bind(this));

  }

  static workers = {}
  // Returns a function, that, as long as it continues to be invoked, will not
  // be triggered. The function will be called after it stops being called for
  // N milliseconds. If `immediate` is passed, trigger the function on the
  // leading edge, instead of the trailing.
  static debounce(func, wait, immediate) {
    var timeout;
    return function () {
      var context = this, args = arguments;
      var later = function () {
        timeout = null;
        if (!immediate) func.apply(context, args);
      };
      var callNow = immediate && !timeout;
      clearTimeout(timeout);
      timeout = setTimeout(later, wait);
      if (callNow) func.apply(context, args);
    };
  }
  static svgToString(svg: SVGElement): string {
    // need to add namespace declarations for this to be a valid xml document
    return (svg as any).outerHTML.replace('<svg:svg ', '<svg:svg xmlns:svg="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" ')
  }
}