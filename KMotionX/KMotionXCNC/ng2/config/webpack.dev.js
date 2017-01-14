var webpackMerge = require('webpack-merge');
var ExtractTextPlugin = require('extract-text-webpack-plugin');
var commonConfig = require('./webpack.common.js');
var helpers = require('./helpers');

module.exports = webpackMerge(commonConfig, {
  devtool: 'cheap-module-eval-source-map',

  output: {
    path: helpers.root('dist'),
    pathinfo:true,
    publicPath: "/ng2/dist/",
    filename: '[name].js',
    chunkFilename: '[id].chunk.js'
  },

  plugins: [
    new ExtractTextPlugin('[name].css')
  ],

  devServer: {
    historyApiFallback: true,
    stats: 'minimal',
    proxy: [
      {
        context: ['/ws'],
        ws:true,
        target: 'ws://localhost:8080',
        secure: false
      },      
      {
        context: ['/api/**', '/settings/**'],
        target: 'http://localhost:8080',
        secure: false
      }
    ]
  }
});