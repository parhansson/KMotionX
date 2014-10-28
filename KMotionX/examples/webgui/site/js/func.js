function clone(obj) {
    return JSON.parse(JSON.stringify(obj));
}

function toggleButton(btn_sel, cond) {
    if (cond) {
        $(btn_sel).addClass("button_pressed");
    } else {
        $(btn_sel).removeClass("button_pressed");
    }
}
var lineTA = new lineTextArea();
function lineTextArea() {
	var that = this;
	this.tarea = null;
    this.init = function(tarea) {
        that.tarea = tarea;
        that.lines = tarea.value.split("\n");
        that.linePos = [];

        // calculate start/end
        var startPos = 0;
        var endPos = tarea.value.length;
        var lineLength;
        for (var x = 0; x < that.lines.length; x++) {
            lineLength = that.lines[x].length;
            endPos = startPos + lineLength;
            that.linePos[x] = [startPos, endPos];            
            
            startPos += (lineLength + 1);
        }

    }

    this.toLine = function(lineNum) {
		if(that.tarea == null){
			return;
		}
        var pos = that.linePos[lineNum];
        var startPos = pos[0];
        var endPos = pos[1];

        // do selection
        // Chrome / Firefox

        if (typeof(that.tarea.selectionStart) != "undefined") {
            that.tarea.focus();
            that.tarea.selectionStart = startPos;
            that.tarea.selectionEnd = endPos;

            // we need to scroll to this row but scrolls are in pixels,
            // so we need to know a row's height, in pixels
            var lineHeight = that.tarea.clientHeight / 21; //tarea.rows;

            // scroll !!
            that.tarea.scrollTop = lineHeight * lineNum;

            return true;
        }

        // IE
        if (document.selection && document.selection.createRange) {
            that.tarea.focus();
            that.tarea.select();
            var range = document.selection.createRange();
            range.collapse(true);
            range.moveEnd("character", endPos);
            range.moveStart("character", startPos);
            range.select();
            return true;
        }

        return false;

    };
}

