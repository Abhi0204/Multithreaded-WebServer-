//Declaring all the HeaderFiles

#include <iostream>
#include <vector>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<list>
#include <algorithm>  
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <queue>
#include <vector>
#include <semaphore.h>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fstream>
#include <errno.h>
#include<iomanip>
#include<cctype>


using namespace std;

char cwd[1024];
int client_connect;
int thread_num=10;
int debugflag=0;
pthread_mutex_t mutex;
char RootPath[1024];
char logging_valuepath[200];
const int no_of_t=100;
bool logging_value=false;
char *dir,*logfile;
bool busy;
int sleep_time;
int flag_Sched=0;
sem_t mutex_JobQueue;

pthread_t listening_thread, scheduling_thread,threadID[100];
struct sockaddr_in server_addr,client_addr;
int client_s;
socklen_t len;
int socketfd;
int flag=1,listen_port=8080,queuing_time=60;


//Class Created to hold all the information regarding the incoming JobRequest.
class JobRequest       
{
        public:
        int type,status,ip_addr;
        char *IncomingTime, *ScheduleTime,*buffers;

        char *filename;
        string path;
         char ClientPath[100];
        string requestP;
        char *request_Header;
        int filesize, client_connect;
        char *LastModifiedTime;
        char *Header;
};

JobRequest execution_process[no_of_t];
JobRequest client_request;
JobRequest debugging;
list<JobRequest>mylist;
queue<JobRequest>readyQueue;
int threadSignals[no_of_t];
JobRequest execution_Request[no_of_t];
JobRequest processrequest;

//A function created for list to sort all the JobRequests according to the file Size.

        bool compare(JobRequest &fz1, JobRequest &fz2)
        {

            return fz1.filesize > fz2.filesize;
             

        }
//Thread class created to hold the information about threads
class thread
{
 public:

        bool occupied;
        pthread_attr_t attributes;
        thread()
                {
                        occupied=false;
                        pthread_attr_init(&attributes);
                }
pthread_attr_t attr;
};
thread thread_pool[no_of_t];



//Function created to fetch all the files in the current Searching Directory
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back((dirp->d_name));
    }
    closedir(dp);
    return 0;
}


//structure created to sort the files in the Directory alphabetically
struct FileCompare 
{
  bool operator() (string i,string j) 
    { 
        return (i<j);
    }
} myobj;


//Serving Request Worker Thread

