/***************************************************************************//**
  @file         myShell.c
  @author       Apostolou Orestis
  @date         Thursday,  1 February 2018
*******************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define EXEC 1
#define LIST 2
#define SLIST 3

//define commands as structures so that we can handle ; and &&
struct cmd{
	int type;     // EXEC, LIST or SLIST
};

struct execcmd{
	int type;              // EXEC
	char *argv[256];
};

struct listcmd{
	int type;             // LIST
	struct cmd *left;
	struct cmd *right;
};

struct seriallistcmd{
	int type;            //SLIST
	struct cmd *left;
	struct cmd *right; 
};

int fork1(void);
void runcmd(struct cmd *cmd);
struct cmd *parsecmd(char*);

                                             // BUILT IN FUNCTIONS//
                                             // BUILT IN FUNCTIONS//
                                             // BUILT IN FUNCTIONS//



int checkIfBuilt(char *buf,int nbuf){
	char whitespace[] = " \t\r\n\v";
	int num;
	int p,i,flag;
	char buf2[256];
	char str_cd[3];
	char str_cd2[3];
	char str_quit[5];
	char str_quit2[5];
	flag=0;
	memcpy(buf2,buf,nbuf);
	memcpy(str_cd,"cd ",3);
	memcpy(str_cd2,"cd\n",3);
	memcpy(str_quit,"quit\n",5);
	memcpy(str_quit2,"quit ",5);
	num=1;
	if (nbuf>=3){
		p=0;
		while (0==memcmp(&buf2[p]," ",1)&&p<nbuf+1){
				p++;
			} 
		if (0==memcmp(&buf2[p],str_cd,3)||0==memcmp(&buf2[p],str_cd2,3)){
			num=1;
			p+=3;
			while (0==memcmp(&buf2[p]," ",1)&&p<nbuf+1){
				p++;
			}
			while (0==memcmp(&buf2[p],"\n",1)&&p<nbuf+1){
				p++;
			}
			if (nbuf==p+1||nbuf==p){
				chdir(getenv("HOME"));
			}
			else{
				buf2[strlen(buf2)-1] = 0;  // chop \n
				for (i=p;i<strlen(buf2);i++){
					if (0==memcmp(&buf2[i],";",1)){
						if (0==flag)
							flag=i+1;
					}
				}
				if (flag!=0){
					for (i=strlen(buf2);i>flag-2;i--){
						buf2[i]=0;
					}
					while (0==memcmp(&buf2[strlen(buf2)-1]," ",1)||(0==memcmp(&buf2[strlen(buf2)-1],"\n",1))){
					buf2[strlen(buf2)-1] = 0;
				}

				}
				while (0==memcmp(&buf2[strlen(buf2)-1]," ",1)||(0==memcmp(&buf2[strlen(buf2)-1],"\n",1))){
					buf2[strlen(buf2)-1] = 0;
				}
				if (chdir(&buf2[p])!=0){
					if (strcmp(&buf2[p]," ")==0||strcmp(&buf2[p],"\n")){
						chdir(getenv("HOME"));
					}
					else
					printf("No such directory as %s\n",&buf2[p]);
				}

			}
		}
		if (nbuf>=5){
			if(0==memcmp(buf,str_quit,5)||0==memcmp(buf,str_quit2,5)){
				printf("Goodbye!\n");
				num=0;
			}
		}
	}
	return num;
}

                                             // END OF BUILT IN FUNCTIONS//
                                             // END OF BUILT IN FUNCTIONS//
                                             // END OF BUILT IN FUNCTIONS//




int getcmd(char *buf, int nbuf){
  memset(buf, 0, nbuf);
  if(fgets(buf, nbuf, stdin) == 0)
    return -1; // EOF
  return 0;

}

                                             // MAIN FUNCTIONS//
                                             // MAIN FUNCTIONS//
                                             // MAIN FUNCTIONS//



void myShellLoop(void){
	static char line[512];
	int r;
	int status=1;
	int exit_status=1;
	do {
		exit_status=1;
		printf("Apostolou_8587>");
		if (getcmd(line,sizeof(line))<0){
			printf("Error in getting the command\n");
			exit(-1);
		}

		exit_status=checkIfBuilt(line,strlen(line));          //built functions: cd and quit
		if (0!=exit_status){
			if (fork1()==0){
				runcmd(parsecmd(line));
			}
			wait(&r);
		}

		//status=checkType(line);


	}while(status && exit_status>0);
}


void batchShell(char *arg){
	static char line[512];
	int r;
	int status=1;
	int exit_status=1;
	FILE *myfile;
	if ((myfile=fopen(arg,"r"))==0){
		printf("The file \"%s\" does not exist\n",arg);
		exit(-1);
	}
	while (fgets(line,512,myfile)&&1==exit_status){
		exit_status=1;
		//printf("%s",line);
		exit_status=checkIfBuilt(line,strlen(line));          //built functions: cd and quit
		if (0!=exit_status&&strcmp(line,"\n")!=0){
			if (fork1()==0){
				runcmd(parsecmd(line));
			}
			wait(&r);
		}
	}

	fclose(myfile);


}

int main(int argc, char **argv){
	if (1==argc){
		myShellLoop();
	}
	else if(2==argc){
		batchShell(argv[1]);
	}
	else
		printf("Usage: \"%s batchfile\", or just \"%s\"\n",argv[0],argv[0]);
	return 0;
}

int fork1(void){
	int pid;
	pid=fork();
	if (pid<0){
		printf("error in creating the proccess\n");
		exit(-1);
	}
	return pid;
}

                                             // END OF MAIN FUNCTIONS//
                                             // END OF MAIN FUNCTIONS//
                                             // END OF MAIN FUNCTIONS//





void runcmd(struct cmd *cmd){
	int r,c,*status;
	int child;
	struct execcmd *ecmd;
	struct listcmd *lcmd;
	struct seriallistcmd *slcmd;

	if (0==cmd){
		printf("No command found\n");
		exit(-1);

	}

	switch(cmd->type){
		default:
		   printf("Runcmd problem\n");
		   exit(-1);
		   break;

		case EXEC:
		   ecmd=(struct execcmd*)cmd;
		   if (0==ecmd->argv[0]){
		   	  printf("Empty command error\n");
		   	  exit(-1);
		   }
		   if(execvp(ecmd->argv[0], ecmd->argv)==-1){
		   	if (strcmp(ecmd->argv[0],"cd")!=0){                   //used for commands like cd ..;ls
		      printf("No such command as \"");
		      c=0;
		      while (ecmd->argv[c]!=NULL){
		      	 printf("%s ",ecmd->argv[c]);
		      	 c++;
		      }
		      printf("\"\n");
		      exit(-1);
		    }
		   }
		   break;

		case LIST:
		   lcmd= (struct listcmd*)cmd;
		   if(fork1()==0){
		      runcmd(lcmd->left);
		   }
		   wait(&r);
		   runcmd(lcmd->right);
		   break;

		case SLIST:
		   slcmd=(struct seriallistcmd*)cmd;
		   if (child=fork1()==0){
		   	   runcmd(slcmd->left);
		   }
		   waitpid(child,status,0);
		   if (0==*status){
		   	   runcmd(slcmd->right);
		   }
		   break;

	}
	_exit(0);
}

struct cmd* execcmd(void){
	struct execcmd *cmd;
	cmd=malloc(sizeof(*cmd));
	memset(cmd,0,sizeof(*cmd));
	cmd->type=EXEC;
	return (struct cmd*)cmd;
}

struct cmd* listcmd(struct cmd *left,struct cmd *right){
	struct listcmd *cmd;
	cmd=malloc(sizeof(*cmd));
	memset(cmd,0,sizeof(*cmd));
	cmd->type=LIST;
	cmd->left=left;
	cmd->right=right;
	return (struct cmd*)cmd;
}

struct cmd* seriallistcmd(struct cmd *left,struct cmd *right){
	struct seriallistcmd *cmd;
	cmd=malloc(sizeof(*cmd));
	memset(cmd,0,sizeof(*cmd));
	cmd->type=SLIST;
	cmd->left=left;
	cmd->right=right;
	return (struct cmd*)cmd;
}

                                            /** PARSING **/
                                            /** PARSING **/
                                            /** PARSING **/


