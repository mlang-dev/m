import {mtest_strings} from './mtest';

mtest_strings('parse fun def missing =', 'parse fun def missing =', `
let f x
`,[ "symbol : is expected to parse param_decl = IDENT .: type_expr but got NEWLINE\n",
    "symbol NEWLINE is not expected after grammar rule param_decl = IDENT\n"]);
