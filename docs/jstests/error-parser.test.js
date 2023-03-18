"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const mtest_1 = require("./mtest");
(0, mtest_1.mtest_strings)('parse fun def missing =', 'parse fun def missing =', `
let f x
`, ["symbol NEWLINE is not expected after grammar rule var_decl = IDENT\n",
    "symbol : is expected to parse ident_type = IDENT .: type_item but got NEWLINE\n"]);
//# sourceMappingURL=error-parser.test.js.map