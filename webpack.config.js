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
        library: {
            name: 'm',
            type: 'var',
        },
    },
    devtool: "source-map",
    resolve: {
        extensions: ['.js', '.ts'],
        fallback: {
            "path": false,
            "fs": false
        }
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
    target: 'web'
};

