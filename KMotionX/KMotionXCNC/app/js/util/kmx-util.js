///<reference path="../../typings/KMX.d.ts" />
var KMX = KMX || {};
KMX.Util = {
	createAnchor: {},
	
	
	ab2str: function ab2str(buf) {
		var arr = new Uint8Array(buf)
		var str = "";
		for(var i=0,l=arr.length; i<l; i++)
			str += String.fromCharCode(arr[i]);
		return str;
		//Call stack too deep on certain browsers 
		//return String.fromCharCode.apply(null, new Uint8Array(buf)); //Uint16Array
		//Solution to this can be found in PDF.js
  	},
  	str2ab: function str2ab(str) {
		var buf = new ArrayBuffer(str.length*2); // 2 bytes for each char
		var bufView = new Uint16Array(buf);
		for (var i=0, strLen=str.length; i < strLen; i++) {
		bufView[i] = str.charCodeAt(i);
		}
		return buf;
  	},
	injectScript: function injectScript(source){
		return new Promise(function(resolve, reject) {
				
              var script = document.createElement('script');
              script.type = 'text/javascript';
              script.async = true;
              script.onload = function(){
                  // remote script has loaded
                  resolve("GCodeParser script loaded:" + source);
              };
			  script.onerror = function() { reject(Error("Load script failed: " + source)); }
              script.src = source;
              document.getElementsByTagName('head')[0].appendChild(script);


		});
	},
	getSingletonWorker: function getSingletonWorker(workerScript, messageHandler){
		return new Promise(function(resolve, reject) {
			var worker = this.workers[workerScript];
			if(worker === undefined){
				try {
					worker = new Worker(workerScript);;
				} catch (error) {
					reject(Error(error));
				}
				this.workers[workerScript] = worker;
			}
			//This needs to be set every time. Need to figure out why 
			worker.onmessage = messageHandler;
			resolve(worker);
		}.bind(this)); 
		
	},
	workers :{}
	
}