//We cannot have the current machine undefined
//currentMachine will be updated when file is loaded
var M_Action_None = 0; // do nothing
var M_Action_Setbit = 1; // Set a bit high or low
var M_Action_SetTwoBits = 2; // Set two bits either high or low
var M_Action_DAC = 3; // output a value to a DAC
var M_Action_Program = 4; // run a KMotion User C program
var M_Action_Program_wait = 5; // run a KMotion User C program wait til finished
var M_Action_Program_wait_sync = 6; // run a KMotion User C program wait til finished, resync positions
var M_Action_Program_PC = 7; // run a Windows program wait til finished
var M_Action_Callback = 8; // Callback to the User Application
var M_Action_Waitbit = 9; // Wait/Hold until a bit is high or low  //Not appicable for USer button
var axisNames = ['X', 'Y', 'Z', 'A', 'B', 'C'];

function Machine() {
    this.clear();
    /*
    this.actionTemplate = {
        "type": "M(1-9)|M(100-119)|U(ser)|S",
        "name": "Home|Init",
        "value": 7,
        "action": M_Action_Setbit,
        "prog": {
            "thread": 3,
            "var": 4,
            "file": "m100.c"
        },
        "progPC": "cmd",
        "bit1": {
            "nr": 4,
            "value": 1
        },
        "bit2": {
            "nr": 4,
            "value": 1
        },
        "wbit": {
            "nr": 4,
            "value": 1
        }
    }
            */

}


Machine.prototype.copy = function(data) {
    this.name = data.name;
    this.description = data.description;
    this.dimX = data.dimX;
    this.dimY = data.dimY;
    this.dimZ = data.dimZ;
    this.initProgram = data.initProgram;
    this.initThread = data.initThread;
    this.axes = data.axes || [];
    this.tplanner = data.tplanner || {};
    this.actions = data.actions || [];
}
Machine.prototype.clear = function() {
    this.name = null;
    this.description = null;
    this.dimX = 0;
    this.dimY = 0;
    this.dimZ = 0;
    this.initProgram = null;
    this.initThread = 1;
    this.axes = [];
	this.tplanner = {};
    this.actions = [];
}

Machine.prototype.edit = function() {

    $("input[name='machine_name']").val(this.name);
    $("input[name='machine_description']").val(this.description);
    $("input[name='dimx']").val(this.dimX);
    $("input[name='dimy']").val(this.dimY);
    $("input[name='dimz']").val(this.dimZ);
    $("input[name='bootstrap']").val(this.initProgram);
    $("input[name='bootstrap_thread']").val(this.initThread);
    var that = this;
    $.each(axisNames, function(index, name) {
        var axis = that.axes[index] || {};
        $("#axiscounts" + name).val(axis.countsPerUnit);
        $("#axisaccel" + name).val(axis.maxAccel);
        $("#axisvel" + name).val(axis.maxVel);

    });
    $("#tplannerbreakangle").val(this.tplanner.breakangle);
    $("#tplannerlookahead").val(this.tplanner.lookahead);
    $("#tplannercollineartolerance").val(this.tplanner.collineartolerance);
    $("#tplannercornertolerance").val(this.tplanner.cornertolerance);
    $("#tplannerfacetangle").val(this.tplanner.facetangle);
}

Machine.prototype.store = function() {
    this.name = $("input[name='machine_name']").val();
    this.description = $("input[name='machine_description']").val();
    this.dimX = $("input[name='dimx']").val();
    this.dimY = $("input[name='dimy']").val();
    this.dimZ = $("input[name='dimz']").val();
    this.initProgram = $("input[name='bootstrap']").val();
    this.initThread = parseInt($("input[name='bootstrap_thread']").val());
    this.axes = [];
    var that = this;
    $.each(axisNames, function(index, name) {
        var axis = {};
        axis.name = name;
        axis.countsPerUnit = parseFloat($("#axiscounts" + name).val());
        axis.maxAccel = parseFloat($("#axisaccel" + name).val());
        axis.maxVel = parseFloat($("#axisvel" + name).val());

        that.axes.push(axis);
    });
    this.tplanner.breakangle = $("#tplannerbreakangle").val();
    this.tplanner.lookahead = $("#tplannerlookahead").val();
    this.tplanner.collineartolerance = $("#tplannercollineartolerance").val();
    this.tplanner.cornertolerance = $("#tplannercornertolerance").val();
    this.tplanner.facetangle = $("#tplannerfacetangle").val();
}
