function interpretFile() {
    var intOp = clone(interpreter.interpret);
    intOp.params.file.value = $("#code_apply").data("file");
    doCall(intOp);
}

function loadFile(file) {
    var op = clone(aux.loadFile);
    //File might be loaded twice if event of loaded file comes back before
    //file is loaded.
    //TODO failure to load file should be handled in somehow
	var currentFile = $("#code_apply").data("file");
	
	$("#code_apply").data("file", file);
    doParamCall(op, file, function(data) {
        $("#code_text").html(data);
        var c = document.getElementById('code_text');
    	lineTA.init(c);
        $("#code_text").show();
        $("#file_list").hide();
    });

}

function listFiles() {
    var dir = "gcode/";
    $("#file_list").load(dir + "  tr > td > a", function() {
        $("#code_text").hide();
        $("#file_list").show();
        $("#file_list > a").replaceWith(function() {
            return "<div><button class='button file_button' onclick='loadFile(\"" + dir + "\" + this.innerText)'>" + this.innerText + "</button></div>";
        });
    });
}

function doCall(operation) {


    var params = [];
    var obj = operation.params;
    if (obj !== null) {

        // Visit non-inherited enumerable keys
        Object.keys(obj).forEach(function(key) {
            //console.log(key, obj[key]);
            params.push(obj[key].value);
        });
    }
    doParamCall(operation, params);
}

function doParamCall(operation, params, datafunc) {

    var jsonCall = new rcall(operation.api, operation.func, params, null);
    var posting = $.post(apiUrl(jsonCall), JSON.stringify(jsonCall), datafunc);

}
function apiUrl(remoteCall){
	return "/api/" + remoteCall.clazz +"/"+remoteCall.func;
}

function initAPI() {

}

var rcall = function(clazz, func, params, returnValue) {
    this.clazz = clazz;
    this.func = func;
    this.params = params;
    this.ret = returnValue;
}



var aux = {
    "saveMachine": {
        "api": "aux",
        "func": "saveMachine",
        "params": null
    },
    "loadFile": {
        "api": "aux",
        "func": "loadFile",
        "params": null
    }
};

var kmotion = {
    "feedHold": {
        "api": "kmotion",
        "func": "feedHold",
        "params": null
    },
    "convertToOut": {
        "api": "kmotion",
        "func": "ConvertToOut",
        "params": {
            "thread": {
                "type": "number",
                "min": 1,
                "max": 7,
                "value": 4
            },
            "file": {
                "type": "text",
                "min": 1,
                "max": 1,
                "value": "c/fg/bulle.c"
            },
            "outFile": {
                "type": "text",
                "min": 0,
                "max": 0,
                "value": null
            },
            "maxLength": {
                "type": "number",
                "min": 10,
                "max": 256,
                "value": 256
            }
        }
    },
    "doErrMsg": {
        "api": "kmotion",
        "func": "DoErrMsg",
        "params": {
            "message": {
                "type": "text",
                "min": 1,
                "max": 1,
                "value": "Ett felmeddelande."
            }
        }
    }
};

var interpreter = {
    "interpret": {
        "api": "interpreter",
        "func": "Interpret",
        "params": {
            "boardType": {
                "type": "number",
                "min": 0,
                "max": 2,
                "value": 2
            },
            "file": {
                "type": "text",
                "min": 1,
                "max": 1,
                "value": null
            },
            "start": {
                "type": "number",
                "min": 0,
                "max": 100000000,
                "value": 0
            },
            "end": {
                "type": "number",
                "min": -1,
                "max": 100000000,
                "value": -1
            },
            "restart": {
                "type": "number",
                "min": 0,
                "max": 1,
                "value": 1
            },
            "statusCallback": {
                "type": "text",
                "min": 0,
                "max": 0,
                "value": null
            },
            "completeCallback": {
                "type": "text",
                "min": 0,
                "max": 0,
                "value": null
            }
        }
    },
    "simulate": {
        "api": "interpreter",
        "func": "simulate",
        "params": null
    }
};