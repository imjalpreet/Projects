#include <dirent.h>   
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include<stdio.h>

void stcat(char *a, char *b, char *c){
	int i = 0;
	while(a[i]){
		c[i] = a[i];
		i++;
	}
	int j;
	for(j=0;b[j];j++)
	{
		c[i] = b[j];
		i++;
	}
	c[i] = '\0';
}

void getGroup(char *s, char *id)
{
	int fd, i = 0, flag;
        fd = open("/etc/group", O_RDONLY);
	while(1)
	{
                flag = 0;
                char a[1024];
                read(fd, a, 1);
                i=0;
                while(a[0] != ':'){
                        s[i] = a[0];
                        i++;
                        read(fd, a, 1);
                }
                s[i] = '\0';
                read(fd, a, 2);
                read(fd, a, 1);
                i = 0;
                char b[100];
                while(a[0] != ':'){
                        b[i] = a[0];
                        i++;
                        read(fd, a, 1);
                }
                b[i] = '\0';
                i=0;
                int l1=0, l2=0;
                while(b[l1])
                        l1++;
                while(id[l2])
                        l2++;
                l2--;
                l1--;
                if(l1 == l2){
                        while(l2 >= 0){
                                if(b[l2] != id[l2])
                                {
                                        flag = 1;
                                        break;
                                }
				l2--;
                        }
                }
                else
                        flag = 1;
                if(!flag)
                        break;
                read(fd, a, 1);
                while(a[0] != '\n')
                        read(fd, a, 1);
			
	}
}

void getUser(char *s, char *id)
{
        int fd, i = 0, flag;
        fd = open("/etc/passwd", O_RDONLY);
        while(1){
                flag = 0;
                char a[1024];
                read(fd, a, 1);
                i=0;
                while(a[0] != ':'){
                        s[i] = a[0];
                        i++;
                        read(fd, a, 1);
                }
                s[i] = '\0';
                read(fd, a, 2);
                read(fd, a, 1);
                i = 0;
                char b[100];
                while(a[0] != ':'){
                        b[i] = a[0];
                        i++;
                        read(fd, a, 1);
                }
                b[i] = '\0';
                i=0;
                int l1=0, l2=0;
                while(b[l1])
                        l1++;
                while(id[l2])
                        l2++;
                l2--;
                l1--;
                if(l1 == l2){
                        while(l2 >= 0){
                                if(b[l2] != id[l2])
                                {
                                        flag = 1;
                                        break;
                                }
				l2--;
                        }
                }
                else
                        flag = 1;
                if(!flag)
                        break;
                read(fd, a, 1);
                while(a[0] != '\n')
                        read(fd, a, 1);
        }
}

int stcmp(char a[], char b[])
{
	int len1=0, len2=0;
	while(a[len1])
		len1++;
	while(b[len2])
		len2++;
	if(len1 != len2)
		return 1;
	len2 = 0;
	while(len1--)
	{
		if(a[len2] != b[len2])
			return 1;
		len2++;
	}
	return 0;
}

struct linux_dirent {
   long           d_ino;
   off_t          d_off;
   unsigned short d_reclen;
   char           d_name[];
};

