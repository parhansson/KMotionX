$(document).ready(function() {

    $(function() {
    $("#settingstabs").tabs({ collapsible: false });
    });

    //    $(function() {
    //        $("input:submit, a, button").button();
    //    });
    $(function() {
        $(".jogslider").slider({
            value: 100,
            min: 10,
            max: 100,
            step: 10
        })
    });

    $(function() {
        $("#froslider").slider({
            orientation: "vertical",
            value: 100,
            min: 10,
            max: 200,
            step: 10
        })
    });
    
    //X Move To
    $("#xmove-form").dialog({
        autoOpen: false,
        height: 200,
        width: 340,
        modal: true,
        buttons: {
        "Move To": function() {
            var position = $("#xposition")
            window.external.MoveX(position.val());
            $(this).dialog("close");
            },
            Cancel: function() {
                $(this).dialog("close");
            }
        }
    });

    //Y Move To
    $("#ymove-form").dialog({
        autoOpen: false,
        height: 200,
        width: 340,
        modal: true,
        buttons: {
            "Move To": function() {
                var position = $("#yposition")
                window.external.MoveY(position.val());
                $(this).dialog("close");
            },
            Cancel: function() {
                $(this).dialog("close");
            }
        }
    });

    //Z Move To
    $("#zmove-form").dialog({
        autoOpen: false,
        height: 200,
        width: 340,
        modal: true,
        buttons: {
            "Move To": function() {
                var position = $("#zposition")
                window.external.MoveZ(position.val());
                $(this).dialog("close");
            },
            Cancel: function() {
                $(this).dialog("close");
            }
        }
    });

    //A Move To
    $("#amove-form").dialog({
        autoOpen: false,
        height: 200,
        width: 340,
        modal: true,
        buttons: {
            "Move To": function() {
                var position = $("#aposition")
                window.external.MoveA(position.val());
                $(this).dialog("close");
            },
            Cancel: function() {
                $(this).dialog("close");
            }
        }
    });

    //B Move To
    $("#bmove-form").dialog({
        autoOpen: false,
        height: 200,
        width: 340,
        modal: true,
        buttons: {
            "Move To": function() {
                var position = $("#bposition")
                window.external.MoveB(position.val());
                $(this).dialog("close");
            },
            Cancel: function() {
                $(this).dialog("close");
            }
        }
    });

    //C Move To
    $("#cmove-form").dialog({
        autoOpen: false,
        height: 200,
        width: 340,
        modal: true,
        buttons: {
            "Move To": function() {
                var position = $("#cposition")
                window.external.MoveC(position.val());
                $(this).dialog("close");
            },
            Cancel: function() {
                $(this).dialog("close");
            }
        }
    });
    
});