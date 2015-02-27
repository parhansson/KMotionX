// Copyright (c) 2014 par.hansson@gmail.com

//TODO deleteme
var machine = {};

function clone(obj) {
    return JSON.parse(JSON.stringify(obj));
}

function error(msg) {
  alert(msg);
}

function about() {
  $('#aboutModal').modal();
}

$(function() {
	
    
    $("#about-btn").click(function() {
      $('#aboutModal').modal();
    });
    
    $("#feed_hold_btn").click(function() {
        RPC.feedHold.invoke();
    });    

    $("#status_log_btn").click(function() {
        $("#status").empty();
    });
    $("#error_log_btn").click(function() {
        $("#error").empty();
    });
    $("#console_log_btn").click(function() {
        $("#console").empty();
    });

    $("#raw_log_btn").click(function() {
        $("#output").empty();
    });

    $("#init_machine_btn").click(function() {
        RPC.compileAndLoadCoff.invoke([machine.initProgram, machine.initThread]);
    });
    
    $("#edit_machine_btn").click(function() {
        $('#settingsModal').modal();
    });

  if (!Modernizr.webgl) {
    alert('Sorry, you need a WebGL capable browser to use this.\n\nGet the latest Chrome or FireFox.');
    return;
  }

  if (!Modernizr.localstorage) {
    alert("Man, your browser is ancient. I can't work with this. Please upgrade.");
    return;
  }

  // Show 'About' dialog for first time visits.
  if (!localStorage.getItem("not-first-visit")) {
    localStorage.setItem("not-first-visit", true);
    setTimeout(about, 500);
  }

});