int main(int argc, char *argv[])
{
   	int fd, nread, i, j;
   	char buf[1024*1024*5];
	char m[1024];
   	struct linux_dirent *d;
   	int bpos;
   	char d_type;
	struct stat s;
	if((argc == 2 && (argv[1][0] != '-')) || (argc == 3 && argv[1][0] != '-' && argv[2][0] != '-') || argc == 1){
		stat(argv[1], &s);
		i = 0;
		if(s.st_mode & S_IFREG)
		{
			char *t;
			t = argv[1];
			while((argv[1])[i]){
				i++;
			}
			write(1, t, i);
			write(1, "\n", 2);
			exit(0);
		}
   		fd = open(argc > 1 ? argv[1] : ".", O_RDONLY | O_DIRECTORY);
   		if (fd == -1)
		{
			write(1, "open: No file or Directory", 26);
			exit(0);
		}
       			//handle_error("open");

   		for ( ; ; ) {
       			nread = syscall(SYS_getdents, fd, buf, 1024*1024*5);
       			if (nread == -1)
				write(1, "getdents: No such file or directory\n", 37);
           			//handle_error("getdents");

       			if (nread == 0)
        	   		break;

       			for (bpos = 0; bpos < nread;) {
           			d = (struct linux_dirent *) (buf + bpos);
           			d_type = *(buf + bpos + d->d_reclen - 1);
	   			i = 0;
				char temp[5];
				temp[0] = d->d_name[0];
				temp[1] = '\0';
           			if( d->d_ino != 0 && (d_type == DT_REG || d_type == DT_DIR || d_type == DT_LNK) && stcmp(temp, ".") && stcmp(d->d_name, ".."))
				{
					char final[1024];
					struct stat st;
		   			while(d->d_name[i])
			   			i++;
					if(argc > 1)
					{
						stcat(argv[1], "/", argv[1]);
						stcat(argv[1], d->d_name, final);
					}
					else
						stcat("./", d->d_name, final);
					lstat(final, &st);
					if(st.st_mode & S_IFDIR){
						write(1, "\x1b[1;34m", 8);
		   				write(1, (char *)d->d_name, i);
						write(1, "\x1b[0m", 5);
		   				write(1, "\n", 2);
					}
					else if(S_ISLNK(st.st_mode)){
						write(1, "\x1b[1;36m", 8);
		   				write(1, (char *)d->d_name, i);
						write(1, "\x1b[0m", 5);
		   				write(1, "\n", 2);
					}
					else if(st.st_mode & S_IXUSR)
					{
						write(1, "\x1b[1;32m", 8);
		   				write(1, (char *)d->d_name, i);
						write(1, "\x1b[0m", 5);
		   				write(1, "\n", 2);
					}
					else
					{
						write(1, (char *)d->d_name, i);
						write(1, "\n", 2);
					}
           			}
           			bpos += d->d_reclen;
       			}	
       			write(1, "\n", 2);
		}
	}
	else if((argc == 2 && (argv[1][0] == '-')) || (argc == 3 && (argv[2][0] == '-' || argv[1][0] == '-')))
	{
		int option = 0;
		if(stcmp(argv[1], "-l") == 0 || (argc == 3 && stcmp(argv[2], "-l") == 0))
			option = 1;
		else if(stcmp(argv[1], "-a") == 0 || (argc == 3 && stcmp(argv[2], "-a") == 0))
			option = 2;
		else if(stcmp(argv[1], "-h") == 0 || (argc == 3 && stcmp(argv[2], "-h") == 0))
			option = 3;
		else if(stcmp(argv[1], "-la") == 0 || stcmp(argv[1], "-al") == 0 || (argc == 3 && stcmp(argv[2], "-la") == 0) || (argc == 3 && stcmp(argv[2], "-al") == 0))
			option = 4;
		else if(stcmp(argv[1], "-lh") == 0 || stcmp(argv[1], "-hl") == 0 || (argc == 3 && stcmp(argv[2], "-lh") == 0) || (argc == 3 && stcmp(argv[2], "-hl") == 0))
			option = 5;
		else if(stcmp(argv[1], "-ha") == 0 || stcmp(argv[1], "-ah") == 0 || (argc == 3 && stcmp(argv[2], "-ha") == 0) || (argc == 3 && stcmp(argv[2], "-ah") == 0))
			option = 6;
		else if(stcmp(argv[1], "-lah") == 0 || stcmp(argv[1], "-lha") == 0 || stcmp(argv[1], "-ahl") == 0 || stcmp(argv[1], "-hal") == 0 || stcmp(argv[1], "-alh") == 0 || stcmp(argv[1], "-hla") == 0 || (argc == 3 && stcmp(argv[2], "-lah") == 0) || (argc == 3 && stcmp(argv[2], "-ahl") == 0) || (argc == 3 && stcmp(argv[2], "-hal") == 0) || (argc == 3 && stcmp(argv[2], "-lha") == 0) || (argc == 3 && stcmp(argv[2], "-alh") == 0) || (argc == 3 && stcmp(argv[1], "-hla") == 0))
			option = 7;
		int Flag = 0;
		char temp1[100];
		if(argv[1][0] == '-' && argc == 3)
			stcat("", argv[2], temp1);
		else
			stcat("", argv[1], temp1);
		lstat(temp1, &s);
		if(s.st_mode & S_IFREG)
		{
			int i = 0;
			if(option == 2 || option == 6)
			{
		   		while(temp1[i])
		  			i++;
	  			write(1, (char *)temp1, i);
		   		write(1, "\n", 2);
				exit(1);
			}
			if(option == 3)
			{
		   		while(temp1[i])
			  		i++;
		   		write(1, (char *)temp1, i);
				write(1, "\n", 2);
				exit(1);
			}

			i = 0;
			char permissions[11];
			
			int flag = 0;
			if(s.st_mode & S_IFDIR){
				permissions[i] = 'd';
				flag =1;
			}
			if(S_ISLNK(s.st_mode)){
				permissions[i] = 'l';
				flag = 1;
			}
			if((s.st_mode & S_IFREG) && !flag)
				permissions[i] = '-';
			i++;
			if(s.st_mode & S_IRUSR){
				permissions[i++] = 'r';
			}
			else
				permissions[i++] = '-';
			if(s.st_mode & S_IWUSR){
				permissions[i++] = 'w';
			}
			else
				permissions[i++] = '-';
			if(s.st_mode & S_IXUSR){
				permissions[i++] = 'x';
			}
			else
				permissions[i++] = '-';
			if(s.st_mode & S_IRGRP){
				permissions[i++] = 'r';
			}
			else
				permissions[i++] = '-';
			if(s.st_mode & S_IWGRP){
				permissions[i++] = 'w';
			}
			else
				permissions[i++] = '-';
			if(s.st_mode & S_IXGRP){
				permissions[i++] = 'x';
			}
			else
				permissions[i++] = '-';
			if(s.st_mode & S_IROTH){
				permissions[i++] = 'r';
			}
			else
				permissions[i++] = '-';
			if(s.st_mode & S_IWOTH){
				permissions[i++] = 'w';
			}
			else
				permissions[i++] = '-';
			if(s.st_mode & S_IXOTH){
				permissions[i++] = 'x';
			}
			else
				permissions[i++] = '-';
			write(1, permissions, i);
			write(1, "\t", 2);
			char a[100];
			j = 0;
			i = s.st_nlink;
			while(i){
				j++;
				i/=10;
			}
			i = s.st_nlink;
			int e = j;
			a[j] = '\0';
			j--;
			while(i){
				a[j] = i%10 + '0';
				i/=10;
				j--;
			}
			write(1, a, e);
			write(1, "\t", 2);
			j=0;
			i = s.st_uid;
			while(i){
				j++;
				i/=10;
			}
			i = s.st_uid;
			a[j] = '\0';
			j--;
			if(!i){
				a[0] = '0';
				a[1] = '\0';
			}
			while(i){
				a[j] = i%10 + '0';
				i/=10;
				j--;
			}
			char q[100];
			getUser(q, a);
			i=0;
			while(q[i]){
				i++;
			}
			write(1, q, i);
			write(1, "\t", 2);
			j=0;
			i = s.st_gid;
			while(i)
			{
				j++;
				i/=10;
			}
			i = s.st_gid;
			a[j] = '\0';
			j--;
			if(!i){
				a[0] = '0';
				a[1] = '\0';
			}
			while(i)
			{
				a[j] = i%10 + '0';
				i/=10;
				j--;
			}
			char r[100];
			getGroup(r, a);
			i = 0;
			while(r[i])
				i++;
			write(1, r, i);
			write(1, "\t", 2);
			if(option == 3 || option == 5 || option == 6 || option == 7) 
			{
				int x = 0;
				double size = s.st_size;
				const char* units[] = {" ", "K", "M", "G", "T", "P"};
				while(size > 1024){
					size /= 1024;
					x++;
				}
				int c = 0;
				int temp = size * 10;
				char b[100];
				while(temp){
					temp/=10;
					c++;
				}
				temp = size*10;
				e = ++c;
				b[c] = '\0';
				c--;
				if(!temp)
					b[c] = '0';
				while(temp){
					b[c] = temp%10 + '0';
					c--;
					if(e-2 == c)
					{
						b[c] = '.';
						c--;
					}
					temp/=10;
				}
				write(1, b, e);
				write(1, units[x], 2);
				write(1, "\t", 2);
			}
			else
			{
				long long int g = s.st_size;
				j = 0;
				if(!g){
					write(1, "0\t", 3);
				}
				while(g){
					g /= 10;
					j++;
				}
				g = s.st_size;
				a[j] = '\0';
				e = j;
				j--;
				while(g)
				{
					a[j] = g % 10 + '0';
					g /= 10;
					j--;
				}
				write(1, a, e);
				write(1, "   ", 3);
			}
			time_t time = s.st_mtime;
			i = 1970, 
			flag = 0;
			long long int tstamp1, tstamp;
			tstamp1 = (long long int) time;
			for(i = 1970; ; i++)
			{
				int seconds;
				if(i % 4)
					seconds = 31536000;
				else if(i % 100)
					seconds = 31622400;
				else if(i % 400)
					seconds = 31536000;
				else
					seconds = 31622400;

				if(tstamp1 < seconds)
					break;
				tstamp1 -= seconds;
			}
			//printf("%d ", i);
			int year = i;
			for ( i = 1; i <= 12; i++)
			{
				int second;
				if(i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 10 || i == 12)
					second = 2678400;
				if(i == 4 || i == 6 || i == 9 || i == 11)
					second = 2592000;
				if(i == 2)
				{

					if(i % 4)
						second = 2419200;
					else if(i % 100)
					{
						flag = 1;
						second = 2505600;
					}
					else if(i % 400)
						second = 2419200;
					else
					{
						second = 2505600;
						flag = 1;
					}
				}
				if(tstamp1 < second)
					break;
				tstamp1 -= second;
			}
			char month[][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0", "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Oct\0", "Nov\0", "Dec\0"};
			write(1, month[i-1], 3);
			write(1, " ", 1);
			char m = month[i-1];
			if(i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 10 || i == 12)
				j = 31;
			if(i == 4 || i == 6 || i == 9 || i  == 11)
				j = 30;
			if(flag == 1)
				j = 29;
			else
				j = 28;
			for(i=1;i<=j;i++)
			{
				if(tstamp1 < 86400)
					break;
				tstamp1 -= 86400;
			}
			j = i;
			int k=0;
			while(j)
			{
				j/=10;
				k++;
			}
			a[k] = '\0';
			j = k;
			while(k)
			{
				a[k-1] = i%10 + '0';
				k--;
				i/=10;
			}
			//printf("%d ", j);
			write(1, a, j);
			write(1, "  ", 2);
			tstamp = (long long int) time;
			tstamp += 19800;
			int hour = (tstamp/3600)%24;
			int minute = (tstamp/60)%60;
			i = 0;
			if(hour == 0)
				write(1, "00", 2);
			while(hour)
			{
				i++;
				hour /= 10;
			}
			hour = (tstamp/3600)%24;
			a[i] = '\0';
			int flag2 = 0;
			if(i == 1){
				i++;
				flag2 = 1;
			}
			while(i)
			{
				a[i-1] = hour%10 + '0';
				hour /= 10;
				i--;
				if(flag2)
				{
					a[0] = '0';
					i--;
					flag2 = 0;
				}
			}
			i = 0;
			while(minute)
			{
				i++;
				minute /= 10;
			}
			write(1, a, 2);
			write(1, ":", 1);
			minute = (tstamp/60)%60;
			if(minute == 0)
				write(1, "00", 2);
			a[i] = '\0';
			if(i == 1){
				i++;
				flag2 = 1;
			}
			while(i)
			{
				a[i-1] = minute%10 + '0';
				minute /= 10;
				i--;
				if(flag2)
				{
					a[0] = '0';
					i--;
					flag2 = 0;
				}
			}
			write(1, a, 2);
			write(1, "\t", 2);
			//printf("%lld:%lld\t",(tstamp/3600)%24,(tstamp/60)%60);

			i=0;
			while(temp1[i])
				i++;
			if(permissions[0] == 'd'){
				write(1, "\x1b[1;34m", 8);
				write(1, (char *)temp1, i);
				write(1, "\x1b[0m", 5);
			}
			else if(permissions[0] == 'l')
			{
				write(1, "\x1b[1;36m", 8);
				write(1, (char *)temp1, i);
				write(1, "\x1b[0m", 5);
				char buf[1024];
				int count = readlink(temp1, buf, sizeof(buf));
				if(count >= 0){
					buf[count] = '\0';
					write(1, "->", 2);
					write(1, buf, count);
				}
			}
			else if(permissions[3] == 'x')
			{
				write(1, "\x1b[1;32m", 8);
				write(1, (char *)temp1, i);
				write(1, "\x1b[0m", 5);
			}
			else
				write(1, (char *)temp1, i);
			write(1, "\n", 2);
			exit(0);
		}
		if(option){
		i = 0;
		while(argc == 3 && argv[2][i] && argv[2][0] != '-')
		{
			i++;
		}
		while(argv[1][i] && argv[1][0] != '-')
		{	
			i++;
		}
		if(argc == 3 && argv[1][0] == '-')
		{
			argv[2][i] = '/';
			argv[2][i+1] = '\0';
		}
		else if(argc == 3 && argv[2][0] == '-')
		{
			j = 0;
			while(argv[1][j]){
				m[j] = argv[1][j]; 
				j++;
			}
			m[j] = '/';
			m[j+1] = '\0';
		}
		if(argc == 3 && argv[2][0] != '-')
		{
			fd = open(argc > 1 ? argv[2] : "./", O_RDONLY | O_DIRECTORY);
		}
		else if(argv[1][0] != '-')
		{
			fd = open(argc > 1 ? m : "./", O_RDONLY | O_DIRECTORY);
		}
		else
			fd = open("./", O_RDONLY | O_DIRECTORY);
   		if (fd == -1)
		{
			write(1, "open: No such file or directory\n", 33);
       			//handle_error("open");
		}
		for( ; ; ) {
       			nread = syscall(SYS_getdents, fd, buf, 1024*1024*5);
       			if (nread == -1)
				write(1, "getdents: No such file or directory\n", 37);
           			//handle_error("getdents");

       			if (nread == 0)
        	   		break;
			
       			for (bpos = 0; bpos < nread;) {
           			d = (struct linux_dirent *) (buf + bpos);
           			d_type = *(buf + bpos + d->d_reclen - 1);
	   			i = 0;
				char temp[5];
				temp[0] = d->d_name[0];
				temp[1] = '\0';
           			if( d->d_ino != 0 && (d_type == DT_REG || d_type == DT_DIR || d_type == DT_LNK) && (((option == 1 || option == 3 || option == 5) && stcmp(temp, ".")) || option == 2 || option == 4 || option == 6 || option == 7))
				{
					if(option == 2 || option == 6)
					{
		   				while(d->d_name[i])
			   				i++;
		   				write(1, (char *)d->d_name, i);
		   				write(1, "\n", 2);
           					bpos += d->d_reclen;
						continue;
					}
					if(option == 3)
					{
		   				while(d->d_name[i])
			   				i++;
		   				write(1, (char *)d->d_name, i);
		   				write(1, "\n", 2);
           					bpos += d->d_reclen;
						continue;
					}

					i = 0;
					char permissions[11];
					char final[1000];
					if(argc == 3 && argv[2][0] != '-')
						stcat(argv[2], d->d_name, final);
					else if(argv[1][0] != '-')
						stcat(m, d->d_name, final);
					else
					{
						stcat("./", d->d_name, final);
					}
					lstat(final, &s);
					int flag = 0;
					if(s.st_mode & S_IFDIR){
						permissions[i] = 'd';
						flag =1;
					}
					if(s.st_mode & S_ISUID || s.st_mode & S_ISGID)
					{
						permissions[i] = 's';
						flag = 1;
					}
					if(S_ISLNK(s.st_mode)){
						permissions[i] = 'l';
						flag = 1;
					}
					if((s.st_mode & S_IFREG) && !flag)
						permissions[i] = '-';
					i++;
					if(s.st_mode & S_IRUSR){
						permissions[i++] = 'r';
					}
					else
						permissions[i++] = '-';
					if(s.st_mode & S_IWUSR){
						permissions[i++] = 'w';
					}
					else
						permissions[i++] = '-';
					if(s.st_mode & S_IXUSR){
						permissions[i++] = 'x';
					}
					else
						permissions[i++] = '-';
					if(s.st_mode & S_IRGRP){
						permissions[i++] = 'r';
					}
					else
						permissions[i++] = '-';
					if(s.st_mode & S_IWGRP){
						permissions[i++] = 'w';
					}
					else
						permissions[i++] = '-';
					if(s.st_mode & S_IXGRP){
						permissions[i++] = 'x';
					}
					else
						permissions[i++] = '-';
					if(s.st_mode & S_IROTH){
						permissions[i++] = 'r';
					}
					else
						permissions[i++] = '-';
					if(s.st_mode & S_IWOTH){
						permissions[i++] = 'w';
					}
					else
						permissions[i++] = '-';
					flag = 0;
					if(s.st_mode & S_IXOTH){
						permissions[i] = 'x';
						flag = 1;
					}
					if(s.st_mode & S_ISVTX)
					{
						permissions[i] = 't';
						flag = 1;
					}
					if(!flag)
						permissions[i] = '-';
					i++;
					write(1, permissions, i);
					write(1, "\t", 2);
					char a[100];
					j = 0;
					i = s.st_nlink;
					while(i){
						j++;
						i/=10;
					}
					i = s.st_nlink;
					int e = j;
					a[j] = '\0';
					j--;
					while(i){
						a[j] = i%10 + '0';
						i/=10;
						j--;
					}
					write(1, a, e);
					write(1, "\t", 2);
					j=0;
					i = s.st_uid;
        				while(i){
                				j++;
                				i/=10;
        				}
        				i = s.st_uid;
        				a[j] = '\0';
        				j--;
					if(!i){
						a[0] = '0';
						a[1] = '\0';
					}
        				while(i){
                				a[j] = i%10 + '0';
                				i/=10;
                				j--;
        				}
					char q[100];
					getUser(q, a);
					i=0;
					while(q[i]){
						i++;
					}
					write(1, q, i);
					write(1, "\t", 2);
					j=0;
					i = s.st_gid;
					while(i)
					{
						j++;
						i/=10;
					}
					i = s.st_gid;
					a[j] = '\0';
					j--;
					if(!i){
						a[0] = '0';
						a[1] = '\0';
					}
					while(i)
					{
						a[j] = i%10 + '0';
						i/=10;
						j--;
					}
					char r[100];
					getGroup(r, a);
					i = 0;
					while(r[i])
						i++;
					write(1, r, i);
					write(1, "\t", 2);
					if(option == 3 || option == 5 || option == 6 || option == 7) 
					{
						int x = 0;
						double size = s.st_size;
						const char* units[] = {" ", "K", "M", "G", "T", "P"};
						while(size > 1024){
							size /= 1024;
							x++;
						}
						int c = 0;
						int temp = size * 10;
						char b[100];
						while(temp){
							temp/=10;
							c++;
						}
						temp = size*10;
						e = ++c;
						b[c] = '\0';
						c--;
						if(!temp)
							b[c] = '0';
						while(temp){
							b[c] = temp%10 + '0';
							c--;
							if(e-2 == c)
							{
								b[c] = '.';
								c--;
							}
							temp/=10;
						}
						write(1, b, e);
						write(1, units[x], 2);
						write(1, "\t", 2);
					}
					else
					{
						long long int g = s.st_size;
						j = 0;
						if(!g){
							write(1, "0\t", 3);
						}
						while(g){
							g /= 10;
							j++;
						}
						g = s.st_size;
						a[j] = '\0';
						e = j;
						j--;
						while(g)
						{
							a[j] = g % 10 + '0';
							g /= 10;
							j--;
						}
						write(1, a, e);
						write(1, "   ", 3);
					}
					time_t time = s.st_mtime;
					i = 1970, 
					  flag = 0;
					long long int tstamp1, tstamp;
					tstamp1 = (long long int) time;
					for(i = 1970; ; i++)
					{
						int seconds;
						if(i % 4)
							seconds = 31536000;
						else if(i % 100)
							seconds = 31622400;
						else if(i % 400)
							seconds = 31536000;
						else
							seconds = 31622400;

						if(tstamp1 < seconds)
							break;
						tstamp1 -= seconds;
					}
					//printf("%d ", i);
					int year = i;
					for ( i = 1; i <= 12; i++)
					{
						int second;
						if(i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 10 || i == 12)
							second = 2678400;
						if(i == 4 || i == 6 || i == 9 || i == 11)
							second = 2592000;
						if(i == 2)
						{

							if(i % 4)
								second = 2419200;
							else if(i % 100)
							{
								flag = 1;
								second = 2505600;
							}
							else if(i % 400)
								second = 2419200;
							else
							{
								second = 2505600;
								flag = 1;
							}
						}
						if(tstamp1 < second)
							break;
						tstamp1 -= second;
					}
					char month[][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0", "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Oct\0", "Nov\0", "Dec\0"};
					//printf("%s ", month[i-1]);
					write(1, month[i-1], 3);
					write(1, " ", 1);
					char m = month[i-1];
					if(i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 10 || i == 12)
						j = 31;
					if(i == 4 || i == 6 || i == 9 || i  == 11)
						j = 30;
					if(flag == 1)
						j = 29;
					else
						j = 28;
					for(i=1;i<=j;i++)
					{
						if(tstamp1 < 86400)
							break;
						tstamp1 -= 86400;
					}
					j = i;
					int k=0;
					while(j)
					{
						j/=10;
						k++;
					}
					a[k] = '\0';
					j = k;
					while(k)
					{
						a[k-1] = i%10 + '0';
						k--;
						i/=10;
					}
					write(1, a, j);
					write(1, "  ", 2);
					//printf("%d ", i);
					tstamp = (long long int) time;
					tstamp += 19800;
					int hour = (tstamp/3600)%24;
					int minute = (tstamp/60)%60;
					i = 0;
					if(hour == 0)
						write(1, "00", 2);
					while(hour)
					{
						i++;
						hour /= 10;
					}
					hour = (tstamp/3600)%24;
					a[i] = '\0';
					int flag2 = 0;
					if(i == 1){
						i++;
						flag2 = 1;
					}
					while(i)
					{
						a[i-1] = hour%10 + '0';
						hour /= 10;
						i--;
						if(flag2)
						{
							a[0] = '0';
							i--;
							flag2 = 0;
						}
					}
					i = 0;
					flag2 = 0;
					while(minute)
					{
						i++;
						minute /= 10;
					}
					write(1, a, 2);
					write(1, ":", 1);
					minute = (tstamp/60)%60;
					if(minute == 0)
						write(1, "00", 2);
					a[i] = '\0';
					if(i == 1){
						i++;
						flag2 = 1;
					}
					while(i)
					{
						a[i-1] = minute%10 + '0';
						minute /= 10;
						i--;
						if(flag2)
						{
							a[0] = '0';
							i--;
							flag2 = 0;
						}
					}
					write(1, a, 2);
					write(1, "\t", 2);
					i=0;
		   			while(d->d_name[i])
			   			i++;
					if(permissions[9] == 't')
					{
						write(1, "\x1b[1;42;30m", 10);
		   				write(1, (char *)d->d_name, i);
						write(1, "\x1b[0m", 5);
					}
					else if(permissions[0] == 'd'){
						write(1, "\x1b[1;34m", 8);
						write(1, (char *)d->d_name, i);
						write(1, "\x1b[0m", 5);
					}
					else if(permissions[0] == 'l')
					{
						write(1, "\x1b[1;36m", 8);
						write(1, (char *)d->d_name, i);
						write(1, "\x1b[0m", 5);
						char buf[1024];
						int count = readlink(final, buf, sizeof(buf));
						if(count >= 0){
							buf[count] = '\0';
							write(1, "->", 2);
							write(1, buf, count);
						}
					}
					else if(permissions[3] == 'x')
					{
						write(1, "\x1b[1;32m", 8);
		   				write(1, (char *)d->d_name, i);
						write(1, "\x1b[0m", 5);
					}
					else
		   				write(1, (char *)d->d_name, i);
					write(1, "\n", 2);
				}
           			bpos += d->d_reclen;
			}
		}
	}
	}
   	exit(EXIT_SUCCESS);
}
