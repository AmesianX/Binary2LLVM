#include "buildLLVM.h"
using namespace llvm;
/*int main(int argc, char**argv) {
  Module* Mod = makeLLVMModule();
  verifyModule(*Mod);
  PassManager PM;
  PM.add(createPrintModulePass(outs()));
  PM.run(*Mod);
  return 0;
}*/
/*

Module* buildLLVM::makeLLVMModule() {
    // Module Construction,
    Module* mod = new Module("test.ll", getGlobalContext());
    return mod;
}



void buildLLVM::dumpModuleContent(Module* mod, raw_ostream& out)
{
    verifyModule(*mod);
    PassManager PM;
    PM.add(createPrintModulePass(out));
    PM.run(*mod);
}



bin2llvmBasicBlock::bin2llvmBasicBlock(BPatch_basicBlock* obb, Function* top):bblock(obb),parent(top)
{
}
BasicBlock* bin2llvmBasicBlock::makeBasicBlock()
{

    BasicBlock* newBB = llvm::BasicBlock::Create(parent->getContext(),"BasicBlock",parent);
    populateBasicBlock(newBB);
    return newBB;
}
void bin2llvmBasicBlock::populateBasicBlock(BasicBlock* popTgt)
{
    // first check whatever registers are live
    // here, and check if they have divergent sources?
    BPatch_point* ept = bblock->findEntryPoint();
    std::vector<BPatch_register> curLiveRegs;

    ept->getLiveRegisters(curLiveRegs);
    // we will need to trace back to the predecessors,
    // for every live register, we do breadth first search
    // backward to find where the register is def-ed
    // in the dataflow graph, this stops when
    // we hit the dominator, or a block where a def happens
    // i.e. if the dominator does not have this register has live
    // and it does not def it, we actually have a problem
    // when the dominator has it live and no other def is in
    // our list, we dont need a phi, as there is only one source
    // if we have a



    return;
}

bin2llvmInstruction::bin2llvmInstruction(BPatch_instruction* oinsn):binsn(oinsn)
{
}
void bin2llvmInstruction::generateInstruction(IRBuilder<>& bbBuilder)
{
}


bin2llvmFunction::bin2llvmFunction(BPatch_function* of,Module* top):func(of),parent(top)
{
}
// we will always return void and takes in nothing for that function
// as everything will be read off the stack and push onto the stack
Function* bin2llvmFunction::makeFunction()
{

    LLVMContext& context = parent->getContext();
    Type* rtType = Type::getVoidTy(context);
    std::vector<Type*> paramsType;FunctionType* newFuncType = FunctionType::get(rtType,ArrayRef<Type*>(paramsType),false);
    std::string partFuncName = this->func->getName();
    Constant* tmpFuncC = parent->getOrInsertFunction(partFuncName, newFuncType);
    Function* actualNewFunc = cast<Function>(tmpFuncC);
    populateFunction(actualNewFunc);
    return actualNewFunc;
}
void bin2llvmFunction::populateFunction(llvm::Function* popTgt)
{
    BPatch_flowGraph *fg = func->getCFG();
    std::set<BPatch_basicBlock *> blocks;
    fg->getAllBasicBlocks(blocks);
    std::set<BPatch_basicBlock *>::iterator block_iter;
    for (block_iter = blocks.begin(); block_iter != blocks.end(); ++block_iter)
    {
        BPatch_basicBlock *block = *block_iter;
        // make basic block
        bin2llvmBasicBlock b2b(block,popTgt);
        llvm::BasicBlock* curBB = b2b.makeBasicBlock();


    }


}
*/
