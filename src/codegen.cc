#include "codegen.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#include "util.h"

using namespace llvm;
using namespace std;

llvm::LLVMContext g_context;


void* _generate_global_var_node(code_generator* cg, var_node* node,
                                bool is_external = false);
void* _generate_local_var_node(code_generator* cg, var_node* node);
void* _generate_prototype_node(code_generator* cg, exp_node* node);
void* _generate_block_node(code_generator* cg, exp_node* block);

code_generator* create_code_generator(parser* parser) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  code_generator* cg = new code_generator();
  cg->parser = parser;
  cg->context = &g_context;
  cg->builder = new llvm::IRBuilder<>(g_context);
  return cg;
}

void destroy_code_generator(code_generator* cg) {
  delete (llvm::IRBuilder<>*)cg->builder;
  delete cg;
}

Function* _get_function(code_generator* cg, std::string name) {
  // First, see if the function has already been added to the current module.
  if (auto* f = cg->module->getFunction(name)) return f;

  // If not, check whether we can codegen the declaration from some existing
  // prototype.
  auto fp = cg->protos.find(name);
  if (fp != cg->protos.end())
    return (Function*)_generate_prototype_node(cg, (exp_node*)fp->second);

  // If no existing prototype exists, return null.
  return nullptr;
}

GlobalVariable* _get_global_variable(code_generator* cg, std::string name) {
  // First, see if the function has already been added to the current module.
  if (auto* gv = cg->module->getNamedGlobal(name)) return gv;

  // If not, it's defined in other module, we can codegen the external
  // declaration from existing type.
  auto fgv = cg->gvs.find(name);
  if (fgv != cg->gvs.end()) {
    // log(DEBUG, "found defition before");
    return (GlobalVariable*)_generate_global_var_node(cg, fgv->second, true);
  }

  // If no existing prototype exists, return null.
  return nullptr;
}

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
llvm::AllocaInst* _create_entry_block_alloca(code_generator* cg,
                                             llvm::Function* fun,
                                             const std::string& var_name) {
  llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
  llvm::IRBuilder<> builder(&fun->getEntryBlock(),
                            fun->getEntryBlock().begin());
  return builder.CreateAlloca(llvm::Type::getDoubleTy(*context), 0,
                              var_name.c_str());
}

/// _create_argument_allocas - Create an alloca for each argument and register
/// the argument in the symbol table so that references to it will succeed.
void _create_argument_allocas(code_generator* cg, prototype_node* node,
                              llvm::Function* fun) {
  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
  llvm::Function::arg_iterator arg_it = fun->arg_begin();
  for (size_t i = 0, e = node->args.size(); i != e; ++i, ++arg_it) {
    // Create an alloca for this variable.
    llvm::AllocaInst* alloca =
        _create_entry_block_alloca(cg, fun, node->args[i]);

    // Create a debug descriptor for the variable.
    /*DIScope *Scope = KSDbgInfo.LexicalBlocks.back();
    DIFile *Unit = DBuilder->createFile(KSDbgInfo.TheCU->getFilename(),
                                        KSDbgInfo.TheCU->getDirectory());
    DILocalVariable *D = DBuilder->createLocalVariable(
                                                       dwarf::DW_TAG_arg_variable,
    Scope, Args[Idx], Unit, Line, KSDbgInfo.getDoubleTy(), Idx);

    DBuilder->insertDeclare(Alloca, D, DBuilder->createExpression(),
                            DebugLoc::get(Line, 0, Scope),
                            Builder.GetInsertBlock());
    */

    // Store the initial value into the alloca.
    builder->CreateStore(arg_it, alloca);

    // Add arguments to variable symbol table.
    cg->named_values[node->args[i]] = alloca;
  }
}

void* _generate_num_node(code_generator* cg, exp_node* node) {
  llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
  return llvm::ConstantFP::get(*context, llvm::APFloat(((num_node*)node)->double_val));
}

void* _generate_ident_node(code_generator* cg, exp_node* node) {
  auto ident = (ident_node*)node;
  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
  llvm::Value* v = (llvm::Value*)cg->named_values[ident->name];
  if (!v) {
    GlobalVariable* gVar = _get_global_variable(cg, ident->name);
    if (gVar) {
      return builder->CreateLoad(gVar, ident->name.c_str());
    } else {
      log(ERROR, "Unknown variable name: %s", ident->name.c_str());
      return 0;
    }
  }
  return builder->CreateLoad(v, ident->name.c_str());
}