void *thread_serve(void* attr)
{

const char* d;

        int *i_des;
        i_des=(int*) attr;
  time_t it;
   struct stat buffer;

              stat((const char*)execution_Request[*i_des].ClientPath, &buffer);
              time(&it);
              time_t now = time(NULL);
              execution_Request[*i_des].ScheduleTime=asctime( gmtime((const time_t*)&now));
        while(1)
        {
              char *TimeNow,LastModifiedTime[40]; time_t t;
              char line[1024];FILE *f,*tmp;
              char header[1024];
           
              
                                                

                memset(header,'\0', sizeof(header));
                while(threadSignals[*i_des]==0);
if(execution_Request[*i_des].status == 404)
                {
                        
                      
                 vector<string>files=vector<string>();
                 getdir(RootPath,files);
std::sort (files.begin(),files.end(), myobj); 
const char *buffer;
send(execution_Request[*i_des].client_connect,"FILE NOT FOUND!!! 404 ERROR\n\n\n\n",(strlen("FILE NOT FOUND!!! 404 ERROR\n\n\n\n")),0);
send(execution_Request[*i_des].client_connect,"Files in The Directory\n\n\n\n",(strlen("Files in The Directory\n\n\n\n")),0);

for(int i=0;i<files.size();i++)
{
buffer=files[i].c_str();
send(execution_Request[*i_des].client_connect, buffer,(strlen(buffer)),0);
send(execution_Request[*i_des].client_connect,"\n",(strlen("\n")),0);
}

             }
                

              else if(execution_Request[*i_des].status==200)
                {  
                       
                    if(execution_Request[*i_des].type==1)

                    {
                        time(&t);
                        TimeNow=asctime(gmtime(&t));
                        sprintf(header,"HTTP/1.1 200 OK \n Date: %s Server: JUNGZO\n Last Modified: %s Content-Type:text/html/jpg\n Content-Length:%d\n\n\n\n",TimeNow,LastModifiedTime,execution_Request[*i_des].filesize);
                                write(execution_Request[*i_des].client_connect,header,strlen(header));

                                f=fopen(execution_Request[*i_des].ClientPath,"r");
                                {
                        


                                    if(f!=NULL)
                                    {
                                         while (fgets(line,sizeof(line), f ) != NULL )
                                        {

                                        write(execution_Request[*i_des].client_connect,line,strlen(line));

                                     }
                                fclose(f);

                       
                                    }
                     

                               }
               }

else  if(execution_Request[*i_des].type==2)

                    {
                        time(&t);
                        TimeNow=asctime(gmtime(&t));

                          sprintf(header,"HTTP/1.1 200 OK\nDate: %sServer: JUNGZO\nLast-Modified: %sContent-Type: text/html/jpg\nContent-Length: %d\r\n\r\n",TimeNow,execution_Request[*i_des].LastModifiedTime,execution_Request[*i_des].filesize);
                                write(execution_Request[*i_des].client_connect,header,strlen(header));

tmp=fopen(execution_Request[*i_des].ClientPath,"r");

                                 if(tmp!=NULL)
                                {
 
                                        fseek(tmp,0,SEEK_END);
                                        execution_Request[*i_des].filesize=ftell(tmp);
                                }

                                fclose(tmp);
                                
         
                              
                     }

else
{

 write(execution_Request[*i_des].client_connect, "Invalid Request! Only HTTP GET and HEAD are served",sizeof("Invalid Request! Only HTTP GET and HEAD are served here!"));
}
                 


                } 

                        if(logging_value==true)
                        {

                            tmp=fopen(logging_valuepath,"a");
                            if(tmp!=NULL)
                            {


unsigned char bytes[4];
char result[100];

char ip[16];
  sprintf(result, "%d.%d.%d.%d",
    (execution_Request[*i_des].ip_addr >> 24) & 0xFF,
    (execution_Request[*i_des].ip_addr >> 16) & 0xFF,
    (execution_Request[*i_des].ip_addr >>  8) & 0xFF,
    (execution_Request[*i_des].ip_addr      ) & 0xFF);

execution_Request[*i_des].Header=strtok(execution_Request[*i_des].buffers,"\n\n");
const char* d=(execution_Request[*i_des].path).c_str();
strcat(execution_Request[*i_des].Header,d);


inet_ntop(client_addr.sin_family,(struct sockaddr_in*)&client_addr.sin_addr,result,sizeof(result));

fprintf(tmp," Client Ip Address: %s \n Request Received Time:[%s]\n Request Schedule Time:[%s]\n Header: \"%s\"\n Status: %d\n File Size: %d\n\n",result,execution_Request[*i_des].IncomingTime,execution_Request[*i_des].ScheduleTime,execution_Request[*i_des].Header,execution_Request[*i_des].status,execution_Request[*i_des].filesize);
                        fclose(tmp);

                            }

                        }
                        close(execution_Request[*i_des].client_connect);

threadSignals[*i_des]=0;
thread_pool[*i_des].occupied=false;
busy=false;



        }

}


