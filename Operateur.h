#define cle 314
#define cle2 318
#define IFLAGS (SEMPERM | IPC_CREAT)
#define SKEY   (key_t) IPC_PRIVATE
#define SEMPERM 0600				  /* Permission */

typedef struct
{
  long type;
  int nbrMachine; 
} messageMachine;

int msgid_op;
