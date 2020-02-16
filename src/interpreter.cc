#include "jit.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

void eval_statement(exp_node* node, void* vp_jit){
  auto jit = (JIT*)vp_jit;
  if(node->type == NodeType::PROTOTYPE_NODE)
      generate_code(jit->cg, node);
  else if (node->type == NodeType::FUNCTION_NODE){
      //function definition
      //log(DEBUG, "it's a definition. %d", node->type);
      auto def = generate_code(jit->cg, node);
      //dumpm(jit->cg->module.get());
      jit->mjit->addModule(std::move(jit->cg->module));
      create_module_and_pass_manager(jit->cg, make_unique_name("mjit").c_str());
  }else{
      //expression: statement or expression evalution
      auto fn = make_unique_name("main-fn");
      auto node_type = node->type;
      node = parse_exp_to_function(jit->cg->parser, node, fn.c_str());
      if(node){
          if(auto p_fun = generate_code(jit->cg, node)){
              //dumpm(jit->cg->module.get());
              auto mk = jit->mjit->addModule(std::move(jit->cg->module));
              auto mf = jit->mjit->findSymbol(fn);
              double (*fp)() = (double (*)())(intptr_t)cantFail(mf.getAddress());
              auto result = fp();
              if (node_type != NodeType::VAR_NODE){ //keep global variables in the jit
                  printf("%f\n", fp());
                  jit->mjit->removeModule(mk);
              }
              create_module_and_pass_manager(jit->cg, make_unique_name("mjit").c_str());
          }
      }
  }
  fprintf(stderr, "m> ");
}

int run_interactive(){
    parser* parser = create_parser(true, stdin);
    code_generator* cg = create_code_generator(parser);
    JIT* jit = create_jit(cg);
    create_builtins(parser, cg->context);
    create_module_and_pass_manager(cg, make_unique_name("mjit").c_str());
    generate_runtime_module(cg, parser);
    jit->mjit->addModule(std::move(jit->cg->module));
    create_module_and_pass_manager(cg, make_unique_name("mjit").c_str());
    fprintf(stderr, "m> ");
    parse_block(parser, nullptr, &eval_statement, jit);
    fprintf(stderr, "bye!\n");
    destroy_jit(jit);
    destroy_code_generator(cg);
    destroy_parser(parser);
    return 0;
}