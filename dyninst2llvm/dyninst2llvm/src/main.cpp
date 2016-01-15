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
#include <boost/program_options.hpp>
#include <fstream>
namespace poption = boost::program_options;
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



static std::ostream* contentOut;
static std::ostream* messageOut;
static std::ostream& contents()
{
    return *contentOut;
}
static std::ostream& message()
{
    return *messageOut;
}

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

static std::string getBBName(BPatch_basicBlock* curBB)
{
    std::string curName="BB";
    curName+=boost::lexical_cast<std::string>(curBB->getBlockNumber());

    return curName;
}


void basicBlockExperiments(BPatch_basicBlock* curBB)
{
    std::string curBBName = getBBName(curBB);
    message()<<"generating C for basic block "<<curBBName<<"\n";
    contents()<<curBBName<<":\n";
    std::vector<Instruction::Ptr> insns;
    curBB->getInstructions(insns);
    std::vector<Instruction::Ptr>::iterator insn_iter;
    for (insn_iter = insns.begin(); insn_iter != insns.end(); ++insn_iter)
    {
        Instruction::Ptr insn = *insn_iter;
        contents()<<"\t"<<insn->format()<<"\t:\t";
        std::set<Dyninst::InstructionAPI::RegisterAST::Ptr> regsRead;
        insn->getReadSet( regsRead);
        std::set<Dyninst::InstructionAPI::RegisterAST::Ptr>::iterator curRegReadIter;
        contents()<<"register read: ";
        for(curRegReadIter = regsRead.begin();
                curRegReadIter!= regsRead.end();
                curRegReadIter++ )
        {
            contents()<<(*curRegReadIter)->format()<<" ";
        }

        contents()<<"\t\t";
        std::set<Dyninst::InstructionAPI::RegisterAST::Ptr> regsWrite;
        insn->getWriteSet( regsWrite);
        std::set<Dyninst::InstructionAPI::RegisterAST::Ptr>::iterator curRegWriteIter;
        contents()<<"register write: ";
        for(curRegWriteIter = regsWrite.begin();
                curRegWriteIter!= regsWrite.end();
                curRegWriteIter++ )
        {
            contents()<<(*curRegWriteIter)->format()<<" ";
        }
        contents()<<"\n";
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
    std::vector<BPatch_edge*> allOutEdges;
    curBB->getOutgoingEdges(allOutEdges);
    contents()<<"\t";
    for(auto edgeIter = allOutEdges.begin();
        edgeIter!=allOutEdges.end();
        edgeIter++)
    {
        BPatch_edge* curOutEdge = *edgeIter;
        BPatch_edgeType ety = curOutEdge->getType();
        BPatch_basicBlock* dest = curOutEdge->getTarget();
        contents()<<ety<<":"<<getBBName(dest)<<";";
    }
    contents()<<"\n";


}



void headBasicGraphExperiments(BPatch_basicBlock* curHead,
                               BPatch_basicBlockLoop* containerLoop,
                               std::set<std::string>& seenBlocks,
                               std::set<std::string>& outBlocks)
{
    std::string curHeadName = getBBName(curHead);
    if(!containerLoop->hasBlock(curHead))
    {
        outBlocks.insert(curHeadName);
        return;
    }
    if(seenBlocks.count(curHeadName))
    {
        return;
    }
    seenBlocks.insert(curHeadName);

    basicBlockExperiments(curHead);


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
        headBasicGraphExperiments(nextHop,containerLoop,seenBlocks,outBlocks);
        destinationNames.push_back(getBBName(nextHop));
        destinationTypes.push_back(ety);
    }


    /*for(auto targetIter = allTarget.begin();
        targetIter!=allTarget.end();
        targetIter++)
    {
        BPatch_basicBlock* nextHop = *targetIter;
        destinationNames.push_back( headBasicGraphExperiments(nextHop,containerLoop,seenBlocks));

    }*/
    /*std::cout<<"BasicBlock "<<curHeadName<< " branches to : ";
    auto destNameIter = destinationNames.begin();
    auto destTypeIter = destinationTypes.begin();
    for(;
        destNameIter!=destinationNames.end();
        destNameIter++, destTypeIter++)
    {
        std::cout<<*destNameIter<<":";
        std::cout<<*destTypeIter;
        std::cout<<"\t";
    }*/
    //std::cout<<"\n";

}
void insertRegisterASTStr(std::set<RegisterAST::Ptr>& insRegs,
                          std::set<std::string>& usedRegsName)
{
    for(auto regIter = insRegs.begin(); regIter!=insRegs.end(); regIter++ )
    {
        RegisterAST::Ptr curInsReg = *regIter;
        usedRegsName.insert(uniformRegName( curInsReg->format()));
    }
}

void collectUsedRegs(std::vector<BPatch_basicBlock*>& allBBs,
                     std::set<std::string>& readRegsName,
                     std::set<std::string>& writeRegsName
                     )
{
    for(auto bbIter = allBBs.begin(); bbIter!=allBBs.end(); bbIter++)
    {
        BPatch_basicBlock* curBB = *bbIter;
        std::vector<Instruction::Ptr> insns;
        curBB->getInstructions(insns);
        std::vector<Instruction::Ptr>::iterator insn_iter;
        for (insn_iter = insns.begin(); insn_iter != insns.end(); ++insn_iter)
        {
            Instruction::Ptr insn = *insn_iter;
            std::set<RegisterAST::Ptr> regsRead;
            insn->getReadSet(regsRead);
            insertRegisterASTStr(regsRead,readRegsName);

            std::set<RegisterAST::Ptr> regsWrite;
            insn->getWriteSet(regsWrite);
            insertRegisterASTStr(regsWrite,writeRegsName);
        }
    }
}

