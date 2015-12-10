/*!
 * angular-typecheck
 * Extra type checking methods for Angular.js
 * Copyright (c) 2015 Peter Bakondy https://github.com/pbakondy
 * See LICENSE in this repository for license information
 */
/* eslint quotes: [2, "single"]*/
/* global angular */

/**
 * Extra type checking methods for Angular.js
 *
 * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects
 * https://kangax.github.io/compat-table/es5/
 * https://kangax.github.io/compat-table/es6/
 */

(function () {

  'use strict';

  if (typeof angular !== 'object') {
    return;
  }

  var toString = Object.prototype.toString;

  if (typeof angular.isBoolean === 'undefined') {
    angular.isBoolean = function(value) {
      return typeof value === 'boolean';
    };
  }

  if (typeof angular.isRegExp === 'undefined') {
    angular.isRegExp = function(value) {
      return toString.call(value) === '[object RegExp]';
    };
  }

  // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/isNaN
  if (typeof angular.isNaN === 'undefined') {
    angular.isNaN = Number.isNaN || function(value) {
      return typeof value === 'number' && value !== value;
    };
  }

  if (typeof angular.isNull === 'undefined') {
    angular.isNull = function(value) {
      return value === null;
    };
  }

  // ES6 Types

  // https://github.com/angular/angular.js/blob/master/src/Angular.js#L635
  // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Typed_arrays
  if (typeof angular.isTypedArray === 'undefined') {
    var TYPED_ARRAY_REGEXP = /^\[object (Uint8(Clamped)?)|(Uint16)|(Uint32)|(Int8)|(Int16)|(Int32)|(Float(32)|(64))Array\]$/;
    angular.isTypedArray = function (value) {
      return TYPED_ARRAY_REGEXP.test(toString.call(value));
    };
  }

  // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
  if (typeof angular.isArrayBuffer === 'undefined') {
    angular.isArrayBuffer = function(value) {
      return toString.call(value) === '[object ArrayBuffer]';
    };
  }

  // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView
  if (typeof angular.isDataView === 'undefined') {
    angular.isDataView = function(value) {
      return toString.call(value) === '[object DataView]';
    };
  }

  if (typeof angular.isMap === 'undefined') {
    angular.isMap = function(value) {
      return toString.call(value) === '[object Map]';
    };
  }

  if (typeof angular.isSet === 'undefined') {
    angular.isSet = function(value) {
      return toString.call(value) === '[object Set]';
    };
  }

  if (typeof angular.isWeakMap === 'undefined') {
    angular.isWeakMap = function(value) {
      return toString.call(value) === '[object WeakMap]';
    };
  }

  if (typeof angular.isWeakSet === 'undefined') {
    angular.isWeakSet = function(value) {
      return toString.call(value) === '[object WeakSet]';
    };
  }

  if (typeof angular.isPromise === 'undefined') {
    angular.isPromise = function(value) {
      return toString.call(value) === '[object Promise]';
    };
  }

  if (typeof angular.isSymbol === 'undefined') {
    angular.isSymbol = function(value) {
      return typeof value === 'symbol';
    };
  }

  // custom detection
  // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/JSON
  if (typeof angular.isJSON === 'undefined') {
    angular.isJSON = function(value) {
      return angular.isNull(value) || angular.isBoolean(value) || angular.isNumber(value) ||
        angular.isString(value) || angular.isArray(value) || angular.isObject(value);
    };
  }


  // type detection
  if (typeof angular.detectType === 'undefined') {
    angular.detectType = function(value) {

      // primitives
      if (angular.isUndefined(value)) {
        return 'undefined';
      }
      if (angular.isNull(value)) {
        return 'null';
      }
      if (angular.isBoolean(value)) {
        return 'boolean';
      }
      if (angular.isNumber(value)) {
        return 'number';
      }
      if (angular.isString(value)) {
        return 'string';
      }
      if (angular.isSymbol(value)) {
        return 'symbol';
      }

      // structured data
      if (angular.isArrayBuffer(value)) {
        return 'arraybuffer';
      }
      if (angular.isDataView(value)) {
        return 'dataview';
      }

      // custom objects
      if (angular.isTypedArray(value)) {
        return 'typedarray';
      }
      if (angular.isArray(value)) {
        return 'array';
      }
      if (angular.isRegExp(value)) {
        return 'regexp';
      }
      if (angular.isDate(value)) {
        return 'date';
      }
      if (angular.isFunction(value)) {
        return 'function';
      }

      // keyed collections
      if (angular.isMap(value)) {
        return 'map';
      }
      if (angular.isSet(value)) {
        return 'set';
      }
      if (angular.isWeakMap(value)) {
        return 'weakmap';
      }
      if (angular.isWeakSet(value)) {
        return 'weakset';
      }

      // promise
      if (angular.isPromise(value)) {
        return 'promise';
      }

      // general object
      if (angular.isObject(value)) {
        return 'object';
      }

    };
  }

}());