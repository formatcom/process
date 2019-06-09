#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
  pid_t cpid, w;
  int wstatus;
  unsigned char childexitcode;

  printf("parent %ld\n", (long)getpid());
  cpid = fork();
  if (cpid == -1)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (cpid == 0)
  { // codigo que ejecuta proceso hijo
    printf("child  %ld\n", (long)getpid());

    char* args[100];
    // simulando comando
    const char* cmd = "sleep 20"; 
    char* token;
    char delim[] = " ";

    char* ptr = strdup(cmd);

    if (ptr == NULL) {
      perror("ptr");
      exit(127);
    }

    printf("int command %s\n", cmd);
    token = strtok(ptr, delim);

    // parse comando a args
    int n = 0;
    while (token != NULL) { 
      if (n<100) args[n] = token;
      else break;
      token = strtok(NULL, delim);
      n++;
    }

    int i;
    printf("parse command ");
    for (i = 0; i < n; i++) {
      printf("%s ", args[i]);
    }
    printf("\n");

    int err = execv("/bin/busybox", args);
    if (err == -1) {
      perror("execv");
      if (errno == ENOENT) {
	exit(127);
      }else{
        exit(126);
      }
    }
  }

  // codigo que ejecuta el padre
  w = waitpid(cpid, &wstatus, 0);
  if (w == -1) {
    perror("waitpid");
    exit(EXIT_FAILURE);
  }

  if (WIFEXITED(wstatus)) {
    childexitcode = WEXITSTATUS(wstatus);
  }else if(WIFSIGNALED(wstatus)){
    childexitcode = 128;
    childexitcode += WTERMSIG(wstatus);
  }
  printf("pid %ld exited, status=%d\n",
	          (long)cpid, childexitcode);

  return 0;
}