//Debug Request!! Fulfilling one request at a time.
void callDebug()
{

std::cout<<"***********************************************************************************************************************************************\n"<<std::endl;
std::cout<<"Debugging Mode::OPEN\n\n"<<std::endl;
              
              const char* d;
      
              char *TimeNow,LastModifiedTime[40]; time_t t;
              char line[1024];FILE *f,*tmp;
              char header[1023];
              struct stat buffer;
              time_t it;
              stat((const char*)debugging.ClientPath, &buffer);
              time(&it);
              time_t now = time(NULL);
              debugging.IncomingTime=asctime(gmtime(&it));
              debugging.ScheduleTime=asctime(gmtime(&it));
              memset(header,'\0', sizeof(header));

                     if(debugging.status == 404)
                        {
                             std::cout<<"File Not Found!!! 404 Error"<<std::endl;
                             std::cout<<"File Size is--->"<<debugging.filesize<<std::endl;
                             vector<string>files=vector<string>();
                             getdir(RootPath,files);

                             const char *buffer;
                       send(debugging.client_connect,"FILE NOT FOUND!!! 404 ERROR\n\n\n\n",(strlen("FILE NOT FOUND!!! 404 ERROR\n\n\n\n")),0);
                       send(debugging.client_connect,"Files in The Directory\n\n\n\n",(strlen("Files in The Directory\n\n\n\n")),0);

                                    for(int i=0;i<files.size();i++)
                                        {
                                            std::cout<<files[i]<<std::endl;
                                        }
                                     std::cout<<"\n"<<std::endl;
                            
 
                                   for(int i=0;i<files.size();i++)
                                       {
                                          buffer=files[i].c_str();
                                          send(debugging.client_connect, buffer,(strlen(buffer)),0);
                                          send(debugging.client_connect,"\n",(strlen("\n")),0);
                                       }
                
                        }


                              else if(debugging.status==200)
                            {  
                                 std::cout<<"HTTP Status----->"<<debugging.status<<std::endl;
                       
                                    if(debugging.type==1)

                                    {    
                                       std::cout<<"Request::GET\n\n"<<std::endl;
                                       time(&t);
                                       TimeNow=asctime(gmtime(&t));
                                       sprintf(header," HTTP/1.1 200 OK \n Date: %s Server: JUNGZO\n Last Modified: %s Content-Type:text/html/jpg\n Content-Length:%d\n\n\n\n",TimeNow,debugging.LastModifiedTime,debugging.filesize);
                                       write(debugging.client_connect,header,strlen(header));

                                                 f=fopen(debugging.ClientPath,"r");
                                                    {
                        


                                                          if(f!=NULL)
                                                              {
                                                                  while (fgets(line,sizeof(line), f ) != NULL )
                                                                    {

                                                                         write(debugging.client_connect,line,strlen(line));

                                                                     }
                                                                              fclose(f);

                        
                                                               }
                     

                                                        }
                                                   std::cout<<"METADATA\n\n"<<header<<std::endl;
                              
                                     }


                                                   else  if(debugging.type==2)

                                                     {

                                                        std::cout<<"Request::HEAD\n"<<std::endl;
                                                        time(&t);
                                                        TimeNow=asctime(gmtime(&t));
                                                        std::cout<<"Current Time is::"<<TimeNow<<std::endl;
                                                        sprintf(header," HTTP/1.1 200 OK\nDate: %sServer: JUNGZO\nLast-Modified: %sContent-Type: text/html/jpg\nContent-Length: %d\r\n\r\n",TimeNow,debugging.LastModifiedTime,debugging.filesize);
                                                        write(debugging.client_connect,header,strlen(header));

                                                         tmp=fopen(debugging.ClientPath,"r");

                                                              if(tmp!=NULL)
                                                                  {
 
                                                                         fseek(tmp,0,SEEK_END);
                                                                         debugging.filesize=ftell(tmp);
                                                                  }

                                                              fclose(tmp);
                                
         
                                                              std::cout<<"Metadata\n\n"<<header<<std::endl;


                                                      }

                          }

                                   std::cout<<"LOGGING ON SERVER\n\n\n"<<std::endl;
                                   unsigned char bytes[4];
                                   char result[16];

                                   inet_ntop(client_addr.sin_family,(struct sockaddr_in*)&client_addr.sin_addr,result,sizeof(result));
                                   printf(" Client Ip Address: %s \n Request Received Time:[%s]\n Request Schedule Time:[%s]\n Header: \"%s\"\n Status: %d\n File Size: %d\n\n",result,debugging.IncomingTime,debugging.ScheduleTime,debugging.Header,debugging.status,debugging.filesize);
                                   std::cout<<"ThreadPool created of "<<thread_num<<" threads"<<std::endl;
                                   std::cout<<"Client Path is ::"<<debugging.ClientPath;
                                   std::cout<<"\n"<<std::endl;

                                   if(flag_Sched==1)
                                      {
                                        std::cout<<"Scheduling Policy:: SJF\n"<<std::endl;
                                      }
                                       else
                                           {
                                             std::cout<<"Scheduling Policy:: FCFS\n"<<std::endl;
                                           }
                                             std::cout<<"Debugging Mode::CLOSE"<<std::endl;

                       std::cout<<"***********************************************************************************************************************************************\n"<<std::endl;
close(debugging.client_connect);
}



//Listening to Requests from the client

