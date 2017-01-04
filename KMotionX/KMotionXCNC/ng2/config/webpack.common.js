var webpack = require('webpack');
var HtmlWebpackPlugin = require('html-webpack-plugin');
var ExtractTextPlugin = require('extract-text-webpack-plugin');
var helpers = require('./helpers');

module.exports = {
  entry: {
    'polyfills': './src/polyfills.ts',
    'vendor': './src/vendor.ts',
    'app': './src/main.ts',
    //'worker': './src/worker.ts'
  },

  resolve: {
    extensions: ['', '.js', '.ts']
  },

  module: {
    preLoaders: [
      {
        test: /\.ts$/,
        loader: 'tslint-loader'
      }
    ],
    loaders: [
      // {
      //   test: /\.ts$/,
      //   loaders: ['awesome-typescript-loader', 'angular2-template-loader']
      // },
      {
        test: /\.component.ts$/,
        loaders: ['ts-loader', 'angular2-template-loader']
      },
      {
        test: /\worker.ts$/,
        loaders: ['worker-loader','ts-loader']
      },      
      {
        test: /\.ts$/,
        exclude: /\.component.ts|\.worker.ts$/,
        loader: 'ts-loader'
      },      
      {
        test: /\.html$/,
        loader: 'html'
      },
      {
        test: /\.(png|jpe?g|gif|svg|woff|woff2|ttf|eot|ico)$/,
        loader: 'file?name=assets/[name].[hash].[ext]'
      },
      {
        test: /\.css$/,
        exclude: helpers.root('src', 'app'),
        loader: ExtractTextPlugin.extract('style', 'css?sourceMap')
      },
      {
        test: /\.css$/,
        include: helpers.root('src', 'app'),
        loader: 'raw'
      }
    ]
  },

  plugins: [
    new webpack.optimize.CommonsChunkPlugin({
      name: ['app', 'vendor', 'polyfills']
    }),

    new HtmlWebpackPlugin({
      template: 'src/index.html'
    }),
    new webpack.ProvidePlugin({
      "StringView": "vendor/mozilla/stringview.js"
    }) 
/*
    new webpack.ProvidePlugin({
      "THREE": "three"
    }) 
    */
  ],

  tslint: {
        configuration: {
            rules: {
                quotemark: [true, "single"]
            }
        },
 
        // tslint errors are displayed by default as warnings 
        // set emitErrors to true to display them as errors 
        emitErrors: false,
 
        // tslint does not interrupt the compilation by default 
        // if you want any file with tslint errors to fail 
        // set failOnHint to true 
        failOnHint: true//,       
 
        // name of your formatter (optional) 
        //formatter: "yourformatter",
 
        // path to directory containing formatter (optional) 
        //formattersDirectory: "node_modules/tslint-loader/formatters/",
        
        // These options are useful if you want to save output to files 
        // for your continuous integration server 
        //fileOutput: {
            // The directory where each file's report is saved 
            //dir: "./foo/",
 
            // The extension to use for each report's filename. Defaults to "txt" 
            //ext: "xml",
 
            // If true, all files are removed from the report directory at the beginning of run 
            //clean: true,
 
            // A string to include at the top of every report file. 
            // Useful for some report formats. 
            //header: "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<checkstyle version=\"5.7\">",
 
            // A string to include at the bottom of every report file. 
            // Useful for some report formats. 
            //footer: "</checkstyle>"
        //}
    }
};
