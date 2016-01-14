#include "BPatch.h"
#include "BPatch_addressSpace.h"
#include "BPatch_process.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "BPatch_flowGraph.h"
#include <string>
#include "buildLLVM.h"
#include "buildC.h"
#include <boost/lexical_cast.hpp>
using namespace Dyninst::InstructionAPI;
//typedef Dyninst::InstructionAPI::Expression insAPIExp;
//typedef Dyninst::InstructionAPI::Instruction insAPIIns;
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


BPatch_function* extractFunction(BPatch_addressSpace * app,
                     char* functionName)
{
    std::vector<BPatch_function *> functions;
    BPatch_image *appImage = app->getImage();
    appImage->findFunction(functionName, functions);
    if(functions.empty())
        return 0;
    else
        return functions[0];
}


void basicBlockExperiments(BPatch_basicBlock* curBB,
                           std::map<BPatch_basicBlock*,std::string>& seenBlocks)
{
    std::string curBBName = seenBlocks[curBB];
    std::cout<<"into basic block "<<curBBName<<"\n";


    std::vector<Instruction::Ptr> insns;
    curBB->getInstructions(insns);
    std::vector<Instruction::Ptr>::iterator insn_iter;
    for (insn_iter = insns.begin(); insn_iter != insns.end(); ++insn_iter)
    {
        Instruction::Ptr insn = *insn_iter;
        std::cout<<insn->format()<<"\t:\n";
        /*
        if(insn->readsMemory())
        {
            std::set<Expression::Ptr> allMemRead;
            insn->getMemoryReadOperands(allMemRead);
            for(auto allMemReadIter = allMemRead.begin(); allMemReadIter!=allMemRead.end();
                allMemReadIter++)
            {
                Expression::Ptr curMemReadExp = *allMemReadIter;
                std::cout<<curMemReadExp->format()<<"\n";

                std::vector<Expression::Ptr> allExpressionChildren;
                curMemReadExp->getChildren(allExpressionChildren);
                std::cout<<"\teval mem exp:" <<curMemReadExp->eval().format()<<"\n";
                for(auto childrenIter = allExpressionChildren.begin();
                    childrenIter!=allExpressionChildren.end();
                    childrenIter++)
                {
                    Expression::Ptr curChild = *childrenIter;
                    std::cout<<"\t\t"<<curChild->format()<<"\n";
                    std::cout<<"\t\t"<<curChild->eval().format()<<"\n";
                }
            }
        }*/
    }


}


static std::string getBBName(BPatch_basicBlock* curBB)
{
    std::string curName="BB";
    curName+=boost::lexical_cast<std::string>(curBB->getBlockNumber());

    return curName;
}

std::string headBasicGraphExperiments(BPatch_basicBlock* curHead,
                               BPatch_basicBlockLoop* containerLoop,
                               std::map<BPatch_basicBlock*,std::string>& seenBlocks)
{
    if(!containerLoop->hasBlock(curHead))
        return "";
    if(seenBlocks.count(curHead))
    {
        return seenBlocks[curHead];
    }
    std::string curBBName = getBBName(curHead);
    seenBlocks[curHead]=curBBName;



    // then branch to all target
    std::vector<BPatch_basicBlock*> allTarget;
    curHead->getTargets(allTarget);
    std::vector<BPatch_edge*> allOutEdges;
    curHead->getOutgoingEdges(allOutEdges);
    std::vector<BPatch_edgeType> destinationTypes;
    std::vector<std::string> destinationNames;
    for(auto edgeIter = allOutEdges.begin();
        edgeIter!=allOutEdges.end();
        edgeIter++)
    {
        BPatch_edge* curOutEdge = *edgeIter;
        BPatch_edgeType ety = curOutEdge->getType();
        BPatch_basicBlock* nextHop = curOutEdge->getTarget();
        destinationNames.push_back( headBasicGraphExperiments(nextHop,containerLoop,seenBlocks));
        destinationTypes.push_back(ety);
    }


    /*for(auto targetIter = allTarget.begin();
        targetIter!=allTarget.end();
        targetIter++)
    {
        BPatch_basicBlock* nextHop = *targetIter;
        destinationNames.push_back( headBasicGraphExperiments(nextHop,containerLoop,seenBlocks));

    }*/
    std::cout<<"BasicBlock "<<curBBName<< " branches to : ";
    auto destNameIter = destinationNames.begin();
    auto destTypeIter = destinationTypes.begin();
    for(;
        destNameIter!=destinationNames.end();
        destNameIter++, destTypeIter++)
    {
        std::cout<<*destNameIter<<":";
        std::cout<<*destTypeIter;
        std::cout<<"\t";
    }
    std::cout<<"\n";
    // now deal with this block itself
    basicBlockExperiments(curHead,seenBlocks);
    return  seenBlocks[curHead];

}

