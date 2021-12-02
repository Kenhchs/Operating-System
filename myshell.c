#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
  
#define MAXCOM 1000
#define MAXLIST 100  
#define clear() printf("\033[H\033[J")

void green();
void reset();//reset color
int Input(char*);//get command
void Process_String(char *str);
void Directory();
int main()
{
	char input_string[MAXCOM];
	clear();
	while(1)
	{
		if(Input(input_string))
		{
			continue;
		}
		reset();
		Process_String(input_string);
	}
}
void Directory()
{
	green();
	char cwd[1024];
	getcwd(cwd,sizeof(cwd));
	strcat(cwd,"$ ");
	printf("%s",cwd);
}
int Input(char* str)
{
	green();
	char cwd[1024];
	getcwd(cwd,sizeof(cwd));
	strcat(cwd,"$ ");
	strcat(cwd,"\033[0m");
	char *buffer = readline(cwd);
	strcat(cwd,buffer);
	
	
	if(strlen(buffer) != 0)
	{
		strcpy(str, buffer);
		add_history(buffer);
		return 0;
	}
	else
	{
		return 1;
	}
}
void Process_String(char *str)
{
	char temp[1024];
	strcpy(temp, str);
	char* token = strtok(temp," ");
	if(strcmp(token,"echo") == 0)
	{
		token = strtok(NULL," ");
		while(token != NULL)
		{
			printf("%s ",token);
			token = strtok(NULL," ");
		}
		printf("\n");
	}
	else if(strcmp(token,"pwd") == 0)
	{
		char cwd[1024];
		getcwd(cwd,sizeof(cwd));
		printf("%s\n",cwd);
	}
	else if(strcmp(token,"export") == 0)
	{
		
	}
	else if(strcmp(token,"cd") == 0)//assume that file's name doesn't contain spaces
	{
		int count = 0;
		token = strtok(NULL," ");
		while(token != NULL)
		{
			count++;
			if(count >= 2)
			{
				printf("cd: too many arguments\n");
				break;
			}
			strcpy(temp,token);
			token = strtok(NULL," ");
		}
		if(count == 1)
		{
			if(chdir(temp) < 0)
			{
				printf("cd: %s: No such file or directory\n",temp);
			}
		}
		else if(count == 0)
		{
			char* username = getenv("USER");
			strcpy(temp,"/home/");
			strcat(temp,username);
			chdir(temp);
		}
	}
	else if(strcmp(token,"clear") == 0)
	{
		int count = 0;
		token = strtok(NULL," ");
		while(token != NULL)
		{
			count++;
			if(count >= 1)
			{
				printf("Could not execute command\n");
				break;
			}
			strcpy(temp,token);
			token = strtok(NULL," ");
		}
		if(count == 0)
		{
			clear();
		}
	}
	else if(strcmp(token,"ls") == 0)
	{
		int i = 0;
		char *argv[MAXLIST];
		for(int j = 0 ; j < MAXLIST ; j++)
		{
			argv[j] = NULL;
		}
		argv[i++] = token;
		token = strtok(NULL," ");
		while(token != NULL)
		{
			argv[i++] = token;
			token = strtok(NULL," ");
		}
		argv[i] = NULL;
		
		pid_t pid = fork();
		if(pid < 0)
		{
			printf("Failed forking child\n");
			return;
		}
		else if(pid == 0)
		{
			if(execvp("ls",argv) < 0)
			{
				printf("Could not execute command\n");
			}
			exit(0);
		}
		else
		{
			wait(NULL);
			return;
		}
		
	}
	else if(strcmp(token,"cp") == 0)
	{
		
	}
	else
	{
		printf("Could not execute command\n");
	}
	return;
}
void green()
{
	printf("\033[0;32m");
}
void reset()
{
	printf("\033[0m");
}
