#ifndef BUILDC_H
#define BUILDC_H

#include <algorithm>
#include "BPatch.h"
#include "BPatch_addressSpace.h"
#include "BPatch_process.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "BPatch_flowGraph.h"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string/case_conv.hpp"

using namespace Dyninst::InstructionAPI;


class RegisterTrans{
public:
    static std::string regType2CStr(Result_Type rt)
    {
        switch(rt)
        {

        case Result_Type::s8:
            return "signed char";
        case Result_Type::u8:
            return "uint8_t";
        case Result_Type::s16:
            return "short";
        case Result_Type::u16:
            return "uint16_t";
        case Result_Type::s32:
            return "int";
        case Result_Type::u32:
            return "uint32_t";
        case Result_Type::s64:
            return "long";
        case Result_Type::u64:
            return "uint64_t";
        case Result_Type::sp_float:
            return "float";
        case Result_Type::dp_float:
            return "double";
        case Result_Type::bit_flag:
            return "bool";
        // 48-bit pointers...yay Intel
        //m512,
        //dbl128,
        //m14
        //s48,
        //u48,
        default:
            assert("unsupported type");
        }
    }

    static Result_Type getRegisterType(std::string regName)
    {
        const char* charArray = regName.c_str();
        char first = charArray[0];
        char last = charArray[regName.size()-1];
        if(first=='R' )
        {
            if(last=='D')
                return Result_Type::s32;
            if(last=='W')
                return Result_Type::s16;
            if(last=='B')
                return Result_Type::s8;
            return Result_Type::s64;
        }
        if(first=='E')
        {
            return Result_Type::s32;
        }
        if(last=='H' || last=='L')
        {
            return Result_Type::s16;
        }
        if(last=='F')
        {
            return Result_Type::bit_flag;
        }
        if(regName.substr(0,2)=="XM")
        {
            return Result_Type::dp_float;
        }

        assert(false && "cannot translate to regtype");


    }


    static Result_Type getRegisterType(std::string regName,Instruction::Ptr& insn  )
    {
        const char* charArray = regName.c_str();
        char first = charArray[0];
        char last = charArray[regName.size()-1];
        if(first=='R')
        {
            return Result_Type::s64;
        }
        if(first=='E')
        {
            return Result_Type::s32;
        }
        if(last=='H' || last=='L')
        {
            return Result_Type::s16;
        }
        if(last=='F')
        {
            return Result_Type::bit_flag;
        }
        if(regName.substr(0,2)=="XM")
        {
            return Result_Type::sp_float;
        }
        assert(false && "cannot translate to regtype");

    }

};


class InstructionTrans{

