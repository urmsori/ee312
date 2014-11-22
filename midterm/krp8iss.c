#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "krp8iss.h"

bool loop(krpreg *reg, krpmem * mem);
bool run_once(krpreg *reg, krpmem *mem);
bool read_binary(FILE *fp, krpmem *mem);
bool dump(FILE *fp, krpmem *mem);

int main(int argc, char** argv)
{
  /* argument check */
  if(argc != 3){
    printf("Wrong argument\n");
    return 0;
  }

  /* initialize register, memory */
  krpreg reg;
  krpmem mem;
  krpreg_init(&reg);
  krpmem_init(&mem);


  /* open input file */
  FILE *inputfp = fopen(argv[1],"rb");
  if(inputfp == NULL){
    printf("Open input fail\n");
    return 0;
  }


  /* read binary file */
  if(read_binary(inputfp, &mem) == false){
    printf("file format error\n");
    fclose(inputfp);
    return 0;
  }
  fclose(inputfp);

  /* loop */
  if(loop(&reg, &mem) == false){
    printf("Fail\n");
    fclose(inputfp);
    return 0;
  }

  /* open dump file */
  FILE *dumpfp = fopen(argv[2],"w");
  if(dumpfp == NULL){
    printf("dump file open fail\n");
    return 0;
  }

  /* dump */
  dump(dumpfp, &mem);
  fclose(dumpfp);

  return 0;
}

/* Loop for user interface */
bool loop(krpreg *reg, krpmem * mem)
{
  char request=0;
  int bp = -1;
  while(1){
    /* get request */
    printf(">> ");
    request = getchar();
    while( getchar() != '\n');
    
    /* branch */
    if(request == 's'){
      if(!run_once(reg, mem))
	break;
      print_krpreg(reg);
    }
    else if(request == 'r'){
      if(bp >= 0){ /* breakpoint set */
	while(run_once(reg, mem))
	  {
	    if(bp>=0 && (reg->pc)>=(bp<<2)){
	      bp = -1;
	      break;
	    }
	  }
	/* no display */
      }
      else{ /* no breakpoint */
	/* display */
	while(run_once(reg, mem)){}
	print_krpreg(reg);
	break;
      }

    }
    else if(request == 'b'){
      printf("break point:");
      scanf("%d",&bp);
      while( getchar() != '\n');
    }
    else if(request == 'd'){
      print_krpreg(reg);
    }
    else if(request == 'q')
      break;
    else
      printf("Wrong request : %c\n",request);
  }

  return true;
}

/* Fetch and excute */
bool run_once(krpreg *reg, krpmem *mem)
{
  /* fetch */
  if(!krp_fetch(reg,mem))
    return false;

  /* excute */
  krp_excute(reg,mem);

  return true;
}

/* Read binary file and save to program memory */
bool read_binary(FILE *fp, krpmem *mem)
{
  if(fp == NULL)
    return false;

  uint8_t inputc=0;
  int loopi;
  for(loopi=0;1;loopi++)
    {
      /* getc */
      inputc = (uint8_t) fgetc(fp);
      if(feof(fp))
	break;

      /* save to program memory */
      mem->prog[loopi] = inputc;
    }

  if(loopi%4 != 0)
    return false;

  return true;
}

/* Dump data memory (Little endian) */
bool dump(FILE *fp, krpmem *mem)
{
  if(fp == NULL || mem == NULL)
    return false;
  
  uint32_t i;
  for(i=0; i<DMEM_NUM/4; i++){
    fprintf(fp, "0x%.8X : ", i*4);
    fprintf(fp, "%.2X ", mem->data[i*4]);
    fprintf(fp, "%.2X ", mem->data[i*4 + 1]);
    fprintf(fp, "%.2X ", mem->data[i*4 + 2]);
    fprintf(fp, "%.2X\n", mem->data[i*4 + 3]);
  }
}


/*-------krp8iss.h function---------*/
bool krp_fetch(krpreg *reg, krpmem *mem)
{
  if(reg == NULL || mem == NULL)
    return false;
  if(reg->pc > PMEM_NUM)
    return false;

  if( reg->delay_slot == 0xFFFFFFFF ){
    reg->ir = krpmem_getir(mem,reg->pc);
    reg->pc += 4;
  }
  else{
    /* apply delay slot */
    reg->ir = krpmem_getir(mem,reg->pc);
    if( reg->pc == reg->delay_slot )
      reg->pc += 4;
    else
      reg->pc = reg->delay_slot;
    reg->delay_slot = 0xFFFFFFFF;
  }

  if(reg->ir == 0)
    return false;

  return true;
}


