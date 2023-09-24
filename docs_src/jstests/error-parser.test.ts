import {mtest_strings} from './mtest';

mtest_strings('parse fun def missing :', 'parse fun def missing :', `
def f(x)
`,[ "symbol NEWLINE is not expected after grammar rule func_param_decls = ( param_decls )\n"]);