    static std::string dumpMemExpSet(std::set<Expression::Ptr>& allmem )
    {
        std::string rtStr = "*(";
        for(auto piter = allmem.begin(); piter!= allmem.end(); piter++)
        {
            Expression::Ptr curp = *piter;

            rtStr += curp->format();
        }
        rtStr += ")";
        return rtStr;
    }
    static std::string translateOperand(Operand op)
    {
        std::string rtStr = "";
        bool mem=false;
        if(op.readsMemory())
        {
            std::set<Expression::Ptr> readMemExp;
            op.addEffectiveReadAddresses(readMemExp);
            rtStr += dumpMemExpSet(readMemExp);
            mem = true;
        }

        else if(op.writesMemory())
        {
            std::set<Expression::Ptr> writeMemExp;
            op.addEffectiveWriteAddresses(writeMemExp);
            rtStr+= dumpMemExpSet(writeMemExp);
            mem=true;
        }
        if(!mem)
            rtStr+=op.format(Dyninst::Arch_x86_64);
        return rtStr;

    }

public:
    static std::string translate2C(Instruction::Ptr& insn,
                                   std::vector<std::string>& bbNames,
                                   std::vector<BPatch_edgeType>& edgeTypes
                                   )
    {
        //Expression::Ptr controlFlowTarget
        //if(insn->getControlFlowTarget())

        std::string rtStr="";
        // do all the multiplex things
        const Operation& curOp = insn->getOperation();
        std::vector<Operand> curInsnOperands;
        insn->getOperands(curInsnOperands);

        std::vector<std::string> rightHand;
        std::vector<std::string> leftHand;

        for(auto operandIter = curInsnOperands.begin(); operandIter!= curInsnOperands.end(); operandIter++)
        {
            Operand curOperand= *operandIter;
            std::string opStr = translateOperand(curOperand);
            if(curOperand.isRead())
            {
                rightHand.push_back(opStr);
            }
            if(curOperand.isWritten())
            {
                leftHand.push_back(opStr);
            }

            std::cout<< curOperand.format(Dyninst::Arch_x86_64)<<" == "<<opStr<<"\n";
        }
        entryID curOpId = curOp.getID();
        std::string takenDest = "";
        std::string nonTakenDest = "";

        switch(curOpId)
        {
        case entryID::e_mov:
        case entryID::e_lea:
        case entryID::e_movsd:
        case entryID::e_movsd_sse:
        case entryID::e_movapd:

            rtStr+=leftHand[0];
            rtStr+="=";
            rtStr+=rightHand[0];
            rtStr+=";\n";
            break;
        case entryID::e_cmp:
            rtStr+= "SF = ";
            rtStr+=rightHand[0];
            rtStr+=" <= ";
            rtStr+=rightHand[1];
            rtStr+=";\n";
            rtStr+= "\tZF = ";
            rtStr+=rightHand[0];
            rtStr+=" == ";
            rtStr+=rightHand[1];
            rtStr+=";\n";
            break;
        case entryID::e_test:
            rtStr+= "ZF = (";
            rtStr+= rightHand[0];
            rtStr+= "==";
            rtStr+= rightHand[1];
            rtStr+= "== 0);\n";
            rtStr+= "\tSF = ";
            rtStr+=rightHand[0];
            rtStr+=" < 0 && ";
            rtStr+=rightHand[1];
            rtStr+= "< 0 ";
            rtStr+=";\n";
            break;
        case entryID::e_jle:
        case entryID::e_jnz:
        case entryID::e_jz:

            assert(bbNames.size()==2 && edgeTypes.size()==2 && "branch with no valid successors");
            if(edgeTypes.at(0) == CondJumpTaken)
            {
                takenDest+=bbNames[0];
                nonTakenDest+=bbNames[1];
            }
            else
            {
                takenDest+=bbNames[1];
                nonTakenDest+=bbNames[0];
            }
            if(curOpId == entryID::e_jle)
                rtStr+= "if(SF)";
            else if(curOpId == entryID::e_jnz)
                rtStr+= "if(!ZF)";
            else
                rtStr+= "if(ZF)";
            rtStr+= " goto ";
            rtStr += takenDest;
            rtStr += ";\n\telse goto ";
            rtStr += nonTakenDest;
            rtStr +=";\n";
            break;
        case entryID::e_sub:            
        case entryID::e_subsd:
            rtStr += leftHand[0];
            rtStr += " = ";
            rtStr += rightHand[0];
            rtStr += "-";
            rtStr += rightHand[1];
            rtStr +=";\n";
            break;
        case entryID::e_addsd:
        case entryID::e_add:
            rtStr += leftHand[0];
            rtStr += " = ";
            rtStr += rightHand[0];
            rtStr += "+";
            rtStr += rightHand[1];
            rtStr +=";\n";
            break;
        case entryID::e_mulsd:
            rtStr += leftHand[0];
            rtStr += " = ";
            rtStr += rightHand[0];
            rtStr += "*";
            rtStr += rightHand[1];
            rtStr +=";\n";
            break;
        case entryID::e_xor:
            rtStr += leftHand[0];
            rtStr += " = ";
            rtStr += rightHand[0];
            rtStr += "^";
            rtStr += rightHand[1];
            rtStr += ";\n";
            break;
        case entryID::e_jmp:
            rtStr += "goto ";
            rtStr += bbNames[0];
            rtStr += "\n;";
            break;
        case entryID::e_nop:
            break;
        default:
            rtStr += "not implemented\n";
        }
        return rtStr;
    }
};


// make every body lower case
static std::string uniformRegName(std::string originalRegName)
{
    std::string rtStr = originalRegName;
    boost::to_upper(rtStr);
    return rtStr;
}

static int funcCounter=0;


