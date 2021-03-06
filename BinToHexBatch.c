#include <stdio.h>
#include <string.h>
//#include "syscalls.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"

#define MAX_PATH 1024
//#define NAME_MAX 256
typedef struct {
	long ino;
	char name[NAME_MAX];

} Dirent;

typedef struct { 
	int fd;
	Dirent d;
} Dir;


void dirwalk(char *dir,void(*fcn)(char*))
{
	char name[MAX_PATH];
	struct dirent *dp;
	DIR*dfd;
	
	if((dfd = opendir(dir))==NULL){
		fprintf(stderr,"dirwalk:can't open %s\n",dir);
		return;
	}

	while((dp=readdir(dfd))!=NULL){
		if(strcmp(dp->d_name,".")==0||strcmp(dp->d_name,"..")==0)
			continue;
		if(strlen(dir)+strlen(dp->d_name)+2>sizeof(name))
			fprintf(stderr,"dirwalk:name %s%s too long\n",dir,dp->d_name);
		else{
			sprintf(name,"%s/%s",dir,dp->d_name);
			(*fcn)(name);
		}

	}
	closedir(dfd);
}

void replaceadix(char* src,char *dst)
{
	int i = 0;
	int len = strlen(src);
	i = len;
	while(i-->0) {
		if(src[i]=='.'){
			break;
		}
	};
	if(i==0)i=len;
	strncpy(dst,src,i);
	dst[i]='\0';
	strcat(dst,".h");
	i = 0;
	while(dst[i]!='\0'){
		if(dst[i]=='-')
			dst[i]='_';
		i++;
	}
	printf("%s\n",dst);
}


void bintohex(char* file)
{
	printf("%s\n",file);	
	FILE* fs,*fd;
	char buf[256];
	char dstfile[256];
	char ch;
	int i = 0;
	replaceadix(file,dstfile);
	if((fs=fopen(file,"r"))==NULL){
		printf("%s openfile %s err\n",__FUNCTION__,file);
		goto fail;
	}
	if((fd=fopen(dstfile,"w+"))==NULL){
		printf("%s openfile %s err\n",__FUNCTION__,dstfile);
		goto fail;
	}
	while(!(feof(fs))){
		ch=fgetc(fs);
		if(i++%32==0){
			fprintf(fd,"\n");
		}
		fprintf(fd,"0x%02x,",(unsigned char)ch);
	};

	fflush(fd);
	fclose(fs);
	fclose(fd);

fail:
	return;
}
void createcode(char* name)
{
	int i =0;
	int len = strlen(name);
	char dstname[256];	
	FILE *fd;
	char *p,*lp;
	
	if(!strcasestr(name,".h"))
		goto fail;
	
	p = strtok(name,"/");
	if(p){
	    lp = p;
	    while((p = strtok(NULL,"/"))!=NULL){
		    lp = p;
	    }
	}else
		goto fail;
	
	i = strlen(lp);
	while(i-->0) {
		if(lp[i]=='.'){
			break;
		}
	};
	strncpy(dstname,lp,i);
	dstname[i]='\0';
	
	
	
	if((fd=fopen("./target.h","a"))==NULL)
		goto fail;

	fprintf(fd,"const unsigned char %s[] = {\n#include \"%s\"\n};\n",dstname,lp);

	fflush(fd);
	fclose(fd);
	
	if((fd=fopen("./target2.h","a"))==NULL)
		goto fail;

	fprintf(fd,"{\n    .data = %s,\n    .len = sizeof(%s)\n},\n",dstname,dstname);
	fclose(fd);
fail:
	return;
}

void hextobin(char* file)
{
	//printf("%s\n",file);	
	FILE* fs,*fd;
	char buf[256];
	char dstfile[256];
	char ch;
	int i = 0;

	file="./RGB.txt";
	strcpy(dstfile,"./RGB.bmp");
	//replaceadix(file,dstfile);
	if((fs=fopen(file,"r"))==NULL){
		printf("%s openfile %s err\n",__FUNCTION__,file);
		goto fail;
	}
	if((fd=fopen(dstfile,"w+"))==NULL){
		printf("%s openfile %s err\n",__FUNCTION__,dstfile);
		goto fail;
	}
	while(!(feof(fs))){
		if(fscanf(fs, "%x",&i))
   			 printf("The integer read was:%x\n", i);
    		else
    		{
        		fprintf(stderr, "Error reading an integer from stdin.\n");
        		exit(1);
    		}
		memcpy(buf,&i,4);
		i=fwrite(buf,4,1,fd);
		printf("write data=%d\n",i);
	};

	fflush(fd);
	fclose(fs);
	fclose(fd);

fail:
	return;
}

void main( int argc, char *argv[])
{
	//hextobin(NULL);
	dirwalk(".",bintohex);
	dirwalk(".",createcode);
}

