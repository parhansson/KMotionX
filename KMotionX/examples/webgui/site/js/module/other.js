/**
 * The normalizeArray_node function is subject to the following:
 * 
 * Copyright 2009, 2010 Ryan Lienhart Dahl. All rights reserved. Permission is
 * hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Source http://jsperf.com/normalize-path
 */
(function() {
  var module = angular.module('OtherStuff', []);

  module.factory('NormalizePath', function() {

    function normalizeArray_node(parts, keepBlanks) {
      var directories = [], prev;
      for (var i = 0, l = parts.length - 1; i <= l; i++) {
        var directory = parts[i];

        // if it's blank, but it's not the first thing, and not the last thing,
        // skip it.
        if (directory === "" && i !== 0 && i !== l && !keepBlanks)
          continue;

        // if it's a dot, and there was some previous dir already, then skip it.
        if (directory === "." && prev !== undefined)
          continue;

        // if it starts with "", and is a . or .., then skip it.
        if (directories.length === 1 && directories[0] === ""
            && (directory === "." || directory === ".."))
          continue;

        if (directory === ".." && directories.length && prev !== ".."
            && prev !== "." && prev !== undefined
            && (prev !== "" || keepBlanks)) {
          directories.pop();
          prev = directories.slice(-1)[0]
        } else {
          if (prev === ".")
            directories.pop();
          directories.push(directory);
          prev = directory;
        }
      }
      return directories;
    }

    return function(path, keepBlanks) {
      return normalizeArray_node(path.split("/"), keepBlanks).join("/");
    }
  });
})();