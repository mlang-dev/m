#include "jit.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"

double eval_exp(JIT* jit, exp_node* node){
  // expression: statement or expression evalution
  auto fn = make_unique_name("main-fn");
  auto node_type = node->type;
  double result;
  node = parse_exp_to_function(jit->cg->parser, node, fn.c_str());
  if (node) {
    if (auto p_fun = generate_code(jit->cg, node)) {
      //dumpm(jit->cg->module.get());
      auto mk = jit->mjit->addModule(std::move(jit->cg->module));
      auto mf = jit->mjit->findSymbol(fn);
      double (*fp)() = (double (*)())(intptr_t)cantFail(mf.getAddress());
      // keep global variables in the jit
      result = fp();
      if (node_type != NodeType::VAR_NODE) {
        jit->mjit->removeModule(mk);
      }
      create_module_and_pass_manager(jit->cg,
                                      make_unique_name("mjit").c_str());
    }
  }
  return result;
}

void eval_statement(void* p_jit, exp_node* node) {
  if (node) {
    auto jit = (JIT*)p_jit;
    if (node->type == NodeType::PROTOTYPE_NODE)
      generate_code(jit->cg, node);
    else if (node->type == NodeType::FUNCTION_NODE) {
      // function definition
      auto def = generate_code(jit->cg, node);
      jit->mjit->addModule(std::move(jit->cg->module));
      create_module_and_pass_manager(jit->cg, make_unique_name("mjit").c_str());
    } else {
      auto result = eval_exp(jit, node);
      if(node->type!=NodeType::VAR_NODE)
        printf("%f\n", result);
    }
  }
  fprintf(stderr, "m> ");
}

JIT* build_jit(parser* parser){
  code_generator* cg = create_code_generator(parser);
  JIT* jit = create_jit(cg);
  create_builtins(parser, cg->context);
  create_module_and_pass_manager(cg, make_unique_name("mjit").c_str());
  generate_runtime_module(cg, parser);
  jit->mjit->addModule(std::move(jit->cg->module));
  create_module_and_pass_manager(cg, make_unique_name("mjit").c_str());
  return jit;
}

int run_repl() {
  parser* parser = create_parser(NULL, true, NULL);
  JIT* jit = build_jit(parser);
  fprintf(stderr, "m> ");
  parse_block(parser, nullptr, &eval_statement, jit);
  fprintf(stderr, "bye !\n");
  destroy_jit(jit);
  return 0;
}