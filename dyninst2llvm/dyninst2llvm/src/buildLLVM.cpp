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
    return 0;
}
void bin2llvmBasicBlock::populateBasicBlock()
{
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
// there
Function* bin2llvmFunction::makeFunction()
{

    return 0;
}
void bin2llvmFunction::populateFunction()
{

}

