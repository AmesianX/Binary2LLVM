//		rdi: 0x94f17	rsi: 0xc608000	rbp: 0xfffffffffff6bb0d	rsp: 0x7ffcda2c5a18	rbx: 0xfffffffffffff5e7	rdx: 0xd4	
//		rcx: 0x7f9958143010	rax: 0xd2	
//		r8: 0xfffffffff9e3e000	r9: 0x6304000	r10: 0xd6ce	r11: 0x6304000	r12: 0xc	r13: 0x758	r14: 0xfffffffff3c7c000	r15: 0x750
//RSP = 0x7ffcda2c5a18	

/*** here the partition performed takes into account the stack variables ***/		
void func0(long R13,long R15,long RCX,long RSP, long* STACK, 
            // read port
            int* PARTRAX, 
            void* PARTR8, 
            void* PARTRDI, 
            void* PARTR10RAX,
            void* PARTR9RAX, 
            void* PARTR12RAX, 
            void* PARTRDIRAX, 
            void* PARTRBPRAX,
            void* R8RAXPART, 
            // read write port below
            void* PARTRDXRAX, 
            void* PARTRSIRAX,
            void* PARTRCXRAX)
{
	bool CF;
	int EAX;
	bool OF;
	bool PF;
	long R10;
	int R11D;
	long R12;
	int R14D;
	long R8;
	long R9;
	int R9D;
	long RAX;
	long RBP;
	long RBX;
	long RDI;
	long RDX;
	long RIP;
	long RSI;
	bool SF;
	double XMM0;
	double XMM1;
	double XMM2;
	double XMM3;
	double XMM4;
	double XMM5;
	double XMM6;
	double XMM7;
	double XMM8;
	bool ZF;
	bool AF;
	long R14;
/**** MEM Reg        value         offset range         : port pointer *****/
/**** RSP         0x7ffff15b95d8   -0x68 -- 0x8         : STACK        *****/
/**** RAX@BB10638 0x663a70               0              : PARTRAX      *****/ 
/**** R8@BB10641  0x7fdc541fede8  +0x758*(0..209)+8     : PARTR8       *****/
/**** RDI@BB10641 0x7fdc4defade8  +0x758*(0..209)+8     : PARTRDI      *****/
/**** +RSI: 0x6bdc8 per outer ***/

/**** R10@BB10642 0x7fdc5426abb0  RAX =(0..209)*8+8     : PARTR10RAX   *****/
/**** +R13@BB10639: 0x758   per outer ***/
/**** +RSI@BB10636: 0x6bdc8 per outer -- through stack -0x60 ***/

/**** R9@BB10642  0x7fdc4defb540  RAX =(0..209)*8+8     : PARTR9RAX    *****/
/**** +R13@BB10639: 0x758   per outer ***/
/**** +RSI@BB10636: 0x6bdc8 per outer -- through stack -0x60 ***/


/**** R12@BB10642 0x7fdc5a502de8  RAX =(0..209)*8+8     : PARTR12RAX   *****/
/**** +R13@BB10639: 0x758   per outer -- thourgh RDI ****/
/**** +RSI@BB10636: 0x6bdc8 per outer -- through stack -0x60 ***/

/**** RDI@BB10642 0x7fdc4defade8  RAX =(0..209)*8+0x10  : PARTRDIRAX   *****/
/**** +R13@BB10639: 0x758   per outer ****/
/**** +RSI@BB10636: 0x6bdc8 per outer -- through stack -0x60 ***/

/**** RBP@BB10642 0x7fdc5a56ebb0  RAX =(0..209)*8+8     : PARTRBPRAX   *****/
/**** +R13@BB10639: 0x758   per outer -- thourgh R10 ****/
/**** +RSI@BB10636: 0x6bdc8 per outer -- through stack -0x60 ***/

/**** R8@BB10642  0x7fdc541fede8  RAX =(0..209)*8+0x10  : R8RAXPART    *****/
/**** +R13@BB10639: 0x758   per outer -- thourgh R10 ****/
/**** +RSI@BB10636: 0x6bdc8 per outer -- through stack -0x60 ***/

//=============================all read above ==============================//

/**** RDX@BB10642 0x7fdc6cb7fda8  RAX =(0..209)*8+0x8   : PARTRDXRAX   *****/
/**** RSI@BB10642 0x7fdc669bdda8  RAX =(0..209)*8+0x8   : PARTRSIRAX   *****/
/**** RCX@BB10642 0x7fdc607fbda8  RAX =(0..209)*8+0x8   : PARTRCXRAX   *****/
/** all three +R15@BB10639: 0x750 per outer ***/
/** all three +RDX@BB10636: 0x6af20 -- through stack -0x50 ***/


BB10637:
    EAX = *(STACK+RSP-0x30)             
	ZF = (EAX==EAX== 0);
	SF = (EAX < 0);
	if(ZF||SF) 
	    goto BB10636;
	else 
	    goto BB10638;
BB10636:
	*(STACK+RSP-0x44) += 1;     	    
	RDX = *(STACK+RSP-0x38);	    
	RSI = *(STACK+RSP-0x40);
	*(STACK+RSP-0x50) += RDX;    
	*(STACK+RSP-0x60) += RSI;
	R9D=*(STACK+RSP-0x2c);
	ZF = (*(STACK+RSP-0x44) == R9D);    
	if(!ZF) 
	    goto BB10637;
	else 
	    goto BB10634;
BB10638:
	RAX=*(STACK+RSP-0x20);
	R10=*(STACK+RSP-0x28);
	R14D=2;
	R9=*(STACK+RSP-0x60);
	RSI=*(STACK+RSP-0x18);
	RCX=*(STACK+RSP-0x10);
	R8 =*(STACK+RSP-0x8);
	R10+=*(STACK+RSP-0x60);
	RSI+=*(STACK+RSP-0x50);    
	RCX+=*(STACK+RSP-0x50);
	R8+=*(STACK+RSP-0x60);
	R9 = R9+R13;
	R11D = *(PARTRAX+RAX);              //RAX = 0x663a70; R11D = d2
	RDI= *(STACK+RSP-0x60);
	RDX = *(STACK+RSP-0x50);
BB10640:
	ZF = (R11D==R11D== 0);
	SF = (R11D < 0);
	if(SF || ZF) 
	    goto BB10639;
	else 
	    goto BB10641;
BB10639:
    R14D = R14D+1;
	RDX = RDX+R15;
	R10 = R10+R13;
	R9 = R9+R13;
	RSI = RSI+R15;
	RDI = RDI+R13;
	RCX = RCX+R15;
	R8 = R8+R13;
	ZF = (R14D==*(STACK+RSP-0x48)); //stack val = 212
	if(!ZF) 
	    goto BB10640;
	else 
	    goto BB10636;
BB10641:
    R12=*(STACK+RSP-0x68);
	RBP=*(STACK+RSP-0x58);
	RAX=RAX^RAX;
	RBX = *(STACK+RSP-0x68);
	XMM1=*(PARTR8+R8+8)
	XMM3=*(PARTRDI+RDI + 8);
	R12 = R12+RDI;
	RBP = RBP+R10;
	RBX = RBX+R9;
	goto BB10642;
BB10642:
    XMM2=XMM3;
	XMM0= *(PARTR10RAX + R10 + RAX * 8 + 8);
	XMM2 = XMM2-*(PARTR9RAX+R9 + RAX * 8 + 8);    
	XMM4=*(PARTR12RAX+R12 + RAX * 8 + 8);    
	XMM0 = XMM0-XMM1;
	XMM5=*(PARTRDIRAX+RDI + RAX * 8 + 10);
	XMM0 = XMM0*XMM8;
	XMM2 = XMM2*XMM7;
	XMM0 = XMM0+XMM2;
	XMM2=XMM4;
	XMM2 = XMM2-*(PARTRBPRAX+RBP + RAX * 8 + 8);
	XMM0 = XMM0+*(PARTRDXRAX+RDX + RAX * 8 + 8);
	XMM2 = XMM2*XMM8;
	*(PARTRDXRAX+RDX + RAX * 8 + 8)=XMM0;
	XMM0=XMM5;
	XMM0 = XMM0-XMM3;
	XMM0 = XMM0*XMM6;
	XMM0 = XMM0+XMM2;
	XMM2=*(R8RAXPART+R8 + RAX * 8 + 0x10);
	XMM1 = XMM1-XMM2;
	XMM0 = XMM0+*(PARTRSIRAX + RSI + RAX * 8 + 8);
	XMM1 = XMM1*XMM6;
	*(PARTRSIRAX+RSI + RAX * 8 + 8)=XMM0;
	XMM0=*(RBXRAXPART+RBX + RAX * 8 + 8);
	XMM0 = XMM0-XMM4;
	XMM0 = XMM0*XMM7;
	XMM0 = XMM0+XMM1;
	XMM0 = XMM0+*(PARTRCXRAX+RCX + RAX * 8 + 8);
	*(PARTRCXRAX+RCX + RAX * 8 + 8)=XMM0;
	RAX = RAX+1;
	ZF = (R11D == RAX); // R11D= 210
	if(!ZF) 
	    goto BB10643;
	else 
	    goto BB10639;
BB10643:
	XMM3=XMM5;	    
	XMM1=XMM2;
	goto BB10642;
}