static std::string makeCFunctionDecl(std::set<std::string>& inLiveRegs
                                     )
{
    // all live registers are input params
    std::string decl = "void func";
    decl+= boost::lexical_cast<std::string>(funcCounter);

    funcCounter++;
    decl+="(";
    for(auto liveRegIter = inLiveRegs.begin();
        liveRegIter!= inLiveRegs.end();
        )
    {

        std::string regName = *liveRegIter;
        Result_Type curRegType = RegisterTrans::getRegisterType(regName);
        std::string typeStr = RegisterTrans::regType2CStr(curRegType);
        decl+= typeStr;
        decl+=" ";
        decl+=regName;

        if(inLiveRegs.end()== ++liveRegIter)
        {
            decl+=")";
            break;
        }
        else
            decl+=",";
    }

    return decl;

}


static std::string makeRegCDecl(std::set<std::string>&inLiveRegs,
                                std::set<std::string>& usedRegsRead,
                                std::set<std::string>& usedRegsWrite)
{

    std::string regDecl="";
    for(auto regReadIter = usedRegsRead.begin();
        regReadIter!=usedRegsRead.end();
        regReadIter++)
    {
        std::string regReadName = *regReadIter;
        if(inLiveRegs.count(regReadName))
            continue;
        Result_Type curRegType = RegisterTrans::getRegisterType(regReadName);
        std::string typeStr = RegisterTrans::regType2CStr(curRegType);
        regDecl+="\t"+typeStr+" "+regReadName+";\n";
    }
    for(auto regWriteIter = usedRegsWrite.begin();
        regWriteIter!=usedRegsWrite.end();
        regWriteIter++)
    {
        std::string regWriteName = *regWriteIter;
        if(inLiveRegs.count(regWriteName) || usedRegsRead.count(regWriteName))
            continue;
        Result_Type curRegType = RegisterTrans::getRegisterType(regWriteName);
        std::string typeStr = RegisterTrans::regType2CStr(curRegType);
        regDecl+="\t"+typeStr+" "+regWriteName+";\n";

    }

    return regDecl;
}






// we have raw registers -- these are the full registers
// can be floating point
struct BinaryRegisterRaw
{
    std::string name;
};
struct BinaryRegisterUsed
{
    std::string name;
    struct BinaryRegisterRaw* container;

};

/*
struct  RegInBin
{
    std::string name;
    RegisterType myType;

    std::string containerName;

    RegInBin(std::string name_  )
    {
        name = name_;
        myType = RegisterTrans::getRegisterType(name_);
        containerName = "";
    }
    bool isStrictSubReg()
    {
        return (myType==RegisterTypeInt16 || myType==RegisterTypeInt32);
    }

    std::string getContainerName()
    {
        if(containerName.size()==0 &&  isStrictSubReg())
        {
            const char* charArray = name.c_str();
            std::string cutLastLetter = name.substr(0,name.size()-1);
            std::string curFirstLetter = name.substr(1,name.size());

            if(charArray[0]=='R')
            {
                containerName+=cutLastLetter;

            }
            else if(charArray[0]=='E')
            {
                std::string finalContainer = "R";
                finalContainer+=curFirstLetter;
                containerName+=finalContainer;
            }
            else if(charArray[name.size()-1]=='H' || charArray[name.size()-1]=='L')
            {
                std::string finalContainer = "R";
                if(cutLastLetter.size()==1)
                    cutLastLetter+="X";
                finalContainer+=cutLastLetter;
                containerName+=finalContainer;
            }
        }
        if(containerName.size()==0){
            containerName += name;
        }
        return containerName;
    }
};
*/





// the basic thing here, we declare all the
// registers in the beginning -- all the registers
// that can potentially be live in any basic blocks
// then, just do simple translation of instructions
// the function arguments = live registers at the
// beginning of the function
class buildCFunction{
public:
    buildCFunction(BPatch_function* m):func(m)
    {
    }
    std::string makeFunctionDecl();
private:
    BPatch_function* func;
};


class buildCLoop{
public:
    buildCLoop(BPatch_basicBlockLoop* l):loop(l)
    {
    }
    std::string makeLoopFuncDecl();
private:
    BPatch_basicBlockLoop* loop;
};


#endif // BUILDC_H
