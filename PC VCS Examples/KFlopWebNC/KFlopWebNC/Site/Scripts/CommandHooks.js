
$(document).ready(function() {

    //Axis Creation
 
    var t = setInterval(function() {

        if (window.external.IsEnabledX == true) {
                $("#enablexLED").switchClass("GreenoffLED", "GreenonLED", 500);
            }
            else {
                $("#enablexLED").switchClass("GreenonLED", "GreenoffLED", 500);
            }
        if (window.external.IsEnabledY == true) {
                $("#enableyLED").switchClass("GreenoffLED", "GreenonLED", 500);
            }
            else {
                $("#enableyLED").switchClass("GreenonLED", "GreenoffLED", 500);
            }
        if (window.external.IsEnabledZ == true) {
                $("#enablezLED").switchClass("GreenoffLED", "GreenonLED", 500);
            }
            else {
                $("#enablezLED").switchClass("GreenonLED", "GreenoffLED", 500);
            }
    }, 1000);

    //Axis Enabling
    $('#xenable').click(function() {
        window.external.ToggleEnableX();
        if (window.external.IsEnabledX == true) {
            $("#enablexLED").switchClass("GreenoffLED", "GreenonLED", 500);
        }
        else {
            $("#enablexLED").switchClass("GreenonLED", "GreenoffLED", 500);
        }
    });
    $('#yenable').click(function() {
        window.external.ToggleEnableY();
        if (window.external.IsEnabledY == true) {
            $("#enableyLED").switchClass("GreenoffLED", "GreenonLED", 500);
        }
        else {
            $("#enableyLED").switchClass("GreenonLED", "GreenoffLED", 500);
        }
    });
    $('#zenable').click(function() {
        window.external.ToggleEnableZ();
        if (window.external.IsEnabledZ == true) {
            $("#enablezLED").switchClass("GreenoffLED", "GreenonLED", 500);
        }
        else {
            $("#enablezLED").switchClass("GreenonLED", "GreenoffLED", 500);
        }
    });
    $('#aenable').click(function() {
        window.external.ToggleEnableA();
        if (window.external.IsEnabledA == true) {
            $("#enableaLED").switchClass("GreenoffLED", "GreenonLED", 500);
        }
        else {
            $("#enableaLED").switchClass("GreenonLED", "GreenoffLED", 500);
        }
    });
    $('#benable').click(function() {
        window.external.ToggleEnableB();
        if (window.external.IsEnabledB == true) {
            $("#enablebLED").switchClass("GreenoffLED", "GreenonLED", 500);
        }
        else {
            $("#enablebLED").switchClass("GreenonLED", "GreenoffLED", 500);
        }
    });
    $('#cenable').click(function() {
        window.external.ToggleEnableC();
        if (window.external.IsEnabledC == true) {
            $("#enablecLED").switchClass("GreenoffLED", "GreenonLED", 500);
        }
        else {
            $("#enablecLED").switchClass("GreenonLED", "GreenoffLED", 500);
        }
    });

    //Axis Zeroing    
    $('#xzero').click(function() {
        window.external.ZeroX;
    });
    $('#yzero').click(function() {
        window.external.ZeroY;
    });
    $('#zzero').click(function() {
        window.external.ZeroZ;
    });
    $('#azero').click(function() {
        window.external.ZeroA;
    });
    $('#bzero').click(function() {
        window.external.ZeroB;
    });
    $('#czero').click(function() {
        window.external.ZeroC;
    });

    //Axis Negative Jogging
    $('#xjogneg').mousedown(function() {
        window.external.JogXNegative;
    });
    $('#yjogneg').mousedown(function() {
        window.external.JogYNegative;
    });
    $('#zjogneg').mousedown(function() {
        window.external.JogZNegative;
    });
    $('#ajogneg').mousedown(function() {
        window.external.JogANegative;
    });
    $('#bjogneg').mousedown(function() {
        window.external.JogBNegative;
    });
    $('#cjogneg').mousedown(function() {
        window.external.JogCNegative;
    });
    //Stop Negative Jogging..
    //On Mouse Up
    $('#xjogneg').mouseup(function() {
        window.external.StopX;
    });
    $('#yjogneg').mouseup(function() {
        window.external.StopY;
    });
    $('#zjogneg').mouseup(function() {
        window.external.StopZ;
    });
    $('#ajogneg').mouseup(function() {
        window.external.StopA;
    });
    $('#bjogneg').mouseup(function() {
        window.external.StopB;
    });
    $('#cjogneg').mouseup(function() {
        window.external.StopC;
    });



    //Axis Positive Jogging
    $('#xjogpos').mousedown(function() {
        window.external.JogXPositive;
    });
    $('#yjogpos').mousedown(function() {
        window.external.JogYPositive;
    });
    $('#zjogpos').mousedown(function() {
        window.external.JogZPositive;
    });
    $('#ajogpos').mousedown(function() {
        window.external.JogAPositive;
    });
    $('#bjogpos').mousedown(function() {
        window.external.JogBPositive;
    });
    $('#cjogpos').mousedown(function() {
        window.external.JogCPositive;
    });
    //Stop Positive Jogging..
    //On Mouse Up
    $('#xjogpos').mouseup(function() {
        window.external.StopX;
    });
    $('#yjogpos').mouseup(function() {
        window.external.StopY;
    });
    $('#zjogpos').mouseup(function() {
        window.external.StopZ;
    });
    $('#ajogpos').mouseup(function() {
        window.external.StopA;
    });
    $('#bjogpos').mouseup(function() {
        window.external.StopB;
    });
    $('#cjogpos').mouseup(function() {
        window.external.StopC;
    });


    //Axis Jogging Sliders
    $("#xjogslider").bind("slide", function(event, ui) {
        window.external.SetJogPercentX(ui.value);
    });
    $("#yjogslider").bind("slide", function(event, ui) {
        window.external.SetJogPercentY(ui.value);
    });
    $("#zjogslider").bind("slide", function(event, ui) {
        window.external.SetJogPercentZ(ui.value);
    });
    $("#ajogslider").bind("slide", function(event, ui) {
        window.external.SetJogPercentA(ui.value);
    });
    $("#bjogslider").bind("slide", function(event, ui) {
        window.external.SetJogPercentB(ui.value);
    });
    $("#cjogslider").bind("slide", function(event, ui) {
        window.external.SetJogPercentC(ui.value);
    });

    //Axis Moving
    $('#xmoveto').click(function() {
        $("#xmove-form").dialog("open");
    });
    $('#ymoveto').click(function() {
        $("#ymove-form").dialog("open");
    });
    $('#zmoveto').click(function() {
        $("#zmove-form").dialog("open");
    });
    $('#amoveto').click(function() {
        $("#amove-form").dialog("open");
    });
    $('#bmoveto').click(function() {
        $("#bmove-form").dialog("open");
    });
    $('#cmoveto').click(function() {
        $("#cmove-form").dialog("open");
    });

















    //FRO
    $("#froslider").bind("slide", function(event, ui) {
        window.external.SetFeedRate(ui.value);
    });

 


    //Execute MDI Function
    $('#executemdi').click(function() {
        window.external.ExecuteMDI;
    });


    //Load G Code
    $('#loadgcode').click(function() {
        window.external.LoadGcodeFile;
    });
    //Rewind
    $('#rewindgcode').click(function() {
        window.external.RewindProgram;
    });
    //Run From Line
    $('#runfromline').click(function() {
        window.external.RunFromLine;
    });
    //Simulate
    $('#simulate').click(function() {
        window.external.ToggleSimulate;
        if (window.external.IsSimulationMode == true) {
            $("#simulateLED").switchClass("GreenoffLED", "GreenonLED", 300);
        }
        else {
            $("#simulateLED").switchClass("GreenonLED", "GreenoffLED", 300);
        }
    });
    //Block Delete
    $('#blockdelete').click(function() {
        window.external.ToggleBlockDelete;
        if (window.external.BlockDeleteActive == true) {
            $("#blkdeleteLED").switchClass("GreenoffLED", "GreenonLED", 200);
        }
        else {
            $("#blkdeleteLED").switchClass("GreenonLED", "GreenoffLED", 200);
        }
    });




    //Cycle Start
    $('#cyclestart').click(function() {
        window.external.CycleStart;
    });
    //Cycle Pause
    $('#cyclepause').click(function() {
        window.external.CyclePause;
        if (window.external.IsInterpreterPaused == true) {
            $("#cyclepauseLED").switchClass("RedoffLED", "RedonLED", 500);
        }
        else {
            $("#cyclepauseLED").switchClass("RedonLED", "RedoffLED", 500);
        }
    });
    //Stop
    $('#stop').click(function() {
        window.external.StopInterpreter;
    });



 




    //Main Page Nav
    $('#mainpagebutton').click(function() {
        window.external.NavigateToPage('index.html');
    });
    //Offsets Page Nav
    $('#offsetspagebutton').click(function() {
        window.external.NavigateToPage('offsets.html');
    });
    //Settings Page Nav
    $('#settingspagebutton').click(function() {
        window.external.NavigateToPage('settings.html');
    });
});