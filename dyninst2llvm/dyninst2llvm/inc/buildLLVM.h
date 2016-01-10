// a set of utility to make llvm stuff
#ifndef BUILD_LLVM_H__
#define BUILD_LLVM_H__
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/IR/Verifier.h>
#include "llvm/IR/IRBuilder.h"
#include <algorithm>
#include "BPatch.h"
#include "BPatch_addressSpace.h"
#include "BPatch_process.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "BPatch_flowGraph.h"



class buildLLVM{
public:
    llvm::Module* makeLLVMModule();
    llvm::BasicBlock* makeLLVMBasicBlock(BPatch_basicBlock* binBB);
    void dumpModuleContent(llvm::Module* mod, llvm::raw_ostream& out);

};



class bin2llvmFunction
{
public:
    bin2llvmFunction(BPatch_function* of,llvm::Module* top);
    llvm::Function* makeFunction();
    void populateFunction(llvm::Function* popTgt);
private:
    BPatch_function* func;
    llvm::Module* parent;
};


class bin2llvmBasicBlock
{
public:
    bin2llvmBasicBlock(BPatch_basicBlock* obb, llvm::Function* top);
    llvm::BasicBlock* makeBasicBlock();
    void populateBasicBlock();
private:
    BPatch_basicBlock* bblock;
    llvm::Function* parent;

};

class bin2llvmInstruction
{
public:
    bin2llvmInstruction(BPatch_instruction* oi);
    void generateInstruction(llvm::IRBuilder<>& bbBuilder);
private:
    BPatch_instruction* binsn;
};

#endif
