#include "BPatch.h"
#include "BPatch_addressSpace.h"
#include "BPatch_process.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "BPatch_flowGraph.h"
#include <string>
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
 char* functionName)
{
    std::vector<BPatch_function *> functions;
    BPatch_image *appImage = app->getImage();
    appImage->findFunction(functionName, functions);
    BPatch_flowGraph *fg = functions[0]->getCFG();
    std::set<BPatch_basicBlock *> blocks;
    fg->getAllBasicBlocks(blocks);
    std::set<BPatch_basicBlock *>::iterator block_iter;
    int insns_access_memory=0;
    for (block_iter = blocks.begin(); block_iter != blocks.end(); ++block_iter)
    {
        BPatch_basicBlock *block = *block_iter;
        std::vector<Dyninst::InstructionAPI::Instruction::Ptr> insns;
        block->getInstructions(insns);
        std::vector<Dyninst::InstructionAPI::Instruction::Ptr>::iterator insn_iter;
        for (insn_iter = insns.begin(); insn_iter != insns.end(); ++insn_iter)
        {
            Dyninst::InstructionAPI::Instruction::Ptr insn = *insn_iter;
            std::cout<<insn->format()<<endl;
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr> regsRead;
            insn->getReadSet( regsRead);
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr>::iterator curRegReadIter;
            std::cout<<"register read: ";
            for(curRegReadIter = regsRead.begin(); 
                    curRegReadIter!= regsRead.end();
                    curRegReadIter++ )
            {
                std::cout<<(*curRegReadIter)->format()<<" ";                
            }
            std::cout<<"\n";
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr> regsWrite;
            insn->getWriteSet( regsWrite);
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr>::iterator curRegWriteIter;
            std::cout<<"register write: ";
            for(curRegWriteIter = regsWrite.begin(); 
                    curRegWriteIter!= regsWrite.end();
                    curRegWriteIter++ )
            {
                std::cout<<(*curRegWriteIter)->format()<<" ";                
            }
               
            std::cout<<"\n";
            /*if (insn->readsMemory() || insn->writesMemory()) {
                insns_access_memory++;
            }*/
        }
    }
    //std::cout<<insns_access_memory<<" memory access\n";
    
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
    iterateThroughFunction(app, argv[2]);
    // Example 2: get entry point
    return 0;
}