void *thread_listen(void* attr)
{
          socklen_t len=sizeof(client_addr);
          sem_init(&mutex_JobQueue,0,1);
          int n;char buff[1024];
          time_t it;
          client_request.type=0;
          time_t now = time(NULL);
          int count=0;

while(1)
        {
                            FILE *file;struct stat buffer;
                     
   client_request.client_connect =accept(socketfd,(struct sockaddr *)&client_addr,&len);


                             if(client_s<0)
                               {
                                   std::cout<<"Request cannot be accepted\n";
                               }
                            else

                n=read(client_request.client_connect,buff,1023);

                client_request.buffers = (char*)buff;
                client_request.Header=strtok(buff," ");

                client_request.path=strtok(NULL," ");

                client_request.request_Header=strtok(NULL,"\n");

                
                   if(client_request.path.length()==1)
                         {
                              client_request.filesize=0;
                              client_request.status=404;
                         }
                            else
                                {

                                     if(!strcmp(client_request.Header,"GET")) 
                                        {
                                              client_request.type=1;
                                        }
 
                                             else if(!strcmp(client_request.Header,"HEAD"))
                                                    {
                                                         client_request.type=2;
                                                         client_request.filesize=0;
                                                     }
                                                          else
                                                             {
                                                                 client_request.type=-1;

                                                             }

                client_request.ip_addr=(&client_addr)->sin_addr.s_addr;

                   for (int i = 0; i < (client_request.path).length(); i++)
                         {
                            if (client_request.path[i] == '\n' || client_request.path[i] == '\r' )
                                client_request.path[i] = '\0';
                         } 
                 
                 std::size_t found = client_request.path.find("~");
                
                           if (found!=std::string::npos)

                        {    
                             if (getcwd(cwd, sizeof(cwd)) != NULL)


                             client_request.path.erase (0,1);
                             strcpy(client_request.ClientPath,cwd);
                          const char* e = (client_request.path).c_str();
                             strcat(client_request.ClientPath,e);



                        }
                            else 
                                {
                                   strcpy(client_request.ClientPath,RootPath);
                                   const char* e = (client_request.path).c_str();
                                   strcat(client_request.ClientPath,e);
                                }
      

              file=fopen(client_request.ClientPath,"r");
 
                   if(file!= NULL)
 
                       {
                            fseek(file,0,SEEK_END);//seek till the end of the file
                            client_request.filesize=ftell(file);
                            client_request.status=200;
 
                            fclose (file);


                       }
                          else if(file==NULL)
                             {
                                 client_request.status = 404;
                                 client_request.filesize=0;
                             }
                                 
                         }


              stat((const char*)client_request.ClientPath, &buffer);
              time(&it);
              client_request.IncomingTime=asctime(gmtime(&buffer.st_mtime));;
              client_request.LastModifiedTime=asctime(gmtime(&it));

          if(debugflag==1)
            {
                 debugging=client_request;
                 callDebug();
            }
                   else
                       {
                          sem_wait(&mutex_JobQueue);

                        if (flag_Sched == 0)
                           {
                              readyQueue.push(client_request);
                           }
                            else if(flag_Sched == 1)
                                 {

                                     mylist.push_back(client_request);
                              
                                 }

                             sem_post(&mutex_JobQueue);

                       }


}

}


