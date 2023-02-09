#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 2048
#define MAX_CODE_LENGTH 512
#define LINE_LENGTH 64
#define NUM_OPCODES 27
#define OPCODE_LENGTH 5

typedef struct {
    int op; /* opcode */
    int m; /* M */
} instruction;

void print_instructions(int PC, int instruction_end_index, instruction * InstructionMemory, char OPCODE_NAMES[NUM_OPCODES][OPCODE_LENGTH]);
void initial_state(int PC, int instruction_end_index, instruction * InstructionMemory, char OPCODE_NAMES[NUM_OPCODES][OPCODE_LENGTH]);
int print_trace(instruction Instruction, int PC, int BP, int SP, char OPCODE_NAMES[NUM_OPCODES][OPCODE_LENGTH], int * stack, int NDB);
void print_stack_state(int SP, int BP, int * stack);
void final_print(int PC, int BP, int SP,int * stack);
void machine(FILE *fp);

int main (int argc, char *argv[])
{
    FILE *textfile;
    textfile = fopen(argv[1], "r");
    machine(textfile);
    fclose(textfile);
    return 0;
}

void machine(FILE *fp) {

    // Initialize all the values of the stack
    // and the instruction memory to 0
    instruction InstructionMemory[MAX_CODE_LENGTH];

    int stack[MAX_STACK_HEIGHT] = {0};

    // Manual declaration of all instruction opcodes
    char OPCODE_NAMES[27][5] = {{"LIT"}, {"RTN"}, {"CAL"}, {"POP"}, 
                                {"PSI"}, {"PRM"}, {"STO"}, {"INC"}, 
                                {"JMP"}, {"JPC"}, {"CHO"}, {"CHI"}, 
                                {"HLT"}, {"NDB"}, {"NEG"}, {"ADD"}, 
                                {"SUB"}, {"MUL"}, {"DIV"}, {"MOD"},
                                {"EQL"}, {"NEQ"}, {"LSS"}, {"LEQ"}, 
                                {"GTR"}, {"GEQ"}, {"PSP"}};
    
    int read = 0;
    int instruction_end_index = 0;
    int i;


    
    for(i = 0; i < LINE_LENGTH; i++) {
        
        instruction newInstruction;
        
        read = fscanf(fp, "%d %d", &newInstruction.op, &newInstruction.m);
        InstructionMemory[i] = newInstruction;
        if (read != 2) {
            instruction_end_index = i;
            break;
        }
    }

    int PC = 0;
    int BP = 0;
    int SP = 0;
    
    // Print the instructions
    print_instructions(PC, instruction_end_index, InstructionMemory, OPCODE_NAMES);
    


    int NDB = 0;
    printf("Tracing ...\n");

    print_trace(InstructionMemory[0], PC, BP, SP, OPCODE_NAMES, stack, NDB);
    while (1) {
        
         // Invariant check
            if (!(0 <= BP) && !(BP <= SP) && !(0 <= SP) && !(SP < MAX_STACK_HEIGHT))
            {   
                printf("Invariants violated!\n");
                exit(1);
            }
            
            if(!(0 <= PC && PC < MAX_CODE_LENGTH))
            {   
                 if((SP - 1) < 0)
                {
                    printf("Trying to pop an empty stack!\n");
                    exit(1);
                }
                
                printf("Invariants violated!" );
                exit(1);
            }
        
        switch (InstructionMemory[PC].op) {

            // LIT
            case 1:
                stack[SP] = InstructionMemory[PC].m;
                SP++;
                break;

            // RTN
            case 2:
                PC = stack[SP - 1];
                BP = stack[SP - 2];
                SP -= 2; 
                break;

            // CAL
            case 3:
                stack[SP] = BP;
                stack[SP + 1] = PC;
                BP = SP;
                SP += 2;
                PC = InstructionMemory[PC].m;
                PC--;
                break;
            
            // POP
            case 4:
                SP = SP - 1;
                if(SP < 0)
                {
                    printf("Trying to pop an empty stack!\n");
                    exit(1);
                }
                break;

            // PSI
            case 5:
                stack[SP - 1] = stack[stack[SP - 1]];
                break;

            // PRM
            case 6:
                stack[SP] = stack[BP - InstructionMemory[PC].m];
                SP++;
                break;
                
            // STO
            case 7:
                stack[stack[SP - 1] + InstructionMemory[PC].m] = stack[SP - 2];
                SP -= 2;
                break;

            // INC
            case 8:
                SP += InstructionMemory[PC].m;
                break;

            // JMP
            case 9:
                PC = stack[SP - 1];
                SP--;
                PC--;
                break;

            // JPC  
            case 10:
                if(stack[SP - 1] != 0) {
                    PC = InstructionMemory[PC].m;
                    PC--;
                }
                    
                SP--;
                break;
                
            // CHO 
            case 11:
                putc(stack[SP-=1],stdout);
                SP--;
                break;
                
            // CHI
            case 12:
                stack[SP] = getc(stdin); 
                SP--;
                break;

            // HLT
            case 13:
                if(NDB==0)
                    final_print(PC+=1,  BP,  SP, stack);
                return;
                break;

            // NDB
            case 14:
                NDB = 1;
                break;

            // NEG
            case 15:
                stack[SP - 1] = -1 * stack[SP - 1];
                break;

            // ADD
            case 16:
                stack[SP - 2] = stack[SP - 1] + stack[SP - 2];
                SP--;
                break;

            // SUB
            case 17:
                stack[SP - 2] = stack[SP - 1] - stack[SP - 2];
                SP--;
                break;
            
            // MUL
            case 18:
                stack[SP - 2] = stack[SP - 1] * stack[SP - 2];
                SP--; 
                break;
            
            // DIV
            case 19:
                if(stack[SP - 2] == 0)
                {
                    printf("Divisor is zero in DIV instruction!\n");
                    exit(1); 
                }
                stack[SP - 2] = stack[SP - 1] / stack[SP - 2];
                SP--;
            
                break;
            
            // MOD
            case 20:
                if (stack[SP - 2] == 0)
                {
                    printf("Modulus is zero in MOD instruction!\n");
                    exit(1);
                }
                stack[SP - 2] = stack[SP - 1] % stack[SP - 2];
                SP--;
                break;

            // EQL
            case 21:
                stack[SP - 2] = stack[SP - 1] == stack[SP - 2];
                SP--;
                break;
            
            // NEQ
            case 22:
                stack[SP - 2] = (stack[SP - 1] != stack[SP - 2]);
                SP--;
                break;

            // LSS
            case 23:
                stack[SP - 2] = stack[SP - 1] < stack[SP - 2];
                SP--;
                break;

            // LEQ
            case 24:

                stack[SP - 2] = stack[SP - 1] <= stack[SP - 2];
                SP--;
                break;
            
            // GTR
            case 25:
                stack[SP - 2] = stack[SP - 1] > stack[SP - 2];
                SP--;
                break;
            
            // GEQ
            case 26:
                stack[SP - 2] = stack[SP - 1] >= stack[SP - 2];
                SP--;
                break;
            
            // PSP
            case 27:
                stack[SP] = SP;
                SP++;
                break;
            
            default:
                exit(1);
                break;
        }

        if(SP < 0){
                printf("Trying to pop an empty stack!\n");
                exit(1);
            }

        PC++;
        if (NDB == 0)
            print_trace(InstructionMemory[PC], PC, BP, SP, OPCODE_NAMES, stack, NDB);
        
    }
    
}


