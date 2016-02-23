#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TSTBUFSIZE 64
void main (void)
{
	int fdwr=-1;
	char bufwr[64]={0};
	int ret=0;
	int lenw=0;
	int i=1;
	int pid=fork();
	if(pid!=0)
	{
		while(1){
			fdwr= open("./rdwrtestfile",O_CREAT|O_RDWR,S_IRWXU|S_IRWXG);

			if(fdwr<0)
			{
				ret=1;
				goto openfilefail;
			}
			if(++i>9)
			{
			      i=0;
			}

			//i=1;
			memset(bufwr,'0'+i,TSTBUFSIZE);

			lenw=lseek(fdwr,TSTBUFSIZE/*i*/,SEEK_SET);
			if(lenw!=TSTBUFSIZE/*i*/)
			{
				ret=2;
				goto openfilefail;
			}
			lenw=write(fdwr,bufwr,TSTBUFSIZE);
			if(lenw!=TSTBUFSIZE)
			{
				ret=3;
				goto openfilefail;
			}
			close(fdwr);
			printf("write test success\n");
			sleep(1);
		}

openfilefail:
		close(fdwr);
		printf("rdwrtest FAIL:%d,%d\n",ret,i);
		perror("wwresult:");
		return;
	}
	else
	{
		while(1)
		{
			fdwr= open("./rdwrtestfile",O_RDWR,0);

			if(fdwr<0)
			{
				ret=1;
				goto readfilefail;
			}

			
			memset(bufwr,0,TSTBUFSIZE);
			if(++i>9)
			{
			      i=0;
			}
			i=1;
			lenw=lseek(fdwr,TSTBUFSIZE*i,SEEK_SET);
			if(lenw!=TSTBUFSIZE*i)
			{
				ret=2;
				goto readfilefail;
			}
			lenw=read(fdwr,bufwr,TSTBUFSIZE);
			if(lenw!=TSTBUFSIZE)
			{
				ret=3;
				goto readfilefail;
			}
			close(fdwr);
			bufwr[TSTBUFSIZE-1]='\0';
			printf("read test success:%s\n",bufwr);
			sleep(1);
		}
readfilefail:
		close(fdwr);
		printf("readtest FAIL:%d,%d\n",ret,i);
		perror("rrresult:");
		return;
	}


}