//Scheduler Thread/Will wait for t seconds for the requests to be stored in the ready Queue.
void *thread_scheduler(void* attr)
{
sleep(queuing_time);

   time_t st;
  int sched_queue=*((int*)attr);
int i;

while(1)
{    sem_wait(&mutex_JobQueue);



    if(!readyQueue.empty()||!mylist.empty())
    {
        if(sched_queue==0)
        {
            processrequest=readyQueue.front();
            readyQueue.pop();

        }
        else if(sched_queue==1)
        {
            mylist.sort(compare);
            processrequest=mylist.front();
            mylist.pop_front();

        }
             sem_post(&mutex_JobQueue);
}




//Main Function Execution
int main(int argc,char** argv)

{

int checks;




//Getting Command Line Arguements
for(int i=1;i<argc;i++)
{

                   if(strcmp(argv[i],"-d")==0)
                   {
                     debugflag=1;
                   }

                      else if(strcmp(argv[i],"-h")==0)
                    {
                      std::cout<<"myhttpd" <<std::endl << "usage: ./myhttpd [-d][-h]"
                        << " [-l file] [-p port] [-r dir] [-t time] "
                        << "[-n threadnum] [-s sched]" << std::endl << std::endl<<std::endl

                        << "  -d            : Enter debugging mode. That is, do not daemonize, only accept one client_connect at a"
                        <<std::endl<<"                  time and enable logging_value to stdout.Without this option, the web server should run as"
                        <<std::endl<<"                  a daemon process  in the background."
                        <<std::endl
                        << "  −h            : Print a usage summary with all options and exit."
                        << std::endl
                        << "  −l file       : Log all requests to the given file."
                        << std::endl
                        << "  −p port       : Listen on the given port. If not provided,"
                        << std::endl << "                  myhttpd will listen on port 8080."
                        << std::endl
                        << "  −r dir        : Set the root directory for the http server"
                        << std::endl << "                  to dir."
                        << std::endl
                        << "  −t time       : Set the queuing time to time seconds."
                        << std::endl << "                  The default is 60 seconds."
                        << std::endl
                        << "  −n threadnum  : Set number of threads waiting ready in the"
                        << std::endl << "                  execution thread pool to threadnum."
                        << std::endl << "                  The default is 4 execution threads."
                        << std::endl
                        << "  −s sched      : Set the scheduling policy. It can be either"
                        << std::endl << "                  FCFS or SJF. The default will be FCFS"
                        << std::endl;

                     }

                  else if(strcmp(argv[i],"-p")==0)
                      {
                       listen_port=atoi(argv[i+1]);
                      }

                 else if(strcmp(argv[i],"-t")==0)
                     {
                       queuing_time=atoi(argv[i+1]);
                     }

                 else if(strcmp(argv[i],"-n")==0)
                     {
                       thread_num=atoi(argv[i+1]);

                     }

                 else if(strcmp(argv[i],"-s")==0)
                     {
                        if(!strcmp(argv[i+1],"SJF"))
                          {
                                flag_Sched=1;
                          }
                             else if(!strcmp(argv[i+1],"FCFS"))
                             {
                                 flag_Sched=0;
                             }
                     }

                   else if(!strcmp(argv[i],"-r"))
                {

                        strcpy(RootPath,argv[i+1]);
                }

                  else if(!strcmp(argv[i],"-l"))
                 {      logging_value=true;
                        logfile=argv[i+1];
                        strcpy(logging_valuepath,cwd);
                        strcat(logging_valuepath,logfile);


                 }
}

//Socket Programming Code.

                        socketfd=socket(AF_INET,SOCK_STREAM,0);

                        if((socketfd)<0)
                        {
                                std::cout<<"Socket not Created";
                                exit(1);
                        }
			else
			{
                                bzero((char*)&server_addr,sizeof(server_addr));
                                server_addr.sin_family=AF_INET; 
                                server_addr.sin_addr.s_addr = INADDR_ANY;
                                server_addr.sin_port = htons(listen_port);


			}
                        int pk=setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(int));
                        int ret=bind(socketfd,(struct sockaddr*)&server_addr,sizeof(server_addr));

                        if(ret<0)
                        {
                                perror("Binding Unsuccessful");
                                exit(1);
                        }
                        else if(ret==0)
                        {
                                 std::cout<<"Binding Successfull"<<std::endl;
                         }



                        if(listen(socketfd,thread_num)==-1)
                        {
                                perror("Listening Unsuccessful");
                                exit(1);
                        }


                                std::cout<<"Server Listening"<<std::endl;



//End of Socket Code





	pthread_create(&(listening_thread),NULL,&thread_listen,&checks);			//creating listener thread

								// putting scheduler to sleep
	pthread_create(&(scheduling_thread),NULL,&thread_scheduler,&flag_Sched);	//creating scheduler thread

                int p[thread_num];
                for(int j=0;j<thread_num;j++)
	        {
                    p[j]=j;
		    pthread_create(&(threadID[j]),NULL,&thread_serve,&(p[j]));
 
                }




                for(int k=0;k<thread_num;k++)
                {
                      pthread_join((threadID[k]), NULL);
                } 



        pthread_join(listening_thread,NULL);
	pthread_join(scheduling_thread,NULL);

	return 0;
}

