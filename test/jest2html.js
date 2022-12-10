
var fs = require('fs');
var path = require('path');
var process = require("process");
const ts = require("typescript");
const doctrine = require("doctrine");

var from_dir = "./jstests/";

const tutorial_config = {
    "to_dir": "../docs/tutorial/",
    "show_graph": false,
    "template": {
        "header": "./tutorial.html_header_template",
        "footer": "./tutorial.html_footer_template"
    },
    "list": {
        "general": {
            "files": ["general.test.js"],
            "order": 1
        },
        "operators": {
            "files": ["bitwise.test.js", "logic.test.js", "relation.test.js", "numeric.test.js"],
            "order" : 2,
        },
        "control": {
            "files": ["control.test.js"],
            "order": 3,
        },
        "aggregate": {
            "files": ["aggregate.test.js"],
            "order": 4,
        },
        "reference": {
            "files": ["reference.test.js"],
            "order": 5
        },
        "pattern-match": {
            "files": ["pattern-match.test.js"],
            "order": 6
        }
    }
};

const example_config = {
    "to_dir": "../docs/example/",
    "show_graph": true,
    "template": {
        "header": "./example.html_header_template",
        "footer": "./example.html_footer_template"
    },
    "list": {
        "general": {
            "files": ["example-mandelbrot.test.js"],
            "order": 1
        }
    }
};

function _generate_content_from_path(from_path, show_graph)
{
    let program = ts.createProgram([from_path], {allowJs: true, removeComments: false});
    const sourceFile = program.getSourceFile(from_path);
    const commentRanges = ts.getLeadingCommentRanges(
        sourceFile.getFullText(), 
        sourceFile.getFullStart());
    var file_description = '';
    if (commentRanges && commentRanges.length){
        const commentStrings = commentRanges.map(r=>sourceFile.getFullText().slice(r.pos,r.end));
        var ast = doctrine.parse(commentStrings[0], { unwrap: true });
        file_description = ast.description;
    }
    var test_cases = '';
    ts.forEachChild(sourceFile, node => {
        if(ts.isExpressionStatement(node)){
            if(ts.isCallExpression(node.expression) && 
            node.expression.expression.expression != undefined && 
            node.expression.expression.expression.escapedText == 'mtest'){
                tutorial = node.expression.arguments[4];
                if(tutorial!=undefined && tutorial.kind == ts.SyntaxKind.FalseKeyword) 
                    return;
                let test_name = node.expression.arguments[0].text;
                let test_control_name = test_name.replace(/[ .,]/g, '_').toLowerCase();
                let test_result_control_name = test_control_name + '_result';
                let test_result_graph_name = test_control_name + '_graph_result';
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
                var graph_control = '';
                if (show_graph){
                    graph_control = `<div style="display: inline-block; width: 350px"><canvas id="${test_result_graph_name}" style="height: 240px; width: 100%;"></canvas></div>`;
                }
                const one_test_case = `
                <div style="font-size: small;">${test_description}</div>
                <div>
                    <div style="display: inline-block; margin-top: 10px; width: 400px">
                        <textarea id="${test_control_name}"  rows = "${code_rows}" style="resize: none; font-size: small">${test_code}</textarea>
                    </div>
                    ${graph_control}
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
    return `
    <div style="font-size: small">${file_description}</div>
    ${test_cases}`;
}

function generate_file(from_paths, test_navigations, to_path, html_header_file, html_footer_file, show_graph)
{
    console.log(from_paths, to_path);
    var html_header = '';
    var html_footer = '';
    html_header = fs.readFileSync(html_header_file, 'utf8');
    html_footer = fs.readFileSync(html_footer_file, 'utf8');
    html_from_file = '';
    for(var from_path of from_paths){
        html_from_file += _generate_content_from_path(from_path, show_graph);
    }
    html_file = `${html_header}${test_navigations}
    </p>
    ${html_from_file}${html_footer}`;
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
        const test_case = test_name == current_test_name? `<a class="current">${test_name}</a>` : `<a href="/tutorial/${test_html}">${test_name}</a>`;
        test_cases += '\n' + test_case;
    });
    var test_navigations = `
        <nav>
            ${test_cases}
        </nav>`;
    return test_navigations;
}

function generate_htmls(config)
{
    interested_files = new Array();
    list_pages = config["list"];
    for (const test_name of Object.keys(list_pages)){
        let test_html = `${test_name}.html`;
        let test_info = list_pages[test_name];
        interested_files.push([test_info["order"], test_name, test_html, test_info["files"]]);
    }
    interested_files.sort();
    interested_files.forEach(file_info=>{
        let test_name = file_info[1];
        let test_html = file_info[2];
        let files= file_info[3];
        var test_navigations = build_test_navigations(interested_files, test_name);
        var to_path = path.join(config["to_dir"], test_html);
        var from_paths = files.map(file=>path.join(from_dir, file));
        generate_file(from_paths, test_navigations, to_path, config["template"]["header"], config["template"]["footer"], config["show_graph"]);
    });
}

function main()
{
    if (process.argv.length > 2){
        
        //generate examples
        console.log("generating examples...");
        generate_htmls(example_config);
        return;
    }
    console.log("generating tutorials...");
    generate_htmls(tutorial_config);
}

main();