void loopExperiments(BPatch_basicBlockLoop* curLoop)
{
    std::vector<BPatch_basicBlock*> loopEntries;
    curLoop->getLoopEntries(loopEntries);
    assert(loopEntries.size()>0 && "no loop entry found");
    BPatch_basicBlock* lEntry = loopEntries.at(0);
    // from this point, we do depth first search
    // for all the basicblocks in the loop
    std::map<BPatch_basicBlock*,std::string> seenBlocks;
    headBasicGraphExperiments(lEntry,curLoop,seenBlocks);
    //std::vector<BPatch_basicBlock*> allBBs;
    //curLoop->getLoopBasicBlocks(allBBs);
    // for each of the instruction, check the result
    // type, and check the register


}

// make a function to try out
// various features in the APIs
void experiments(BPatch_addressSpace * app,
                 char* functionName)
{
    BPatch_function* curFun = extractFunction(app, functionName);
    BPatch_flowGraph *fg = curFun->getCFG();
    //fg->getAllBasicBlocks(blocks);
    std::vector<BPatch_basicBlockLoop*> outerLoops;
    fg->getOuterLoops(outerLoops);
    std::cout<<"Num of outer loops: "<<outerLoops.size()<<"\n";
    getchar();
    for(auto outerLoopIter = outerLoops.begin(); outerLoopIter!= outerLoops.end();
        outerLoopIter++)
    {
        loopExperiments(*outerLoopIter);
        getchar();
    }


}




void makeCFunction(BPatch_addressSpace * app,
                   char* functionName)
{
    llvm::outs()<<"(convert 2 C function:)checking out function with name "<<functionName<<"\n";
    std::vector<BPatch_function *> functions;
    BPatch_image *appImage = app->getImage();
    appImage->findFunction(functionName, functions);
    if(functions.empty())
        return;

    buildCFunction bc(functions[0]);
    bc.makeFunctionDecl();
}

/*
void iterateThroughFunction(BPatch_addressSpace * app,
 char* functionName, llvm::Module* go)
{
    llvm::outs()<<"checking out function with name "<<functionName<<"\n";
    std::vector<BPatch_function *> functions;
    BPatch_image *appImage = app->getImage();
    appImage->findFunction(functionName, functions);
    if(functions.empty())
        return;
    bin2llvmFunction b2f(functions[0],go);
    llvm::Function* funcCreated = b2f.makeFunction();
    BPatch_flowGraph *fg = functions[0]->getCFG();
    std::set<BPatch_basicBlock *> blocks;
    fg->getAllBasicBlocks(blocks);
    std::set<BPatch_basicBlock *>::iterator block_iter;


    int insns_access_memory=0;
    int totalNumInst = 0;
    // find all the points of function call
    llvm::outs()<<"try to find fnctl\n";
    std::vector<BPatch_point*>* allCallPoint = functions[0]->findPoint(BPatch_subroutine);

    std::map<BPatch_point*,Dyninst::InstructionAPI::Instruction::Ptr> pt2ins;

    for(auto callPtIter = allCallPoint->begin(); callPtIter!= allCallPoint->end(); callPtIter++)
    {
        //BPatch_point* curPt = *callPtIter;

        Dyninst::InstructionAPI::Instruction::Ptr curInsPtr = (*callPtIter)->getInsnAtPoint();
        if(curInsPtr == 0)
        {
            llvm::outs()<<"not found\n";
            void* insAddr = (*callPtIter)->getAddress();
            llvm::outs()<<insAddr<<"\n";
        }
        else
        {
            pt2ins[*callPtIter] = curInsPtr;
            llvm::outs()<<curInsPtr->format()<<"\n";
        }
        if((*callPtIter)->isDynamic())
        {
            llvm::outs()<<"dynamic call site, skip\n";
            continue;
        }
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
        // we create a new basic block here



        std::vector<Dyninst::InstructionAPI::Instruction::Ptr> insns;
        block->getInstructions(insns);
        std::vector<Dyninst::InstructionAPI::Instruction::Ptr>::iterator insn_iter;
        for (insn_iter = insns.begin(); insn_iter != insns.end(); ++insn_iter)
        {
            Dyninst::InstructionAPI::Instruction::Ptr insn = *insn_iter;
            llvm::outs()<<insn->ptr()<<" "<<insn->format()<<"\t\t\t\t\t";
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
*/
int main(int argc, char* argv[] ) {
    const char *progName = argv[1]; // = ...
    int progPID = 42; // = ...
    const char *progArgv[] = {"InterestingProgram", "-h", NULL}; // = ...
    // Example 1: create/attach/open a binary
    BPatch_addressSpace *app = startInstrumenting(open, // or attach or open
        progName,
        progPID,
        progArgv);
    for(auto regIter = app->getRegisters_begin(); regIter!=app->getRegisters_end(); regIter++)
    {
        std::cout<<regIter->name();

    }
    //getchar();
    /*
    buildLLVM llvmBuilder;
    llvm::Module* go = llvmBuilder.makeLLVMModule();
    iterateThroughFunction(app, argv[2],go);
    llvm::outs()<<"\n";
    llvmBuilder.dumpModuleContent(go,llvm::outs());*/

    // make the C module
    //makeCFunction(app,argv[2]);

    experiments(app,argv[2]);
    return 0;
}
