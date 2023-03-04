
var fs = require('fs');

var grammar_file = "../../m/lib/parser/m_grammar.txt";
var to_file = "../docs/grammar.html";

const html_header_file = "./grammar.html_header_template";
const html_footer_file = "./grammar.html_footer_template";

function main()
{
    var html_header = '';
    var html_footer = '';
    html_header = fs.readFileSync(html_header_file, 'utf8');
    html_footer = fs.readFileSync(html_footer_file, 'utf8');
    let grammar_text = fs.readFileSync(grammar_file, 'utf8');
    let html_file = `${html_header}${grammar_text}${html_footer}`;
    fs.writeFile(to_file, html_file, err => {
        if (err) {
          console.error(err);
        }
        // file written successfully
      });
}

main();