bool krp_excute(krpreg *reg, krpmem *mem)
{
  /* decode opcode */
  uint32_t opcode = IR_OPCODE(reg->ir);
  uint32_t ra = IR_RA(reg->ir);
  uint32_t rb = IR_RB(reg->ir);
  uint32_t rc = IR_RC(reg->ir);
  //printf("opcode:%X\n",opcode);

  if(opcode == OP_ADDI1)
    {
      reg->gpr[ra] = reg->gpr[rb] + 
	signExt(IR_IMM17(reg->ir), 17);
    }
  else if(opcode == OP_ADDI2)
    {
      uint32_t imm = signExt(IR_IMM10(reg->ir), 10);
      if(IR_MODE(reg->ir) == 0)
	{
	  reg->gpr[ra] = reg->gpr[rb]+
	    shl(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 1)
	{
	  reg->gpr[ra] = reg->gpr[rb]+
	    lsr(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 2)
	{
	  reg->gpr[ra] = reg->gpr[rb]+
	    asr(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 3)
	{
	  reg->gpr[ra] = reg->gpr[rb]+
	    ror(imm, IR_SHAMT(reg->ir));
	}
    }
  else if(opcode == OP_ANDI1)
    {
      reg->gpr[ra] = reg->gpr[rb] & 
	signExt(IR_IMM17(reg->ir), 17);
    }
  else if(opcode == OP_ANDI2)
    {
      uint32_t imm = signExt(IR_IMM10(reg->ir), 10);
      if(IR_MODE(reg->ir) == 0)
	{
	  reg->gpr[ra] = reg->gpr[rb]&
	    shl(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 1)
	{
	  reg->gpr[ra] = reg->gpr[rb]&
	    lsr(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 2)
	{
	  reg->gpr[ra] = reg->gpr[rb]&
	    asr(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 3)
	{
	  reg->gpr[ra] = reg->gpr[rb]&
	    ror(imm, IR_SHAMT(reg->ir));
	}
    }
  else if(opcode == OP_ORI1)
    {
      reg->gpr[ra] = reg->gpr[rb] | 
	signExt(IR_IMM17(reg->ir), 17);
    }
  else if(opcode == OP_ORI2)
    {
      uint32_t imm = signExt(IR_IMM10(reg->ir), 10);
      if(IR_MODE(reg->ir) == 0)
	{
	  reg->gpr[ra] = reg->gpr[rb] |
	    shl(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 1)
	{
	  reg->gpr[ra] = reg->gpr[rb] |
	    lsr(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 2)
	{
	  reg->gpr[ra] = reg->gpr[rb] |
	    asr(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 3)
	{
	  reg->gpr[ra] = reg->gpr[rb] |
	    ror(imm, IR_SHAMT(reg->ir));
	}
    }
  else if(opcode == OP_MOVI1)
    {
      reg->gpr[ra] = signExt(IR_IMM17(reg->ir), 17);
    }
  else if(opcode == OP_MOVI2)
    {
      uint32_t imm = signExt(IR_IMM10(reg->ir), 10);
      if(IR_MODE(reg->ir) == 0)
	{
	  reg->gpr[ra] = shl(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 1)
	{
	  reg->gpr[ra] = lsr(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 2)
	{
	  reg->gpr[ra] = asr(imm, IR_SHAMT(reg->ir));
	}
      if(IR_MODE(reg->ir) == 3)
	{
	  reg->gpr[ra] = ror(imm, IR_SHAMT(reg->ir));
	}
    }
  else if(opcode == OP_ADD)
    {
      reg->gpr[ra] = reg->gpr[rb] + reg->gpr[rc];
    }
  else if(opcode == OP_SUB)
    {
      reg->gpr[ra] = reg->gpr[rb] - reg->gpr[rc];
    }
  else if(opcode == OP_NEG)
    {
      reg->gpr[ra] = -(reg->gpr[rc]);
    }
  else if(opcode == OP_NOT)
    {
      reg->gpr[ra] = ~(reg->gpr[rc]);
    }
  else if(opcode == OP_AND)
    {
      reg->gpr[ra] = reg->gpr[rb] & reg->gpr[rc];
    }
  else if(opcode == OP_OR)
    {
      reg->gpr[ra] = reg->gpr[rb] | reg->gpr[rc];
    }
  else if(opcode == OP_XOR)
    {
      reg->gpr[ra] = reg->gpr[rb] ^ reg->gpr[rc];
    }
  else if(opcode == OP_LSR)
    {
      uint32_t shamt = IR_SHAMT(reg->ir);
      uint32_t i = IR_I(reg->ir);
      if(i==0)
	reg->gpr[ra] = (reg->gpr[rb])>>shamt;
      else
	reg->gpr[ra] = (reg->gpr[rb])>>(reg->gpr[rc]);
    }
  else if(opcode == OP_ASR)
    {
      uint32_t shamt = IR_SHAMT(reg->ir);
      uint32_t i = IR_I(reg->ir);
      if(i==0)
	reg->gpr[ra] = signExt((reg->gpr[rb])>>shamt, 32-shamt);
      else
	reg->gpr[ra] = signExt((reg->gpr[rb])>>(reg->gpr[rc]), 32-(reg->gpr[rc]));
    }
  else if(opcode == OP_SHL)
    {
      uint32_t shamt = IR_SHAMT(reg->ir);
      uint32_t i = IR_I(reg->ir);
      if(i==0)
	reg->gpr[ra] = (reg->gpr[rb])<<shamt;
      else
	reg->gpr[ra] = (reg->gpr[rb])<<(reg->gpr[rc]);
    }
  else if(opcode == OP_ROR)
    {
      uint32_t shamt = IR_SHAMT(reg->ir);
      uint32_t i = IR_I(reg->ir);
      if(i==0)
	reg->gpr[ra] = ror((reg->gpr[rb]), shamt);
      else
	reg->gpr[ra] = ror(reg->gpr[rb], reg->gpr[rc]);
    }
  else if(opcode == OP_BR)
    {
      if(IR_COND(reg->ir) == 0) /* Never */
	{/* nothing */}
      else if(IR_COND(reg->ir) == 1) /* Always */
	{
	  reg->delay_slot = reg->gpr[rb];
	}
      else if(IR_COND(reg->ir) == 2) /* Zero */
	{
	  if(reg->gpr[rc] == 0)
	    reg->delay_slot = reg->gpr[rb];
	}
      else if(IR_COND(reg->ir) == 3) /* Nonzero */
	{
	  if(reg->gpr[rc] != 0)
	    reg->delay_slot = reg->gpr[rb];
	}
      else if(IR_COND(reg->ir) == 4) /* Plus */
	{
	  if((int32_t)reg->gpr[rc] >= 0)
	    reg->delay_slot = reg->gpr[rb];	  
	}
      else if(IR_COND(reg->ir) == 5) /* Minus */
	{
	  if((int32_t)reg->gpr[rc] < 0)
	    reg->delay_slot = reg->gpr[rb];
	}
    }
  else if(opcode == OP_BRL)
    {
      reg->gpr[ra] = reg->pc;
      if(IR_COND(reg->ir) == 0) /* Never */
	{/* nothing */}
      else if(IR_COND(reg->ir) == 1) /* Always */
	{
	  reg->delay_slot = reg->gpr[rb];
	}
      else if(IR_COND(reg->ir) == 2) /* Zero */
	{
	  if(reg->gpr[rc] == 0)
	    reg->delay_slot = reg->gpr[rb];
	}
      else if(IR_COND(reg->ir) == 3) /* Nonzero */
	{
	  if(reg->gpr[rc] != 0)
	    reg->delay_slot = reg->gpr[rb];
	}
      else if(IR_COND(reg->ir) == 4) /* Plus */
	{
	  if((int32_t)reg->gpr[rc] >= 0)
	    reg->delay_slot = reg->gpr[rb];	  
	}
      else if(IR_COND(reg->ir) == 5) /* Minus */
	{
	  if((int32_t)reg->gpr[rc] < 0)
	    reg->delay_slot = reg->gpr[rb];
	}
    }
  else if(opcode == OP_J)
    {
      reg->delay_slot = reg->pc + signExt(IR_IMM22(reg->ir), 22);
    }
  else if(opcode == OP_JL)
    {
      reg->gpr[ra] = reg->delay_slot;
      reg->delay_slot = reg->pc + signExt(IR_IMM22(reg->ir), 22);
    }
  else if(opcode == OP_LD)
    {
      if(reg->gpr[rb] == 0x1F)
	reg->gpr[ra] = krpmem_getdata(mem, zeroExt(IR_IMM17(reg->ir), 17));
      else
	reg->gpr[ra] = krpmem_getdata(mem, signExt(IR_IMM17(reg->ir), 17) + reg->gpr[rb]);
	
    }
  else if(opcode == OP_LDR)
    {
      reg->gpr[ra] = krpmem_getdata(mem, (reg->pc) + signExt(IR_IMM22(reg->ir), 22));
    }
  else if(opcode == OP_ST)
    {
      if(reg->gpr[rb] == 0x1F)
	krpmem_setdata(mem, zeroExt(IR_IMM17(reg->ir), 17), reg->gpr[ra]);
      else
	krpmem_setdata(mem, reg->gpr[rb] + signExt(IR_IMM17(reg->ir), 17) , reg->gpr[ra]);
    }
  else if(opcode == OP_STR)
    {
      krpmem_setdata(mem, reg->pc + signExt(IR_IMM22(reg->ir), 22), reg->gpr[ra]);
    }
  else if(opcode == OP_LEA)
    {
      reg->gpr[ra] = reg->pc + signExt(IR_IMM22(reg->ir), 22);
    }
  else if(opcode == OP_MSN)
    {
      reg->gpr[ra] = 20110343;
    }
  else
    return false;

  return true;
}

void krpreg_init(krpreg *reg)
{
  memset(reg, 0, sizeof(krpreg));
  reg->delay_slot = 0xFFFFFFFF;
  /*  int i;
  for(i=0; i<GPR_NUM; i++)
  reg->gpr[i] = i;*/
}

void print_krpreg(krpreg *reg)
{
  printf("PC:%.8X\n",(reg->pc)-4);
  printf("Inst:%.8X\n",reg->ir);
  int i;
  for(i=0; i<GPR_NUM; i++)
    {
      printf("reg[%d]:%d\t",i,reg->gpr[i]);
      if((i%4)==3)
	printf("\n");
    }
}

void krpmem_init(krpmem *mem)
{
  memset(mem, 0, sizeof(krpmem));
}

void print_krpmem(krpmem *mem)
{
  
}

/* Program memory -> uint32_t
 * Little endian -> Big endian 
 */
uint32_t krpmem_getir(krpmem *mem, uint32_t pc)
{
  uint32_t ret = 0;
  int i;
  for(i=0; i<4; i++){
    ret |= ((uint32_t)mem->prog[pc+i]) << 8*i;
  }
  return ret;
}

/* Data memory -> uint32_t
 * Little endian -> Big endian 
 */
uint32_t krpmem_getdata(krpmem *mem, uint32_t index)
{
  uint32_t ret = 0;
  int i;
  for(i=0; i<4; i++){
    ret |= ((uint32_t)mem->data[index+i]) << 8*i;
  }
  return ret;
}

/* Data memory <- uint32_t
 * Little endian <- Big endian 
 */
void krpmem_setdata(krpmem *mem, uint32_t index, uint32_t value)
{
  int i;
  for(i=0; i<4; i++){
    mem->data[index+i] = (uint8_t)(value & 0xFF);
    if(i!=3){
      value = value >> 8;
    }
  }
}


/* operations */
uint32_t signExt(uint32_t src, int len)
{
  if( (src & (1<<(len-1))) == 0 )
    return src;

  return src | ((0xFFFFFFFF >> len) << len);
}
uint32_t zeroExt(uint32_t src, int len)
{
  return src;
}
uint32_t shl(uint32_t src, int shamt)
{
  return src<<shamt;
}
uint32_t lsr(uint32_t src, int shamt)
{
  return src>>shamt;
}
uint32_t asr(uint32_t src, int shamt)
{
  return signExt(src>>shamt, 32-shamt);
}
uint32_t ror(uint32_t src, int shamt)
{
  return (src>>(shamt+1)) | (src<<(31-shamt));
}
