const path = require("path");
const { library } = require("webpack");
const bundleOutputDir = "./docs";

module.exports = {
    entry: {
        main: "./docs_src/main.ts"  
    },
    output: {
        filename: "[name].bundle.js",
        path: path.join(__dirname, bundleOutputDir),
        publicPath: 'public/dist/',
        library: {
            name: 'm',
            type: 'var',
        },
    },
    devtool: "source-map",
    resolve: {
        extensions: ['.js', '.ts']
    },
    module: {
        rules: [
            {
                test: /\.js$/,
                exclude: ['/node_modules/']
            },            
            { test: /\.tsx?$/, loader: "ts-loader" },        
            {
                test: /\.css$/,
                sideEffects: true,
                loader: "css-loader"
            }
        ]
    },
    target: 'node'
};