void print_instructions(int PC, int instruction_end_index, instruction * InstructionMemory, char OPCODE_NAMES[NUM_OPCODES][OPCODE_LENGTH]) {

    printf("Addr\tOP\tM\n");
    for (int i = 0; i < instruction_end_index; i++)
        printf("%d\t%s\t%d\n", i, OPCODE_NAMES[InstructionMemory[i].op - 1], InstructionMemory[i].m);
        
}

void print_stack_state(int SP, int BP, int * stack) {
    printf("stack: ");
    for (int i = BP; i < SP; i++)
        printf("S[%d]: %d ", i, stack[i]);
    printf("\n");
}

int print_trace(instruction Instruction, int PC, int BP, int SP, char OPCODE_NAMES[NUM_OPCODES][OPCODE_LENGTH], int * stack, int NDB) {
    
    printf("PC: %d BP: %d SP: %d\n", PC, BP, SP);
    print_stack_state(SP, BP, stack);
    printf("==> addr: %d \t%s   %d\n", PC, OPCODE_NAMES[Instruction.op - 1], Instruction.m);
    
    return 0;
}

void final_print(int PC, int BP, int SP,int * stack){
    printf("PC: %d BP: %d SP: %d\n", PC, BP, SP);
    print_stack_state(SP, BP, stack);
    exit(0);
}


