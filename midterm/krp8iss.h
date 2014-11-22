/* krp8iss.h */
#include <stdint.h>
#include <stdbool.h>

/* Instruction register parsing */
#define IR_OPCODE(A) ((0xF8000000 & (A)) >> 27)
#define IR_RA(A) ((0x7C00000 & (A)) >> 22)
#define IR_RB(A) ((0x3E0000 & (A)) >> 17)
#define IR_RC(A) ((0x1F000 & (A)) >> 12)
#define IR_IMM22(A) (0x3FFFFF & (A))
#define IR_IMM17(A) (0x1FFFF & (A))
#define IR_IMM10(A) ((0x1FF80 & (A)) >> 7)
#define IR_MODE(A) ((0x60 & (A)) >> 5)
#define IR_SHAMT(A) (0x1F & (A))
#define IR_I(A) ((0x20 & (A)) >> 5)
#define IR_COND(A) (0x7 & (A))

/* Opcode matching */
#define OP_ADDI1 0
#define OP_ADDI2 1
#define OP_ANDI1 2
#define OP_ANDI2 3
#define OP_ORI1 4
#define OP_ORI2 5
#define OP_MOVI1 6
#define OP_MOVI2 7
#define OP_ADD 8
#define OP_SUB 9
#define OP_NEG 10
#define OP_NOT 11
#define OP_AND 12
#define OP_OR 13
#define OP_XOR 14
#define OP_LSR 15
#define OP_ASR 16
#define OP_SHL 17
#define OP_ROR 18
#define OP_BR 19
#define OP_BRL 20
#define OP_J 21
#define OP_JL 22
#define OP_LD 23
#define OP_LDR 24
#define OP_ST 25
#define OP_STR 26
#define OP_LEA 27
#define OP_MSN 28

/* operations */
uint32_t signExt(uint32_t src, int len);
uint32_t zeroExt(uint32_t src, int len);
uint32_t shl(uint32_t src, int shamt);
uint32_t lsr(uint32_t src, int shamt);
uint32_t asr(uint32_t src, int shamt);
uint32_t ror(uint32_t src, int shamt);


/* Registers */
typedef struct krpreg
{
#define GPR_NUM 32
  uint32_t pc; /* Program counter */
  uint32_t delay_slot; /* PC for delay slot */
  uint32_t ir; /* Instruction register */
  uint32_t gpr[GPR_NUM]; /* General-purpose register */
} krpreg;

/* Memory */
typedef struct krpmem
{
#define PMEM_NUM 4096
#define DMEM_NUM 4096
  uint8_t prog[PMEM_NUM]; /* Program memory, 4KB */
  uint8_t data[DMEM_NUM]; /* Data memory, 4KB */
} krpmem;

bool krp_fetch(krpreg *reg, krpmem *mem);
bool krp_excute(krpreg *reg, krpmem *mem);

void krpreg_init(krpreg *reg);
void print_krpreg(krpreg *reg);

void krpmem_init(krpmem *mem);
void print_krpmem(krpmem *mem);

/* Memory <-> uint32_t
 * Little endian <-> Big endian 
 */
uint32_t krpmem_getir(krpmem *mem, uint32_t pc);
uint32_t krpmem_getdata(krpmem *mem, uint32_t index);
void krpmem_setdata(krpmem *mem, uint32_t index, uint32_t value);


