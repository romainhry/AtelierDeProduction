#define cle 314
#define IFLAGS (SEMPERM | IPC_CREAT)
#define SKEY   (key_t) IPC_PRIVATE
#define SEMPERM 0600				  /* Permission */

typedef struct
{
  long type;
  int nbrMachine;
  int pid;
} messageMachine;


//structure de données qui sera envoyée via une file de messages IPC
typedef struct
{
  long type;
  int typePiece;
  int nbrPiece;
} message;

int msgid_op;
