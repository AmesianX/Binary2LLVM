#include "buildC.h"


// find all the registers used in this set of basicblocks
/*void findUsedRegistersInBBs(std::set<RegInBin>& regDepo, std::vector<BPatch_basicBlock*>& allBBs)
{
    for(auto bbIter = allBBs.begin(); bbIter!= allBBs.end(); bbIter++)
    {
        BPatch_basicBlock* curBB = *bbIter;
        std::vector<Dyninst::InstructionAPI::Instruction::Ptr> insns;
        curBB->getInstructions(insns);
        std::vector<Dyninst::InstructionAPI::Instruction::Ptr>::iterator insn_iter;
        for (insn_iter = insns.begin(); insn_iter != insns.end(); ++insn_iter)
        {
            Dyninst::InstructionAPI::Instruction::Ptr insn = *insn_iter;
            std::cout<<insn->ptr()<<" "<<insn->format()<<"\t\t\t\t\t";
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr> regsRead;
            insn->getReadSet( regsRead);
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr>::iterator curRegReadIter;
            for(curRegReadIter = regsRead.begin();
                    curRegReadIter!= regsRead.end();
                    curRegReadIter++ )
            {
                Dyninst::InstructionAPI::RegisterAST::Ptr curReg = *curRegReadIter;
                RegInBin curRegBin(curReg->format());

            }
        }
    }

}
*/
void traverseLoopNest(BPatch_basicBlockLoop* curLevelLoop)
{


}



std::string buildCFunction::makeFunctionDecl()
{
    std::string funcDeclStr="void ";

    std::vector<BPatch_point*>* funcEntryPt = func->findPoint(BPatch_entry);
    assert(funcEntryPt->size() && "function should have one point of entry");
    std::vector<BPatch_register> originalLiveRegs;
    funcEntryPt->at(0)->getLiveRegisters(originalLiveRegs);
    std::string funcName = func->getName();


    std::set<std::string> argRegStr;
    std::set<std::string> extraDecRegStr;

    funcDeclStr+=funcName;



    BPatch_flowGraph *fg = func->getCFG();
    std::set<BPatch_basicBlock *> blocks;
    fg->getAllBasicBlocks(blocks);
    std::set<BPatch_basicBlock *>::iterator block_iter;
    std::vector<BPatch_basicBlockLoop*> outerLoops;
    fg->getOuterLoops(outerLoops);
    std::cout<<"Num of outer loops: "<<outerLoops.size()<<"\n";
    getchar();
    // this is a per loop approach
    for(auto outerLoopIter = outerLoops.begin();
        outerLoopIter!=outerLoops.end();
        outerLoopIter++)
    {
        BPatch_basicBlockLoop* curLoop = *outerLoopIter;
        std::vector<BPatch_basicBlock*> allEntries;
        curLoop->getLoopEntries(allEntries);
        std::cout<<"Num of entry bbs: "<<allEntries.size();
        getchar();

    }

    //
    for (block_iter = blocks.begin(); block_iter != blocks.end(); ++block_iter)
    {
        BPatch_basicBlock *block = *block_iter;


        std::vector<Dyninst::InstructionAPI::Instruction::Ptr> insns;
        block->getInstructions(insns);
        std::vector<Dyninst::InstructionAPI::Instruction::Ptr>::iterator insn_iter;
        for (insn_iter = insns.begin(); insn_iter != insns.end(); ++insn_iter)
        {
            Dyninst::InstructionAPI::Instruction::Ptr insn = *insn_iter;
            std::cout<<insn->format()<<"\t\t";
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr> regsRead;
            std::set<Dyninst::InstructionAPI::RegisterAST::Ptr> regsWrite;
            insn->getReadSet( regsRead);
            insn->getWriteSet( regsWrite);


            for(auto curRegReadIter = regsRead.begin();
                    curRegReadIter!= regsRead.end();
                    curRegReadIter++ )
            {
                Dyninst::InstructionAPI::RegisterAST::Ptr curReg = * curRegReadIter;
                std::cout<<curReg->format()<<" ";
                std::cout<<curReg->getID().name()<<" ";
            }

            std::cout<<"\t\t";
            std::cout<<"register write: ";
            for(auto curRegWriteIter = regsWrite.begin();
                    curRegWriteIter!= regsWrite.end();
                    curRegWriteIter++ )
            {

                std::cout<<(*curRegWriteIter)->format()<<" ";

            }
            std::cout<<"\n";
        }
        getchar();
    }



    return "lll";
}

