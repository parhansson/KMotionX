var webpack = require('webpack');
var HtmlWebpackPlugin = require('html-webpack-plugin');
var ExtractTextPlugin = require('extract-text-webpack-plugin');
var helpers = require('./helpers');

var tslintOptions = {
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
};

module.exports = {
  entry: {
    'polyfills': './src/polyfills.ts',
    'vendor': './src/vendor.ts',
    'app': './src/main.ts'
  },

  resolve: {
    //extensions: ['', '.js', '.ts']
    //extensions: ['*', '.js', '.ts']
    extensions: ['.js', '.ts']
  },

  module: {
    rules: [
      {
        test: /\.ts$/,
        enforce: "pre",
        loader: 'tslint-loader'
      },
      {
        test: /\.component.ts$/,
        use: [
          'ts-loader',
          'angular2-template-loader'
        ]
      },
      {
        test: /\.worker.ts$/,
        use: [
          'worker-loader',
          'ts-loader'
        ]
      },
      {
        test: /\.ts$/,
        exclude: /\.component.ts|\.worker.ts$/,
        loader: 'ts-loader'
      },
      {
        test: /\.html$/,
        loader: 'html-loader'
      },
      {
        test: /pdf.worker.js$/,
        loader: 'file-loader',
        options: { name: 'assets/[name].[ext]' }
      },
      {
        test: /\.(png|jpe?g|gif|svg|woff|woff2|ttf|eot|ico)$/,
        loader: 'file-loader',
        options: { name: 'assets/[name].[hash].[ext]' }
      },
      {
        test: /\.css$/,
        exclude: helpers.root('src', 'app'),
        //use: ExtractTextPlugin.extract({ fallbackLoader: 'style-loader', loader: 'css?sourceMap'})
        loader: ExtractTextPlugin.extract({ fallbackLoader: 'style-loader', loader: 'css-loader' })
      },
      {
        test: /\.css$/,
        include: helpers.root('src', 'app'),
        loader: 'raw-loader'
      }
    ]
  },

  plugins: [
    new webpack.optimize.CommonsChunkPlugin({
      names: ['app', 'vendor', 'polyfills', 'manifest']
    }),

    new HtmlWebpackPlugin({
      template: 'src/index.html'
    }),
    /*
        new webpack.ProvidePlugin({
          "StringView": "vendor/mozilla/stringview.js"
        }) 
        new webpack.ProvidePlugin({
          "THREE": "three"
        }) 
        */

    new webpack.LoaderOptionsPlugin({
      options: {
        tslint: tslintOptions
      }
    }),

    // Fixes Angular 2 error
    new webpack.ContextReplacementPlugin(
      /angular(\\|\/)core(\\|\/)(esm(\\|\/)src|src)(\\|\/)linker/,
      __dirname
    )
  ]

};


