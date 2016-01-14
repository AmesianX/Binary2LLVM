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



typedef Dyninst::InstructionAPI::Result_Type usedRegType;

class RegisterTrans{
public:
    static std::string regType2CStr(usedRegType rt)
    {
        switch(rt)
        {

        case usedRegType::s8:
            return "signed char";
        case usedRegType::u8:
            return "uint8_t";
        case usedRegType::s16:
            return "short";
        case usedRegType::u16:
            return "uint16_t";
        case usedRegType::s32:
            return "int";
        case usedRegType::u32:
            return "uint32_t";
        case usedRegType::s64:
            return "long";
        case usedRegType::u64:
            return "uint64_t";
        case usedRegType::sp_float:
            return "float";
        case usedRegType::dp_float:
            return "double";
        case usedRegType::bit_flag:
            return "char";
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

    static usedRegType getRegisterType(std::string regName,Dyninst::InstructionAPI::Instruction::Ptr& insn  )
    {
        const char* charArray = regName.c_str();
        char first = charArray[0];
        char last = charArray[regName.size()-1];
        if(first=='R')
        {
            return usedRegType::s64;
        }
        if(first=='E')
        {
            return usedRegType::s32;
        }
        if(last=='H' || last=='L')
        {
            return usedRegType::s16;
        }
        if(last=='F')
        {
            return usedRegType::bit_flag;
        }
        if(regName.substr(0,2)=="XM")
        {
            return usedRegType::sp_float;
        }
        assert(false && "cannot translate to regtype");

    }

};

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
