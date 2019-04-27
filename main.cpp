#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wininet.h>
#include <vector>
#define ASSERT(cond,do) if(cond){do;return 0;}
#define API_URL "https://www.duitang.com/napi/blog/list/by_album/?album_id=%d&limit=24&start=%d"
//"https://www.duitang.com/napi/blog/list/by_album/?album_id=92480357&limit=24"
using namespace std;

int GetUrl(char *url, char *buf, int *length){
    //char buf[1024];
    long unsigned int readDataLength;
    HINTERNET hinternet=InternetOpen(0,INTERNET_OPEN_TYPE_PRECONFIG,0,0,0);
    ASSERT(!hinternet,InternetCloseHandle(hinternet));
    HINTERNET hinternetopen=InternetOpenUrl(hinternet,url,0,0,INTERNET_FLAG_NO_CACHE_WRITE,0);
    ASSERT(!hinternetopen,InternetCloseHandle(hinternet);InternetCloseHandle(hinternetopen));
    *length=0;
    do{
        InternetReadFile(hinternetopen,buf+*length,1024,&readDataLength);
        *length+=readDataLength;
    }while(readDataLength);
    return 1;
}

int GetFile(char *url, char *file){
    int length=0;
    char buf[1024];
    long unsigned int readDataLength;
    FILE *fp;
    HINTERNET hinternet=InternetOpen(0,INTERNET_OPEN_TYPE_PRECONFIG,0,0,0);
    ASSERT(!hinternet,InternetCloseHandle(hinternet));
    HINTERNET hinternetopen=InternetOpenUrl(hinternet,url,0,0,INTERNET_FLAG_NO_CACHE_WRITE,0);
    ASSERT(!hinternetopen,InternetCloseHandle(hinternet);InternetCloseHandle(hinternetopen));
    length=0;
    fp=fopen(file,"wb");
    ASSERT(!fp,InternetCloseHandle(hinternetopen);InternetCloseHandle(hinternet));
    do{
        InternetReadFile(hinternetopen,buf,1024,&readDataLength);
        fwrite(buf,1,readDataLength,fp);
        length+=readDataLength;
    }while(readDataLength);
    InternetCloseHandle(hinternetopen);
    InternetCloseHandle(hinternet);
    fclose(fp);
    return 1;
}

int GetFileCount(char *buf,int *next){
    char *p;
    int total=0;
    if(p=strstr(buf,"total")){
        total=atoi(p+7);
        p=strstr(buf+7,"next_start");
        *next=atoi(p+12);
    }
    return total;
}

int GetFileName(char *name,char *buf,char *dest){
    char *last,*p,*q=buf;
    if(dest[1]==':'){
        p=dest;
        while(*q++=*p++);
        if(*(q-2)!='/'&&*(q-2)!='\\'){
            *(q-1)='/';
        }
    }
    for(p=name;*p;p++){
        if(*p=='/')
            last=p;
    }
    last++;
    while(*q++=*last++);
    //puts(buf);
    return 1;
}

void Parse(char *buf,int *now,int total,char *dest){
    char result[1024],name[1024];
    char *p,*q;
    for(p=buf;*p;p++){
        if(!strncmp(p,"path",4)){
            p+=12;
            strcpy(result,"http");
            for(q=result+4;*p!='"';p++,q++){
                *q=*p;
            }
            *q=0;
            //puts(result);
            GetFileName(result,name,dest);
            GetFile(result,name);
            printf("[%4d/%4d] %s\n",++*now,total,name);
        }
    }
}

int main(int argc, char* argv[])
{
    char buf[20480],url[1024],dest[1024];
    int length,total,next=0,now=0,id=79440637;
    //system("chcp 65001 > nul");
    printf("Input album_id   : ");
    scanf("%d",&id);
    fflush(stdin);
    printf("Input destination: ");
    scanf("%[^\n]",dest);
    do{
        sprintf(url,API_URL,id,next);
        GetUrl(url,buf,&length);
        total=GetFileCount(buf,&next);
        Parse(buf,&now,total,dest);
    }while(next<total);
    //printf("%d\n",GetFileCount(buf,&next));
    //Parse(buf);
    //GetFile("http://b-ssl.duitang.com/uploads/item/201903/27/2019032790442_CVdml.jpeg","2.jpeg");
    //FILE *fp=fopen("1.jpeg","wb");
    //fwrite(buf,1,length,fp);
    //fclose(fp);
    return 0;
}
