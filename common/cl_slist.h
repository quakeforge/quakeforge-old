#include <quakeio.h>
#define MAX_SERVER_LIST 256

typedef struct {
	char *server;
	char *description;
	int ping;
} server_entry_t;

extern server_entry_t	slist[MAX_SERVER_LIST];

void Server_List_Init(void);
void Server_List_Shutdown(void);
int Server_List_Set(int i,char *addr,char *desc);
int Server_List_Load(QFile *f);
char *gettokstart (char *str, int req, char delim);
int gettoklen(char *str, int req, char delim);
