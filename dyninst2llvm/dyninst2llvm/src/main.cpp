#include "BPatch.h"
#include "BPatch_addressSpace.h"
#include "BPatch_process.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "BPatch_flowGraph.h"
#include <string>
#include "buildLLVM.h"
// Example 1: create an instance of class BPatch
BPatch bpatch;
// Example 2: attaching, creating, or opening a file for rewrite
typedef enum {
    create,
    attach,
    open
} accessType_t;

BPatch_addressSpace *startInstrumenting(accessType_t accessType,
    const char *name,
    int pid, // For attach
    const char *argv[]) { // For create
    BPatch_addressSpace *handle = NULL;
    switch (accessType) {
        case create:
            handle = bpatch.processCreate(name, argv);
            break;
        case attach:
            handle = bpatch.processAttach(name, pid);
            break;
        case open:
            handle = bpatch.openBinary(name);
            break;
    }
    return handle;
}

void iterateThroughFunction(BPatch_addressSpace * app,
 char* functionName, llvm::Module* go)
{
    llvm::outs()<<"checking out function with name "<<functionName<<"\n";
    std::vector<BPatch_function *> functions;
    BPatch_image *appImage = app->getImage();
    appImage->findFunction(functionName, functions);
    BPatch_flowGraph *fg = functions[0]->getCFG();
    std::set<BPatch_basicBlock *> blocks;
    fg->getAllBasicBlocks(blocks);
    std::set<BPatch_basicBlock *>::iterator block_iter;
    int insns_access_memory=0;
    int totalNumInst = 0;
    // find all the points of function call
    llvm::outs()<<"try to find fnctl\n";
    std::vector<BPatch_point*>* allCallPoint = functions[0]->findPoint(BPatch_subroutine);
    for(auto callPtIter = allCallPoint->begin(); callPtIter!= allCallPoint->end(); callPtIter++)
    {
        BPatch_function* calledFunc = (*callPtIter)->getCalledFunction();
        if(calledFunc)
            llvm::outs()<<calledFunc->getName()<<"\n";
        else
            llvm::outs()<<(*callPtIter)->getCalledFunctionName()<<"\n";
    }
    llvm::outs()<<allCallPoint->size()<<" call point found\n";

    for (block_iter = blocks.begin(); block_iter != blocks.end(); ++block_iter)
    {
        BPatch_basicBlock *block = *block_iter;
        std::vector<Dyninst::InstructionAPI::Instruction::Ptr> insns;
        block->getInstructions(insns);
        std::vector<Dyninst::InstructionAPI::Instruction::Ptr>::iterator insn_iter;
        for (insn_iter = insns.begin(); insn_iter != insns.end(); ++insn_iter)
        {
            Dyninst::InstructionAPI::Instruction::Ptr insn = *insn_iter;
            llvm::outs()<<insn->format()<<"\t\t\t\t\t";
            totalNumInst++;
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr> regsRead;
            insn->getReadSet( regsRead);
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr>::iterator curRegReadIter;
            llvm::outs()<<"register read: ";
            for(curRegReadIter = regsRead.begin();
                    curRegReadIter!= regsRead.end();
                    curRegReadIter++ )
            {
                llvm::outs()<<(*curRegReadIter)->format()<<" ";
            }

            llvm::outs()<<"\t\t";
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr> regsWrite;
            insn->getWriteSet( regsWrite);
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr>::iterator curRegWriteIter;
            llvm::outs()<<"register write: ";
            for(curRegWriteIter = regsWrite.begin();
                    curRegWriteIter!= regsWrite.end();
                    curRegWriteIter++ )
            {
                llvm::outs()<<(*curRegWriteIter)->format()<<" ";

            }
            std::vector<Dyninst::InstructionAPI::Operand> operands;
            insn->getOperands(operands);
            for(auto operandIter = operands.begin(); operandIter!=operands.end(); operandIter++)
            {
                llvm::outs()<<"\noperand "<<operandIter->format(Dyninst::Arch_x86_64);

            }


            /*for(curRegReadIter = regsRead.begin();
                    curRegReadIter!= regsRead.end();
                    curRegReadIter++ )
            {
                llvm::outs()<<"\n"<<(*curRegReadIter)->format()<<"\n";
                std::vector<Dyninst::InstructionAPI::InstructionAST::Ptr> uses;
                (*curRegReadIter)->getChildren(uses);
                for(auto useIter = uses.begin(); useIter!=uses.end(); useIter++)
                {
                    llvm::outs()<<"\t\tchild:"<<(*useIter)->format()<<" use \n";
                }
            }*/


            if (insn->readsMemory() || insn->writesMemory()) {
                insns_access_memory++;
                // check if it is
            }



            if(insn->readsMemory())
            {
                std::set<Dyninst::InstructionAPI::Expression::Ptr> memAccess;
                insn->getMemoryReadOperands(memAccess);
                for(auto memAccIter = memAccess.begin(); memAccIter!=memAccess.end(); memAccIter++)
                {
                    llvm::outs()<<"\n\tread memory:";
                    llvm::outs()<<(*memAccIter)->format()<<"\n";
                }
            }
            else if(insn->writesMemory())
            {
                std::set<Dyninst::InstructionAPI::Expression::Ptr> memAccess;
                insn->getMemoryWriteOperands(memAccess);
                for(auto memAccIter = memAccess.begin(); memAccIter!=memAccess.end(); memAccIter++)
                {
                    llvm::outs()<<"\n\twrite memory:";
                    llvm::outs()<<(*memAccIter)->format()<<"\n";
                }
            }

            if(insn->getControlFlowTarget())
            {
                llvm::outs()<<"\ntarget: "<<insn->getControlFlowTarget()->format()<<"\n";
            }

            llvm::outs()<<"\n";

        }
    }
    llvm::outs()<<"\n";
    llvm::outs()<<insns_access_memory<<" memory accesses\n";
    llvm::outs()<<totalNumInst<<" instructions\n";
}

int main(int argc, char* argv[] ) {
    const char *progName = argv[1]; // = ...
    int progPID = 42; // = ...
    const char *progArgv[] = {"InterestingProgram", "-h", NULL}; // = ...
    // Example 1: create/attach/open a binary
    BPatch_addressSpace *app = startInstrumenting(open, // or attach or open
        progName,
        progPID,
        progArgv);
    buildLLVM llvmBuilder;
    llvm::Module* go = llvmBuilder.makeLLVMModule();
    iterateThroughFunction(app, argv[2],go);
    llvm::outs()<<"\n";
    llvmBuilder.dumpModuleContent(go,llvm::outs());
    // Example 2: get entry point
    return 0;
}
