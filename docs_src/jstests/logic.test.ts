/**
 * logic operators
 */
import { mtest } from './mtest';

mtest('true', 'true is evaluated as integer 1.', "True", 1);

mtest('false', 'false is evaluated as integer 0.', "False", 0);

mtest('true and true', 'true && true should be true.', "True and True", 1);

mtest('true and false', 'true && false should be false.', "True and False", 0);

mtest('false and true', 'false && true should be false.', "False and True", 0, false);

mtest('false and false', 'false && false should be false.', "False and False", 0);

mtest('false or false', 'false || false should be false.', "False or False", 0);

mtest('true or false', 'true || false should be true.', "True or False", 1);

mtest('false or true', 'false || true should be true.', "True or False", 1, false);

mtest('true or true', 'true || true should be true.', "True or True", 1, false);

mtest('not true', 'not true should be false.', "not True", 0);

mtest('not false', 'not false is true.', "not False", 1);
