import {mtest_string} from './mtest';


mtest_string('analyzer type mismatch', 'analyzer type mismatch', `
let i:int = "string"
`, "variable type not matched with literal constant. loc (line, col): (2, 5)\n");