void* _generate_binary_node(code_generator* cg, exp_node* node) {
  auto bin = (binary_node*)node;
  llvm::Value* lv = (llvm::Value*)generate_code(cg, bin->lhs);
  llvm::Value* rv = (llvm::Value*)generate_code(cg, bin->rhs);
  if (!lv || !rv) return nullptr;
  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
  if (bin->op=="+")
    return builder->CreateFAdd(lv, rv, "addtmp");
  else if (bin->op=="-")
    return builder->CreateFSub(lv, rv, "subtmp");
  else if (bin->op=="*")
    return builder->CreateFMul(lv, rv, "multmp");
  else if (bin->op=="/")
    return builder->CreateFDiv(lv, rv, "divtmp");
  else if (bin->op=="<"){
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    lv = builder->CreateFCmpULT(lv, rv, "cmptmp");
    return builder->CreateUIToFP(lv, llvm::Type::getDoubleTy(*context),
                                  "booltmp");
  }
  else if (bin->op==">"){
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    lv = builder->CreateFCmpUGT(lv, rv, "cmptmp");
    return builder->CreateUIToFP(lv, llvm::Type::getDoubleTy(*context),
                                  "booltmp");
  }
  else if (bin->op=="=="){
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    lv = builder->CreateFCmpUEQ(lv, rv, "cmptmp");
    return builder->CreateUIToFP(lv, llvm::Type::getDoubleTy(*context),
                                  "booltmp");
  }
  else if (bin->op=="!="){
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    lv = builder->CreateFCmpUNE(lv, rv, "cmptmp");
    return builder->CreateUIToFP(lv, llvm::Type::getDoubleTy(*context),
                                  "booltmp");
  }
  else if (bin->op=="<="){
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    lv = builder->CreateFCmpULE(lv, rv, "cmptmp");
    return builder->CreateUIToFP(lv, llvm::Type::getDoubleTy(*context),
                                  "booltmp");
  }
  else if (bin->op==">="){
    llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
    lv = builder->CreateFCmpUGE(lv, rv, "cmptmp");
    return builder->CreateUIToFP(lv, llvm::Type::getDoubleTy(*context),
                                  "booltmp");
  }
  else{
    Function *func = _get_function(cg, std::string("binary") + bin->op);
    assert(func && "binary operator not found!");
    Value *ops[2] = { lv, rv };
    return builder->CreateCall(func, ops, "binop");
  }
}

void* _generate_call_node(code_generator* cg, exp_node* node) {
  auto call = (call_node*)node;
  llvm::Function* callee = _get_function(cg, call->callee);
  if (!callee)
    return log(ERROR, "Unknown function referenced: %s", call->callee.c_str());
  if (callee->arg_size() != call->args.size())
    return log(ERROR,
               "Incorrect number of arguments passed: callee (prototype "
               "generated in llvm): %lu, calling: %lu",
               callee->arg_size(), call->args.size());

  std::vector<llvm::Value*> arg_values;
  for (unsigned long i = 0, e = call->args.size(); i != e; ++i) {
    arg_values.push_back((llvm::Value*)generate_code(cg, call->args[i]));
    if (!arg_values.back()) return 0;
  }
  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
  return builder->CreateCall(callee, arg_values, "calltmp");
}

void* _generate_prototype_node(code_generator* cg, exp_node* node) {
  auto proto = (prototype_node*)node;
  cg->protos[proto->name] = proto;
  llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
  std::vector<llvm::Type*> doubles(proto->args.size(),
                                   llvm::Type::getDoubleTy(*context));
  llvm::FunctionType* ft = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(*context), doubles, false);
  llvm::Function* fun = llvm::Function::Create(
      ft, llvm::Function::ExternalLinkage, proto->name, cg->module.get());
  unsigned i = 0;
  for (auto& arg : fun->args()) arg.setName(proto->args[i++]);
  return fun;
}

