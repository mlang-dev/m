
var fs = require('fs');
var path = require('path');
var process = require("process");
const ts = require("typescript");

var from_dir = "./jstests/";
var to_dir = "../docs/";

const include_list = new Map();
include_list.set("general.test.js", 1); 
include_list.set("bitwise.test.js", 2);
include_list.set("logic.test.js", 3);
include_list.set("relation.test.js", 4);
include_list.set("numeric.test.js", 5);


const exclude_list = []

const html_header_file = "./tutorial.html_header_template";
const html_footer_file = "./tutorial.html_footer_template";

function generate_file(from_path, test_navigations, to_path)
{
    let program = ts.createProgram([from_path], {allowJs: true});
    const sourceFile = program.getSourceFile(from_path);
    var test_cases = '';
    ts.forEachChild(sourceFile, node => {
        if(ts.isExpressionStatement(node)){
            if(ts.isCallExpression(node.expression) && node.expression.expression.expression.escapedText == 'mtest'){
                tutorial = node.expression.arguments[4];
                if(tutorial!=undefined && tutorial.kind == ts.SyntaxKind.FalseKeyword) 
                    return;
                let test_name = node.expression.arguments[0].text;
                let test_control_name = test_name.replace(/[ .,]/g, '_').toLowerCase();
                let test_result_control_name = test_control_name + '_result';
                let test_description = node.expression.arguments[1].text;
                let test_code = node.expression.arguments[2].text;
                let code_lines = test_code.split("\n");
                if(code_lines[code_lines.length-1] == ''){
                    code_lines.pop();
                }
                if(code_lines[0] == ''){
                    code_lines.shift();
                }
                test_code = code_lines.join('\n');
                let code_rows =  code_lines.length;
                const one_test_case = `
                <!--<div>${test_name}</div>-->
                <div>${test_description}</div>
                <div style="margin-top: 10px;">
                    <textarea id="${test_control_name}"  rows = "${code_rows}" style="resize: none;">${test_code}</textarea>
                </div>
                <div>
                    <div style="display: inline-block; margin-right:5px;"><button type="button" onclick="run('${test_control_name}')" style="min-width: 50px;">run</button></div>
                    <div style="display: inline-block; margin-left:5px;" id= "${test_result_control_name}"></div>
                </div>
                `;
                test_cases += one_test_case;
            }
        }
    });
    var html_header = '';
    var html_footer = '';
    html_header = fs.readFileSync(html_header_file, 'utf8');
    html_footer = fs.readFileSync(html_footer_file, 'utf8');
    html_file = `${html_header}${test_navigations}
    </p>${test_cases}${html_footer}`;
    fs.writeFile(to_path, html_file, err => {
        if (err) {
          console.error(err);
        }
        // file written successfully
      });
}

function build_test_navigations(interested_files, current_test_name)
{
    var test_cases = '';
    interested_files.forEach(file_info=>{
        let test_name = file_info[1];
        let test_html = file_info[2];
        const test_case = test_name == current_test_name? `<a>${test_name}</a>` : `<a href="/${test_html}">${test_name}</a>`;
        test_cases += '\n' + test_case;
    });
    var test_navigations = `
        <nav>
            ${test_cases}
        </nav>`;
    return test_navigations;
}

function main()
{
    fs.readdir(from_dir, function (err, files) {
        if(err){
            console.error("could not list the directory.", err);
            process.exit(1);
        }
        interested_files = new Array();
        files.forEach(function(file, index){
            if (exclude_list.includes(file) || !include_list.has(file)){
                return;
            }
            let test_name = file.replace('.test.js', '');
            let test_html = file.replace('.test.js', '-tut.html');
            interested_files.push([include_list.get(file), test_name, test_html, file]);
        });
        interested_files.sort();
        interested_files.forEach(file_info=>{
            let test_name = file_info[1];
            let test_html = file_info[2];
            let file= file_info[3];
            var from_path = path.join(from_dir, file);
            var to_path = path.join(to_dir, test_html);
            var test_navigations = build_test_navigations(interested_files, test_name);
            fs.stat(from_path, function(error, stat){
                if(error){
                    console.error("Error stating file.", error);
                    return;
                }
                if(stat.isFile()){
                    generate_file(from_path, test_navigations, to_path);
                }
            });
        });
    });
}

main();