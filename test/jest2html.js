
var fs = require('fs');
var path = require('path');
var process = require("process");
const ts = require("typescript");

var from_dir = "./jstests/";
var to_dir = "../docs/";

const include_list = ["general.test.js", "bitwise.test.js"];
const exclude_list = []

function read_file(from_path, to_path, data_handler)
{
    let program = ts.createProgram([from_path], {allowJs: true});
    const sourceFile = program.getSourceFile(from_path);
    const unfoundNodes = [], foundNodes = [];
    ts.forEachChild(sourceFile, node => {
        if(ts.isExpressionStatement(node)){
            if(ts.isCallExpression(node.expression) && node.expression.expression.expression.escapedText == 'mtest'){
                let test_name = node.expression.arguments[0].text;
                let test_control_name = test_name.replace(/[ .]/g, '_').toLowerCase();
                let test_result_control_name = test_control_name + '_result';
                let test_description = node.expression.arguments[1].text;
                let test_code = node.expression.arguments[2].text;
                let code_rows =  test_code.split("\n").length;
                const one_test_template = `
                <h5>${test_name}</h5>
                <div>${test_description}</div>
                <div style="margin-top: 10px;">
                    <textarea id="${test_control_name}"  rows = "${code_rows}" style="resize: none;">
                ${test_code}</textarea>
                </div>
                <div>
                    <div style="display: inline-block; margin-right:5px;"><button type="button" onclick="run('${test_control_name}')" style="min-width: 50px;">run</button></div>
                    <div style="display: inline-block; margin-left:5px;" id= "${test_result_control_name}"></div>
                </div>
                `;
                console.log(one_test_template);
            }
        }
    });
    /*
    fs.readFile(from_path, 'utf8', function(err, data){
        if(err) throw err;
        console.log('processing: ' + from_path);
        data_handler(data, to_path);
    });*/
}

function write_html(data, to_path)
{
    console.log('writing to: ' + to_path);
//     var pattern = String.raw`.*test\(\'(.*)\',[\s\S]*let code = \`([\s\S].)\`[\s\S]*`;
//     var matches = data.matchAll(pattern);
// //    console.log(pattern);
//     for(const match of matches){
//         console.log(match[1]);
//     }
    let tsSourceFile = ts.createSourceFile(__filename, data, ts.ScriptTarget.Latest);
    for(let statement of tsSourceFile.statements){
        console.log(statement);
    }
}

function main()
{
    fs.readdir(from_dir, function (err, files) {
        if(err){
            console.error("could not list the directory.", err);
            process.exit(1);
        }
        files.forEach(function(file, index){
            if (exclude_list.includes(file) || !include_list.includes(file)){
                return;
            }
            var from_path = path.join(from_dir, file);
            var to_path = path.join(to_dir, file.replace('.test.js', '-tut.html'));
            fs.stat(from_path, function(error, stat){
                if(error){
                    console.error("Error stating file.", error);
                    return;
                }
                if(stat.isFile()){
                    read_file(from_path, to_path, write_html);
                }
            });
        });
    });
}

main();