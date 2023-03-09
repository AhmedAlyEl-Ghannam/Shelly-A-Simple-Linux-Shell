#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <stdbool.h>

#define SIZE 1024

/*free memory after use*/
//void memoryHolocaust(char** argv,int size);

int exit_flag = 0;
pid_t id;
int background = 0;

/*INTRODUCTORY MESSAGE*/
void greetingsSpeSHELL1();


/*DIRECTORY STUFF*/
static char* currentDirectory;
void promptMSGShelly();


/**
 * SIGNAL HANDLERS
 */
// signal handler for SIGCHLD */
void signalHandler_CHLD(int p);
// signal handler for SIGINT
void signalHandler_INT(int p);


/**
 * Function used to initialize Shelly + related stuff
 */
// Shell pid, pgid, terminal modes
static pid_t shellyprocess_ID;
static pid_t shelly_processGP_ID;
static int shelly_isinteractive;
static struct termios shellyMODES;
struct sigaction act_CHLD;
struct sigaction act_INT;
void awakenFromThySlumberShelly();

/**
 * Function that reads, parses, and executes command
 * PS: I know the difference between executor and executioner ;)
 */
void executioner (void);

/**
 * Export shenanigans
 */
//int export_flag = 0;
//void *export variable;
//void *export_var;
//void exportation100(char **argv, void *export_var);

void main(void)
{

	awakenFromThySlumberShelly();
	greetingsSpeSHELL1();
	while (exit_flag == 0)
	{
		promptMSGShelly();
		executioner();
		
	}
	
}

/**
 * @FUNCTIONS Garbage Destructor
 */

//void memoryHolocaust(char** argv,int size)
//{
	//int i;
    	//for (i = 0; i < size; i++) 
        	//free(argv[i]);
    	//free(argv);
    	//argv = NULL;
//}

/**
 * @FUNCTIONS Greetings
 */

void greetingsSpeSHELL1(){
        printf("\n\t================================================\n");
        printf("\t               Welcome to Shelly\n");
        printf("\t------------------------------------------------\n");
        printf("\t     Only Those Who are Spe-Shell Shell Pass\n");
		//printf("\t--------------------------------------------\n");
		//printf("\t             Shell Yeah!\n");
        printf("\t================================================\n");
        printf("\n\n");
}

/**
 * @FUNCTIONS Prompt
 */

void promptMSGShelly(){
	char speSHELL1[1313] = "";
	currentDirectory = (char*) calloc(1024, sizeof(char));
	gethostname(speSHELL1, sizeof(speSHELL1));
	printf("%s@%s:~%s :) ", getenv("LOGNAME"), speSHELL1, getcwd(currentDirectory, 1024)); // <user>@<host> <cwd> :)
}

/**
 * @FUNCTION signal handler for SIGCHLD
 */

void signalHandler_CHLD(int p){
	/* Wait for all dead processes.
	 * Use a non-blocking call (WNOHANG) to be sure this signal handler will not
	 * block if a child was cleaned up in another part of the program. */
	while (waitpid(-1, NULL, WNOHANG) > 0);
        
        FILE *pFile;
	pFile = fopen("logs.txt", "a");
	printf("%d: ", id);
	fprintf(pFile, "%s\n", "[LOG] Child Proccess was Terminated");
        fclose(pFile);
	//printf("CHLD SIGNAL HANDLER: Done\n");
}

/**
 * @FUNCTION signal handler for SIGINT
 */
 
void signalHandler_INT(int p){
	// Send a SIGTERM signal to the child process
	if (kill(id,SIGTERM) == 0)
		printf("\nProcess %d received a SIGINT signal\n",id);
	else
		printf("\n");
	
}

/**
 * @FUNCTIONS Initialize Shelly
 */