char whitespace[] = " \t\r\n\v";
char symbols[] = ";";
char combo[]="&&";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '&':
    s++;
    if(*s='&'){
    	s++;
    }
    else{
    	s--;
    	ret='a';
    }
  case ';':
    s++;
    break;
  case '>':
  case '<':
  case '|':
    s++;
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s) )
      {	
      s++;
      }
    break;
  }
  if(eq)
    *eq = s;
  
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  //search a character//
  char *s;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

int
peek2(char **ps,char *es, char *toks)
{
	//search a whole expression//
	char *s;

	s=*ps;
	while(s < es && strchr(whitespace, *s))
       s++;
    *ps=s;
   return *s && strchr(toks,*s) && strchr (toks,*(s+1));
}

struct cmd *parseline(char**, char*);
struct cmd *parseexec(char**, char*);

char 
*mkcopy(char *s, char *es)
{
  int n = es - s;
  char *c = malloc(n+1);
  assert(c);
  strncpy(c, s, n);
  c[n] = 0;
  return c;
}

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    fprintf(stderr, "leftovers: %s\n", s);
    exit(-1);
  }
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps,es,";")){
  	gettoken(ps,es,0,0);
  	cmd=listcmd(cmd,parseline(ps,es));
  }
  else if(peek2(ps,es,"&")){
  	gettoken(ps,es,0,0);
  	cmd=seriallistcmd(cmd,parseline(ps,es));
  }
  return cmd;
}


struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  int sum=0;
  struct execcmd *cmd;
  struct cmd *ret;
  
  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = (struct cmd*)cmd;
  while(!peek(ps, es, ";")&&(!peek2(ps,es,"&&"))){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a') {
      fprintf(stderr, "This operation is not supported\n");
      exit(-1);
    }
    cmd->argv[argc] = mkcopy(q, eq);
    sum+=strlen(cmd->argv[argc])+1;
    argc++;
    if(argc >= 256) {
      fprintf(stderr, "too many args\n");
      exit(-1);
    }
    ret = (struct cmd*)cmd;
  }
  if (sum>513){
  	printf("Input must not exceed 512 characters\n");
  }
  cmd->argv[argc] = 0;
  /** use this to check that ret indeed transfers the arguments **/
  //cmd = (struct execcmd*)ret;
  //printf("argv=%s\n",cmd->argv[1]);
  /** end of check **/
  return ret;
}

                                 // END OF PARSING //
                                 // END OF PARSING //
                                 // END OF PARSING //

