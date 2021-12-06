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
void My_echo(char*);
void My_clear(char*);
void My_cd(char*);
void My_export(char*);
void My_history(char*);
int Is_redirection(char*);

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
	char temp[1024],pass[1024];
	strcpy(temp, str);
	strcpy(pass,str);
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
		My_echo(&pass[0]);
	}
	else if(strcmp(token,"pwd") == 0)
	{
		char cwd[1024];
		getcwd(cwd,sizeof(cwd));
		printf("%s\n",cwd);
	}
	else if(strcmp(token,"export") == 0)
	{
		My_export(&pass[0]);
	}
	else if(strcmp(token,"cd") == 0)//assume that file's name doesn't contain spaces
	{
		My_cd(&pass[0]);
	}
	else if(strcmp(token,"clear") == 0)
	{
		My_clear(&pass[0]);
	}
	else if(strcmp(token,"history") == 0)
	{
		My_history(&pass[0]);
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
			return;
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
void My_echo(char *temp)
{
	FILE *fptr = stdout;
	int state = Is_redirection(temp);
	if(state == 1)
	{
		char temp2[1024];
		strcpy(temp2,temp);
		char *filename = strtok(temp2," ");
		while(filename != NULL)
		{
			if(strcmp(filename,">") == 0)
			{
				filename = strtok(NULL," ");
				fptr = fopen(filename,"w");
				int i = 0;
				while(1)
				{
					if(temp[i] == '>')
					{
						break;
					}
					else
					{
						i++;
					}
				}
				int length = strlen(temp);
				for(int j = i ; j < length ; j++)
				{
					temp[j] = '\0';
				}
				break;
			}
			filename = strtok(NULL," ");
		}		
	}
	char* token = strtok(temp," ");
	token = strtok(NULL," ");
	while(token != NULL)
	{
		if(token[0] == '$')
		{
			char *substr = substring(token ,1 , strlen(token) - 1);
			fprintf(fptr,"%s ",getenv(substr));
		}
		else
		{
			fprintf(fptr,"%s ",token);	
		}
		token = strtok(NULL," ");
	}
	fprintf(fptr,"\n");
	if(state == 1)
	{
		fclose(fptr);	
	}
	return;
}
void My_clear(char *temp)
{
	char* token = strtok(temp," ");
	token = strtok(NULL," ");
	int count = 0;
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
	return;
}
void My_cd(char *temp)
{
	char* token = strtok(temp," ");
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
	return;
}
void My_export(char *temp)
{
	char* token = strtok(temp," ");
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
	if(strcmp(temp_path,"") == 0)
	{
		char *exports[60] = {"CLUTTER_IM_MODULE","COLORTERM","DBUS_SESSION_BUS_ADDRESS","DESKTOP_AUTOSTART_ID","DESKTOP_SESSION","DISPLAY","GDMSESSION","GNOME_DESKTOP_SESSION_ID","GNOME_SHELL_SESSION_MODE","GNOME_TERMINAL_SCREEN","GNOME_TERMINAL_SERVICE","GPG_AGENT_INFO","GTK_IM_MODULE","GTK_MODULES","HOME","IM_CONFIG_PHASE","LANG","LC_ADDRESS","LC_IDENTIFICATION","LC_MEASUREMENT","LC_MONETARY","LC_NAME","LC_NUMERIC","LC_PAPER","LC_TELEPHONE","LC_TIME","LESSCLOSE","LESSOPEN","LOGNAME","LS_COLORS","OLDPWD","PATH","PWD","QT4_IM_MODULE","QT_ACCESSIBILITY","QT_IM_MODULE","SESSION_MANAGER","SHELL","SHLVL","SSH_AGENT_PID","SSH_AUTH_SOCK","TERM","TEXTDOMAIN","TEXTDOMAINDIR","USER","USERNAME","VTE_VERSION","WINDOWPATH","XAUTHORITY","XDG_CONFIG_DIRS","XDG_CURRENT_DESKTOP","XDG_DATA_DIRS","XDG_MENU_PREFIX","XDG_RUNTIME_DIR","XDG_SEAT","XDG_SESSION_DESKTOP","XDG_SESSION_ID","XDG_SESSION_TYPE","XDG_VTNR","XMODIFIERS"};
		for(int i = 0 ; i < 60 ; i++)
		{
			if(getenv(exports[i]) <= 0)
			{
				printf("declare -x %s\n",exports[i]);
				continue;
			}
			if(strcmp(getenv(exports[i]),"") == 0)
			{
				printf("declare -x %s\n",exports[i]);
			}
			else
			{
				printf("declare -x %s=\"%s\"\n",exports[i],getenv(exports[i]));
			}
		}
	}
	else
	{
		setenv(variable,temp_path,1);	
	}
	return;
}
void My_history(char *temp)
{
	char* token = strtok(temp," ");
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
	return;
}
int Is_redirection(char *str)
{
	for(int i = 0 ; i < strlen(str) ; i++)
	{
		if(str[i] == '>')
		{
			return 1;
		}
	}
	return 0;
}