void* _generate_function_node(code_generator* cg, exp_node* node) {
  auto funn = (function_node*)node;
  cg->named_values.clear();
  llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
  auto fun = (llvm::Function*)_generate_prototype_node(cg, (exp_node*)funn->prototype);
  if (!fun) return 0;
  // if (is_binary_op(node->prototype)){
  //   log(DEBUG, "found a binary op def ! op:%c, prec: %d", get_op_name(node->prototype), node->prototype->precedence);
  //   (*cg->parser->op_precedences)[get_op_name(node->prototype)] =
  //       node->prototype->precedence;
  // }
  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
  llvm::BasicBlock* bb = llvm::BasicBlock::Create(*context, "entry", fun);
  builder->SetInsertPoint(bb);
  _create_argument_allocas(cg, funn->prototype, fun);
  llvm::Value* ret_val;
  for (auto stmt : funn->body->nodes) {
    ret_val = (llvm::Value*)generate_code(cg, stmt);
  }
  if (!ret_val) {
    ret_val = llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
  }
  if (ret_val) {
    builder->CreateRet(ret_val);
    cg->fpm->run(*fun);
    llvm::verifyFunction(*fun);
    return fun;
  }
  fun->eraseFromParent();
  // if (is_binary_op(node->prototype))
  //   cg->parser->op_precedences->erase(get_op_name(node->prototype));
  return 0;
}

void* _generate_unary_node(code_generator* cg, exp_node* node) {
  auto unary = (unary_node*)node;
  llvm::Value* operand_v = (llvm::Value*)generate_code(cg, unary->operand);
  if (operand_v == 0) return 0;

  llvm::Function* fun = _get_function(cg, std::string("unary") + unary->op);
  if (fun == 0) return log(ERROR, "Unknown unary operator");

  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
  // KSDbgInfo.emitLocation(this);
  return builder->CreateCall(fun, operand_v, "unop");
}

void* _generate_condition_node(code_generator* cg, exp_node* node) {
  // KSDbgInfo.emitLocation(this);
  auto cond = (condition_node*)node;
  llvm::Value* cond_v = (llvm::Value*)generate_code(cg, cond->condition_node);
  if (cond_v == 0) return 0;

  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
  llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
  // Convert condition to a bool by comparing equal to 0.0.
  cond_v = builder->CreateFCmpONE(
      cond_v, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "ifcond");

  llvm::Function* fun = builder->GetInsertBlock()->getParent();

  // Create blocks for the then and else cases.  Insert the 'then' block at the
  // end of the function.
  llvm::BasicBlock* then_bb = llvm::BasicBlock::Create(*context, "then", fun);
  llvm::BasicBlock* else_bb = llvm::BasicBlock::Create(*context, "else");
  llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*context, "ifcont");

  builder->CreateCondBr(cond_v, then_bb, else_bb);

  // Emit then value.
  builder->SetInsertPoint(then_bb);

  llvm::Value* then_v = (llvm::Value*)generate_code(cg, cond->then_node);
  if (then_v == 0) return 0;

  builder->CreateBr(merge_bb);
  // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
  then_bb = builder->GetInsertBlock();

  // Emit else block.
  fun->getBasicBlockList().push_back(else_bb);
  builder->SetInsertPoint(else_bb);

  llvm::Value* else_v = (llvm::Value*)generate_code(cg, cond->else_node);
  if (else_v == 0) return 0;

  builder->CreateBr(merge_bb);
  // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
  else_bb = builder->GetInsertBlock();

  // Emit merge block.
  fun->getBasicBlockList().push_back(merge_bb);
  builder->SetInsertPoint(merge_bb);
  llvm::PHINode* phi_node =
      builder->CreatePHI(llvm::Type::getDoubleTy(*context), 2, "iftmp");

  phi_node->addIncoming(then_v, then_bb);
  phi_node->addIncoming(else_v, else_bb);
  return phi_node;
}

void* _generate_global_var_node(code_generator* cg, var_node* node,
                                bool is_external) {
  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
  llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
  auto gVar = cg->module->getNamedGlobal(node->var_name);
  auto exp = generate_code(cg, node->init_value);
  if (!gVar) {
    if (is_external) {
      gVar = new llvm::GlobalVariable(
          *cg->module, builder->getDoubleTy(), false,
          llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, node->var_name,
          nullptr, GlobalValue::ThreadLocalMode::NotThreadLocal, 0, true);
      return gVar;
    } else {
      cg->gvs[node->var_name] = node;
      gVar = new llvm::GlobalVariable(
          *cg->module, builder->getDoubleTy(), false,
          llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0,
          node->var_name);  //, nullptr,
                            //GlobalValue::ThreadLocalMode::NotThreadLocal, 0,
                            //false);
      gVar->setInitializer(
          llvm::ConstantFP::get(*context, llvm::APFloat(0.0)));
      builder->CreateStore((Value*)exp, gVar);
    }
    // return builder->CreateLoad(gVar);
    // log(DEBUG, "create the load for gV");
    // return x;
  }
  return 0;
}