void awakenFromThySlumberShelly()
{
	// See if we are running interactively
        shellyprocess_ID = getpid();
        // The shell is interactive if STDIN is the terminal  
        shelly_isinteractive = isatty(STDIN_FILENO);  

	if (shelly_isinteractive) 
	{			
		// Loop until we are in the foreground
		while (tcgetpgrp(STDIN_FILENO) != (shelly_processGP_ID = getpgrp()))
			kill(shelly_processGP_ID, SIGTTIN);             
	              
	              
	        // Set the signal handlers for SIGCHILD and SIGINT
		act_CHLD.sa_handler = signalHandler_CHLD;
		act_INT.sa_handler = signalHandler_INT;			

			
		sigaction(SIGCHLD, &act_CHLD, 0);
		sigaction(SIGINT, &act_INT, 0);
			
		// Put ourselves in our own process group
		setpgid(shellyprocess_ID, shellyprocess_ID); // we make the shell process the new process group leader
		shelly_processGP_ID = getpgrp();
		if (shellyprocess_ID != shelly_processGP_ID) 
		{
			printf("AWAKENFROMTHYSLUMBERSHELLY: Could not Put Shelly in her own Process Group\n");
			exit(EXIT_FAILURE);
		}
		// Grab control of the terminal
		tcsetpgrp(STDIN_FILENO, shelly_processGP_ID);  
			
		// Save default terminal attributes for shell
		tcgetattr(STDIN_FILENO, &shellyMODES);

		// Get the current directory that will be used in different methods
		currentDirectory = (char*) calloc(1024, sizeof(char));
		
        } else {
                printf("AWAKENFROMTHYSLUMBERSHELLY: Could not Make Shelly Interactive\n");
                exit(EXIT_FAILURE);
        }
}

/**
 * @FUNCTIONS Executor of commands
 */

void executioner (void)
{
		char *cmd = malloc(sizeof(char) * SIZE); // command
		char *cmd_cpy = malloc(sizeof(char) * SIZE); // command copy
		char *delim = " \n"; // delimiter
		char *token = malloc(sizeof(char) * SIZE); // token
	
		size_t n = 0;
		int argc = 0;
		int i = 0;
		char **argv = malloc(sizeof(char*) * SIZE);
		
	
		if (getline(&cmd, &n, stdin) == -1)
			printf("EXECUTIONER: line is empty");
		
		cmd_cpy = strdup(cmd);
	
		token = strtok(cmd, delim);
	
		while (token)
		{
			if (strcmp(token,"&") == 0)
				background = 1;
			token = strtok(NULL, delim);
			argc++;
		}
	
		argv = malloc(sizeof(char *) * argc);
	
		token = strtok(cmd_cpy, delim);
	
		while (token)
		{
			if (!(strcmp(token,"&")))
			{	
				token = strtok(NULL, delim);
				continue;
			}
			argv[i] = token;
			token = strtok(NULL, delim);
			i++;
		}
		argv[i] = NULL;
		//printf("%d", strcmp(argv[0], "exit"));
		
		if (!strcmp(argv[0], "exit"))
		{
			printf("Big Green Out\n");
			printf("ADIOS\n");
			exit_flag = 1;
			return;
			
		}
		
		
		id = fork();
		
		if (id < 0)
		{
			//failed forky
			perror("Forky");
		} else if (id == 0) {
				
			//child
			// We set the child to ignore SIGINT signals (we want the parent
			// process to handle them with signalHandler_int)	
			signal(SIGINT, SIG_IGN);
			
			//argv[i] = NULL;
			// If we launch non-existing commands we end the process
			if (execvp(argv[0],argv)==-1){
				printf("Command not found");
				kill(getpid(),SIGTERM);
			}
			 
		}
		
		if (background == 0){
		 	waitpid(id,NULL,0);
	 	} else {
		 	// background is active
		 	printf("Process created with PID: %d\n",id);
		 	background = 0; // clear background flag for next process
		 	
	 	}
	 	
	 	//memoryHolocaust(argv, argc);
}



