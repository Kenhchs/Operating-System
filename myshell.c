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
char* substring(char *str ,int start, int end);
void print_history(int all, int num);
int muti_zero(char*, int);

char hist[2048][1024];
int hist_num = -1;

int main()
{
	char input_string[MAXCOM];
	while(1)
	{
		if(Input(input_string))
		{
			continue;
		}
		Process_String(input_string);
	}
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
	if(hist_num >= 2047)
	{
		printf("Excess of history.");
		exit(0);
	}
	else
	{
		if(strcmp(hist[hist_num],temp) == 0);
		else
		{
			hist_num += 1;
			strcpy(hist[hist_num],temp);
		}
	}
	char* token = strtok(temp," ");
	if(strcmp(token,"echo") == 0)
	{
		token = strtok(NULL," ");
		while(token != NULL)
		{
			if(token[0] == '$')
			{
				char *substr = substring(token ,1 , strlen(token) - 1);
				printf("%s ",getenv(substr));
			}
			else
			{
				printf("%s ",token);	
			}
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
		char* temp_path = malloc(4096);
		strcpy(temp_path,"");
		token = strtok(NULL,"=");
		char *variable = token;
		
		token = strtok(NULL,":");
		while(token != NULL)
		{
			if(strcmp(temp_path,"") == 0);
			else
			{
				strcat(temp_path,":");
			}
			if(token[0] == '$')
			{
				char *substr = substring(token ,1 , strlen(token) - 1);
				if(getenv(substr) != NULL)
				{
					strcat(temp_path,getenv(substr));
				}
			}
			else
			{
				strcat(temp_path,token);
			}
			token = strtok(NULL,":");
		}
		setenv(variable,temp_path,1);
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
	else if(strcmp(token,"history") == 0)
	{
		int count = 0, num;
		token = strtok(NULL," ");
		while(token != NULL)
		{
			int val = atoi(token);
			if(val == 0 && muti_zero(token, strlen(token) - 1) == 0)
			{
				if(count >= 1)
				{
					printf("history: too many arguments\n");
				}
				else
				{
					printf("history: %s: numeric argument required\n",token);	
				}
				return;
			}
			else if(val == 0 && muti_zero(token, strlen(token) - 1) == 1);
			else if(val != 0)
			{
				count += 1;
				if(count >= 2)
				{
					printf("history: too many arguments\n");
					return;
				}
				num = val;
			}
			token = strtok(NULL," ");
		}
		if(count == 0)
		{
			print_history(1, num);
		}
		if(count == 1)
		{
			print_history(0, num);		
		}
	}
	else
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
			if(execvp(argv[0],argv) < 0)
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
char* substring(char *str ,int start, int end)
{
	char* substr = malloc(512);
	strcpy(substr,"");
	for(int i = start ; i <= end ; i++)
	{
		strncat(substr,&str[i],1);
	}
	return substr;
}
void print_history(int all, int num)
{
	if(all || num >= hist_num + 1)
	{
		for(int i = 0 ; i <= hist_num ; i++)
		{
			printf("%d\t%s\n",i+1,hist[i]);
		}
	}
	else
	{
		if(num < hist_num + 1)
		{
			for(int i = hist_num - num + 1 ; i <= hist_num ; i++)
			{
				printf("%d\t%s\n",i+1,hist[i]);
			}
		}
			
	}
}
int muti_zero(char *str, int num)
{
	for(int i = 0 ; i <= num ; i++)
	{
		if(str[i] != '0')
		{
			return 0;
		}
	}
	return 1;
}
