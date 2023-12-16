import {mtest_strings} from './mtest';

mtest_strings('parse func def missing :', 'parse func def missing :', `
def f(x)
`,[ "symbol NEWLINE is not expected after grammar rule func_param_decls = ( param_decls )\n"]);