void* _generate_var_node(code_generator* cg, exp_node* node) {
  auto var = (var_node*)node;
  if (!var->base.parent)
    return _generate_global_var_node(cg, var);
  else
    return _generate_local_var_node(cg, var);
}

void* _generate_local_var_node(code_generator* cg, var_node* node) {
  std::vector<llvm::AllocaInst*> old_bindings;

  llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;
  // fprintf(stderr, "_generate_var_node:1 %lu!, %lu\n", node->var_names.size(),
  // (long)builder->GetInsertBlock());
  llvm::Function* fun = builder->GetInsertBlock()->getParent();
  // fprintf(stderr, "_generate_var_node:2 %lu!\n", node->var_names.size());

  // Register all variables and emit their initializer.
  const std::string& var_name = node->var_name;
  // log(DEBUG, "local var cg: %s", var_name.c_str());
  exp_node* init = node->init_value;

  // Emit the initializer before adding the variable to scope, this prevents
  // the initializer from referencing the variable itself, and permits stuff
  // like this:
  //  var a = 1 in
  //    var a = a in ...   # refers to outer 'a'.
  llvm::Value* init_val;
  if (init) {
    init_val = (llvm::Value*)generate_code(cg, init);
    if (init_val == 0) return 0;
  } else {  // If not specified, use 0.0.
    init_val = llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
  }

  llvm::AllocaInst* alloca = _create_entry_block_alloca(cg, fun, var_name);
  builder->CreateStore(init_val, alloca);

  // Remember the old variable binding so that we can restore the binding when
  // we unrecurse.
  old_bindings.push_back((llvm::AllocaInst*)cg->named_values[var_name]);

  // Remember this binding.
  cg->named_values[var_name] = alloca;
  return 0;
  // KSDbgInfo.emitLocation(this);
  // Codegen the body, now that all vars are in scope.

  // llvm::Value *body_val = (llvm::Value*)generate_code(cg, node->body);
  // if (body_val == 0)
  //     return 0;

  // // Pop all our variables from scope.
  // for (size_t i = 0, e = node->var_names.size(); i != e; ++i)
  //     cg->named_values[node->var_names[i].first] = old_bindings[i];
  // // Return the body computation.
  // return body_val;
}