void loopExperiments(BPatch_basicBlockLoop* curLoop)
{
    std::vector<BPatch_basicBlock*> allBBs;
    curLoop->getLoopBasicBlocks(allBBs);
    message()<<" convert Loop with "<<allBBs.size()<<" basic blocks\n";
    // dump all the basicblock seq num
    for(auto bbIter = allBBs.begin(); bbIter!=allBBs.end(); bbIter++)
    {
        BPatch_basicBlock* curBB = *bbIter;
        message()<<curBB->getBlockNumber()<<" ";
    }
    message()<<"\n";

    std::vector<BPatch_basicBlock*> loopEntries;
    curLoop->getLoopEntries(loopEntries);

    assert(loopEntries.size()>0 && "no loop entry found");
    BPatch_basicBlock* lEntry = loopEntries.at(0);
    // first look at registers that are live at this point
    // these will be the input
    BPatch_point* ept = lEntry->findEntryPoint();
    std::vector<BPatch_register> inLiveRegs;
    ept->getLiveRegisters(inLiveRegs);
    //FIXME: got to make an alternative llvm?
    std::string funcDecl = makeCFunctionDecl(inLiveRegs);
    contents()<<funcDecl<<"\n{\n";

    message()<<"loopEntry "<<getBBName(lEntry)<<"\n";
    std::set<std::string> seenBlocks;
    std::set<std::string> outBlocks;
    curLoop->getLoopBasicBlocks(allBBs);
    // for each basicblock check the used registers
    std::set<std::string> usedRegsRead;
    std::set<std::string> usedRegsWrite;
    collectUsedRegs(allBBs,usedRegsRead,usedRegsWrite);

    std::string regDecl = makeRegCDecl(inLiveRegs,usedRegsRead,usedRegsWrite);

    message()<<"read registers "<<"\n";
    for(auto regStrIter = usedRegsRead.begin();regStrIter!=usedRegsRead.end(); regStrIter++)
    {
        message()<<*regStrIter<<"\t";
    }
    headBasicGraphExperiments(lEntry,curLoop,seenBlocks,outBlocks);

    contents()<<"}\n";
    message()<<"loop done, press any key to continue\n";
    getchar();


}

// make a function to try out
// various features in the APIs
void experiments(BPatch_addressSpace * app,
                 std::string functionName)
{

    char *nameArr = new char[functionName.length() + 1]; // or
    std::strcpy(nameArr, functionName.c_str());
    BPatch_function* curFun = extractFunction(app, nameArr);
    BPatch_flowGraph *fg = curFun->getCFG();
    std::vector<BPatch_basicBlockLoop*> outerLoops;
    fg->getOuterLoops(outerLoops);
    message()<<"Num of outer loops: "<<outerLoops.size()<<"\n";
    message()<<"Press any key to continue\n";
    getchar();
    for(auto outerLoopIter = outerLoops.begin(); outerLoopIter!= outerLoops.end();
        outerLoopIter++)
    {
        loopExperiments(*outerLoopIter);
    }
    delete [] nameArr;


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
    poption::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("prog", poption::value<std::string>(), "name of the program")
        ("proid",poption::value<int>(),"process id")
        ("funname",poption::value<std::string>(),"function name")
        ("ofile",poption::value<std::string>(),"output file name")
    ;
    poption::variables_map vm;
    poption::store(poption::parse_command_line(argc, argv, desc), vm);
    poption::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }
    assert((vm.count("prog")||vm.count("proid"))&& "supply program name using -prog or process id using -procid");
    assert(vm.count("funname") && "function name is not specified");
    accessType_t curAccessType;
    const char *progName;
    int progPID;
    if (vm.count("prog")) {
        curAccessType = open;
        progName = vm["prog"].as<std::string>().c_str();
    } else {
        curAccessType = attach;
        progPID =  vm["procid"].as<int>();
    }
    ofstream file;
    if(vm.count("ofile"))
    {
        std::string outputFileName = vm["ofile"].as<std::string>();
        file.open(outputFileName);
    }
    if(file.is_open())
        contentOut = &file;
    else
        contentOut = &std::cout;

    // standard out for message
    messageOut = &std::cout;

    const char *progArgv[] = {"InterestingProgram", "-h", NULL}; // = ...
    message()<<"starting\n";

    BPatch_addressSpace *app = startInstrumenting(curAccessType, // or attach or open
        progName,
        progPID,
        progArgv);
    for(auto regIter = app->getRegisters_begin(); regIter!=app->getRegisters_end(); regIter++)
    {
        message()<<regIter->name();
        message()<<"\t";
    }
    message()<<"\n";
    std::string funcName = vm["funname"].as<std::string>();
    /*
        buildLLVM llvmBuilder;
        llvm::Module* go = llvmBuilder.makeLLVMModule();
        iterateThroughFunction(app, argv[2],go);
        llvm::outs()<<"\n";
        llvmBuilder.dumpModuleContent(go,llvm::outs());*/



    experiments(app,funcName);
    return 0;
}
