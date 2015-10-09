#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<sys/types.h>
#include<fcntl.h>
int main()
{
 char command[100];
 int pid, status, flag=0;


    while(1){
	flag = 0;
	char *a[3];
	printf(">>");
	scanf("%[^\n]", command);
	pid = fork();
	if (pid!=0) {
		waitpid(pid, &status, WUNTRACED);
		break;
	}
	else {
		char *argv[100], inp_file[100];
		int i=0, j=0, k=0, input, output;
		while(command[i]!='\n'){
			argv[j] = (char *)malloc(sizeof(char)*100);
			while(command[i]==' ')
				i++;
			if(command[i] == '\0')
			{
				argv[j][k] = '\0';
				break;
			}
			if(command[i] == '<')
			{
				flag = 1;
				i++;
				break;
			}
			if(command[i] == '>')
			{
				flag = 2;
				i++;
				break;
			}
			while(command[i] != ' ' && command[i] != '\0')
			{
				argv[j][k] = (command[i]);
				i++;
				k++;
			}
			argv[j][k] = '\0';
			k=0;
			j++;
		}
		while(j<=100)
		{
			argv[j] = '\0';
			j++;
		}
		if(flag == 1)
		{
			k=0;
			while(command[i] == ' ' || command[i] == '<')
				i++;
			while(command[i] != '\n' && command[i] != '\0')
			{
				inp_file[k++] = command[i++];
			}
			inp_file[k] = '\0';
			input = open(inp_file, O_RDONLY);
			dup2(input,0);
			execvp(argv[0], argv);
			close(input);
		}	

		if(flag == 2)
		{
			if(command[i++] == '>')
			{
				k=0;
				while(command[i] == ' ')
					i++;
				while(command[i] != '\n' && command[i] != '\0')
				{
					inp_file[k++] = command[i++];
				}
				inp_file[k] = '\0';
				output = open(inp_file, O_WRONLY|O_APPEND| O_CREAT, S_IRWXU);
				dup2(output,1);
				execvp(argv[0], argv);
				close(output);
			}
			else
			{
				k=0;
				while(command[i] == ' ')
					i++;
				while(command[i] != '\n' && command[i] != '\0')
				{
					inp_file[k++] = command[i++];
				}
				inp_file[k] = '\0';
				output = open(inp_file, O_WRONLY| O_CREAT| O_TRUNC, S_IRWXU);
				dup2(output,1);
				execvp(argv[0], argv);
				close(output);
			}
		}	
		else
		{
		//printf("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
        		execvp(argv[0], argv);
		}
		//If it prints Execvp failed that indicates that execvp has failed
		printf("Execvp Failed\n");
		break;
	}
   }
   return 0;
}
