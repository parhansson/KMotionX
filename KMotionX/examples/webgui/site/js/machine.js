//We cannot have the current machine undefined
//currentMachine will be updated when file is loaded
var currentMachine;

var machineTemplate = {
    "name": null,
    "description": null,
    "dimX": 0,
    "dimY": 0,
    "dimZ": 0,
    "init": {
        "program": null,
        "thread": 1
    },
    "actions": null
}

var actionTemplate = {
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

function initMachine() {
    $("#setup_area").hide();

    $("#setup_area").load("settings.html", function() {
        // Load was performed
        $(function() {
            $("#settingstabs").tabs({
                collapsible: false
            });
        });

        $("#save_machine_btn").click(function() {
            saveMachine(currentMachine);
        });
        $("#new_machine_btn").click(function() {
            currentMachine = clone(machineTemplate);
            editMachine(currentMachine);
        });
    });




    $("#edit_machine_btn").click(function() {
        editMachine(currentMachine);
    });



    $.getJSON("settings/machine.cnf")
        .done(function(data) {
            currentMachine = data;
        }).fail(function() {
            currentMachine = clone(machineTemplate);
        });

}

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


function editMachine(machine) {
    //$( "input[name='Hot Fuzz']" ).next().text( "Hot Fuzz" );
    $("input[name='machine_name']").val(machine.name);
    $("input[name='machine_description']").val(machine.description);
    $("input[name='dimx']").val(machine.dimX);
    $("input[name='dimy']").val(machine.dimY);
    $("input[name='dimz']").val(machine.dimZ);
    $("input[name='bootstrap']").val(machine.init.program);
    $("input[name='bootstrap_thread']").val(machine.init.thread);
    $("#setup_area").show();
}

function saveMachine(machine) {
    machine.name = $("input[name='machine_name']").val();
    machine.description = $("input[name='machine_description']").val();
    machine.dimX = $("input[name='dimx']").val();
    machine.dimY = $("input[name='dimy']").val();
    machine.dimZ = $("input[name='dimz']").val();
    machine.init.program = $("input[name='bootstrap']").val();
    machine.init.thread = $("input[name='bootstrap_thread']").val();

    doParamCall(aux.saveMachine, machine);
    $("#setup_area").hide();
}