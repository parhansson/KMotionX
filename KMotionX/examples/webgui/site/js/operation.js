
function RemoteCall(clazz, func, params) {
    this.clazz = clazz;
    this.func = func;
    this.params = params;
    this.ret = null;

}
RemoteCall.prototype.url = function(){
	return "/api/" + this.clazz +"/"+this.func;
}

RemoteCall.prototype.invoke = function(params, datafunc){
	if(params){
		this.params = params || [];
	}
	$.post(this.url(), JSON.stringify(this), datafunc);
}

var RPC = {
	saveMachine: new RemoteCall('aux','saveMachine'),
	listDir: new RemoteCall('aux','listDir'),
	loadGCodeFile: new RemoteCall('aux','loadGCodeFile'),
	
	feedHold: new RemoteCall('kmotion','feedHold'),
	doErrMsg: new RemoteCall('kmotion','DoErrMsg',['felet']),
	compileAndLoadCoff: new RemoteCall('kmotion','CompileAndLoadCoff'),
	
	setMotionParams: new RemoteCall('interpreter','setMotionParams'),
	simulate: new RemoteCall('interpreter','simulate'),
	interpret: new RemoteCall('interpreter','Interpret'),
}
