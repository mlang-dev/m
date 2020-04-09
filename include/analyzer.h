#include "ast.h"

struct type_env{
  map<string, type_exp*> type_env;
  map<type_exp*, type_exp*> types;
  vector<type_exp*> nogens;
};

type_env* create_type_env();
void destroy_type_env(type_env* env);
type_exp* analyze(type_env* env, exp_node* node);