void* _generate_for_node(code_generator* cg, exp_node* node) {
  // Output this as:
  //   var = alloca double
  //   ...
  //   start = startexpr
  //   store start -> var
  //   goto loop
  // loop:
  //   ...
  //   bodyexpr
  //   ...
  // loopend:
  //   step = stepexpr
  //   endcond = endexpr
  //
  //   curvar = load var
  //   nextvar = curvar + step
  //   store nextvar -> var
  //   br endcond, loop, endloop
  // outloop:
  auto forn = (for_node*)node;
  llvm::LLVMContext* context = (llvm::LLVMContext*)cg->context;
  llvm::IRBuilder<>* builder = (llvm::IRBuilder<>*)cg->builder;

  llvm::Function* fun = builder->GetInsertBlock()->getParent();

  // Create an alloca for the variable in the entry block.
  llvm::AllocaInst* alloca =
      _create_entry_block_alloca(cg, fun, forn->var_name);

  // KSDbgInfo.emitLocation(this);
  // Emit the start code first, without 'variable' in scope.
  llvm::Value* start_v = (llvm::Value*)generate_code(cg, forn->start);
  if (start_v == 0) return 0;

  // Store the value into the alloca.
  builder->CreateStore(start_v, alloca);

  // Make the new basic block for the loop header, inserting after current
  // block.
  llvm::BasicBlock* loop_bb = llvm::BasicBlock::Create(*context, "loop", fun);

  // Insert an explicit fall through from the current block to the LoopBB.
  builder->CreateBr(loop_bb);

  // Start insertion in LoopBB.
  builder->SetInsertPoint(loop_bb);

  // Within the loop, the variable is defined equal  the PHI node.  If it
  // shadows an existing variable, we have to restore it, so save it now.
  llvm::AllocaInst* old_alloca =
      (llvm::AllocaInst*)cg->named_values[forn->var_name];
  cg->named_values[forn->var_name] = alloca;

  // Emit the body of the loop.  This, like any other expr, can change the
  // current BB.  Note that we ignore the value computed by the body, but don't
  // allow an error.
  if (generate_code(cg, forn->body) == 0) return 0;

  // Emit the step value.
  llvm::Value* step_v;
  if (forn->step) {
    step_v = (llvm::Value*)generate_code(cg, forn->step);
    if (step_v == 0) return 0;
  } else {
    // If not specified, use 1.0.
    step_v = llvm::ConstantFP::get(*context, llvm::APFloat(1.0));
  }

  // Compute the end condition.
  llvm::Value* end_cond = (llvm::Value*)generate_code(cg, forn->end);
  if (end_cond == 0) return end_cond;

  // Reload, increment, and restore the alloca.  This handles the case where
  // the body of the loop mutates the variable.
  llvm::Value* cur_var = builder->CreateLoad(alloca, forn->var_name.c_str());
  llvm::Value* next_var = builder->CreateFAdd(cur_var, step_v, "nextvar");
  builder->CreateStore(next_var, alloca);

  // Convert condition to a bool by comparing non-equal to 0.0.
  end_cond = builder->CreateFCmpONE(
      end_cond, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)),
      "loopcond");

  // Create the "after loop" block and insert it.
  llvm::BasicBlock* after_bb =
      llvm::BasicBlock::Create(*context, "afterloop", fun);

  // Insert the conditional branch into the end of LoopEndBB.
  builder->CreateCondBr(end_cond, loop_bb, after_bb);

  // Any new code will be inserted in AfterBB.
  builder->SetInsertPoint(after_bb);

  // Restore the unshadowed variable.
  if (old_alloca)
    cg->named_values[forn->var_name] = old_alloca;
  else
    cg->named_values.erase(forn->var_name);

  // for expr always returns 0.0.
  return llvm::Constant::getNullValue(llvm::Type::getDoubleTy(*context));
}

void* _generate_block_node(code_generator* cg, exp_node* node) {
  auto block = (block_node*)node;
  void* codegen;
  for (auto exp : block->nodes) {
    codegen = generate_code(cg, exp);
  }
  return codegen;
}

void create_module_and_pass_manager(code_generator* cg,
                                    const char* module_name) {
  // Open a new module.
  auto context = (llvm::LLVMContext*)cg->context;
  std::unique_ptr<llvm::Module> module =
      std::make_unique<llvm::Module>(module_name, *context);
  module->setDataLayout(
      llvm::EngineBuilder().selectTarget()->createDataLayout());

  // Create a new pass manager attached to it.
  cg->fpm = std::make_unique<llvm::legacy::FunctionPassManager>(module.get());

  // Promote allocas to registers.
  cg->fpm->add(createPromoteMemoryToRegisterPass());
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  cg->fpm->add(createInstructionCombiningPass());
  // Reassociate expressions.
  cg->fpm->add(createReassociatePass());
  // Eliminate Common SubExpressions.
  cg->fpm->add(createNewGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  cg->fpm->add(createCFGSimplificationPass());

  cg->fpm->doInitialization();

  cg->module = std::move(module);
}

void generate_runtime_module(code_generator* cg, parser* parser) {
  llvm::ArrayRef<exp_node*> nodesRef = parser->ast->builtins;
  for (int i = 0; i < nodesRef.size(); i++) {
    generate_code(cg, nodesRef[i]);
  }
}

void* _generate_unk_node(code_generator* cg, exp_node* node){
  return nullptr;
}

void* (*cg_fp[])(code_generator*, exp_node*) = { 
  _generate_unk_node,
  _generate_num_node,
  _generate_ident_node,
  _generate_var_node,
  _generate_unary_node,
  _generate_binary_node,
  _generate_condition_node,
  _generate_for_node,
  _generate_call_node,
  _generate_prototype_node,
  _generate_function_node,
  _generate_block_node
};

void* generate_code(code_generator* cg, exp_node* node) {
  return cg_fp[node->node_type](cg, node);
}
