var webpackMerge = require('webpack-merge');
var ExtractTextPlugin = require('extract-text-webpack-plugin');
var prodConfig = require('./webpack.prod.js');
var helpers = require('./helpers');
var BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin;

module.exports = webpackMerge(prodConfig, {
  plugins: [
    new BundleAnalyzerPlugin()
  ]
});
