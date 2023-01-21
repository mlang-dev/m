/**
 * relational operators
 */
import { mtest } from './mtest';


mtest('less than', '10 < 20 is evaluated as true (1).', "10 < 20", 1);

mtest('less than float', '10.0 < 20.0 is evaluated as true (1).', "10.0 < 20.0", 1);

mtest('less and equal than float', '10.0 <= 20.0 is evaluated as true (1).', "10.0 <= 20.0", 1);

mtest('less and equal than int', '10 <= 20 is evaluated as true (1).', "10 <= 20", 1);

mtest('less and equal than int equal', '20 <= 20 is evaluated as true (1).', "20 <= 20", 1);

mtest('less and equal than int false', '21 <= 20 is evaluated as false (0).', "21 <= 20", 0);

mtest('greater and equal than int', '21 >= 20 is evaluated as true (1).', "21 >= 20", 1);

mtest('greater than int', '21 > 20 is evaluated as true (1).', "21 > 20", 1);

mtest('equal int false', '21 == 20 is evaluated as false (0).', "21 == 20", 0);

mtest('equal int', '20 == 20 is evaluated as true (1).', "20 == 20", 1);

mtest('not equal int', '21 != 20 is evaluated as true (1).', "21 != 20", 1);

mtest('not equal int false', '20 != 20 is evaluated as false (0).', "20 != 20", 0);