#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <pthread.h> 
#include <semaphore.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/sem.h>
#define ANY 0


sem_t mutex;

struct id_semget
{
	int	id;

} shared;

void* thread_posix(void* arg); 
void *thread_sysetemV(void *arg);
static struct sembuf lock ={ 0, -1, 0};
static struct sembuf unlock ={ 0, 1, 0};   
int main(int argc, char **argv)  
{   
    
    if(argc <= 1) 
    {
         printf("plz use  \n\t%s --posix\n\tor\n\t%s --systemV \n",argv[0],argv[0]);
         return 0;
    }

    int zer=0;
    printf("ПЖ введите число:");
    scanf("%d",&zer);

    if (zer>10000)
    {
        printf("введите число меньше 10000");
        scanf("%d",&zer);
    }    

    if(!strcmp(argv[1], "--posix"))
    {
        sem_init(&mutex, 0, 1);
        
        pthread_t th_posix[zer];
       
        for(int c=0;c<=zer;c++ )
        {
            pthread_create( &th_posix[c] ,NULL, thread_posix, &c); 
        }
        for(int j=0; j<=zer; j++ )
        {
        pthread_join(th_posix[j],NULL); 
        }

        sem_destroy(&mutex); 

    }
    
     if(!strcmp(argv[1], "--systemV"))
    {   
        
        int sem_key;
        key_t semkey;
        if ((semkey = ftok(argv[0], 'a')) == (key_t) -1) 
        {
        perror("EROR--ftor"); exit(1);
        }
        shared.id = semget(semkey, 1, IPC_CREAT | 0666);
      
        int  rtn = semctl (shared.id, 1, GETVAL, ANY);
        printf("\n%d\n",rtn);
        pthread_t th_systemV[zer];
       
        if ( semop(shared.id,&unlock,1)== -1) 
        {
            perror("semop unlock EROR main");
            exit(1);
        }
       
        for(int c_s=0; c_s <= zer; c_s++ )
        {
            pthread_create( &th_systemV[ c_s ], NULL, thread_sysetemV, &c_s); 
        } 
        for( int j_s=0; j_s <= zer; j_s++ )
        {
        pthread_join(th_systemV[ j_s ],NULL); 
        }

            
    }

}

void *thread_posix(void *arg) 
{ 
    int *i =(int*)arg;
    int z=*i;
    
    int value=0;
    int br=0;

    while(1)
    {
        sem_getvalue(&mutex, &value);
        if (value==0)
        {
            sleep(1);
        }

        if (value==1)
        {
            sem_wait(&mutex); 
            sem_getvalue(&mutex, &value); 
            printf("\nЗапускаем и чтото тут делаем \"thread №%d \" \n",z); 
            printf("sem_getvalue = %d\n",value);

            sleep(1); 
            br=1;
            printf("\nexit thread №%d\n",z); 
            sem_post(&mutex); 
        }
        
        if(br==1)
        {
        break;
        }
    
    }    
    
    
} 

void *thread_sysetemV(void *arg)
{
    int *i_s =(int*)arg;
    int z_s=*i_s;
    int br_s=0;
    while(1)
    {   
        if(semctl (shared.id, 1, GETVAL, ANY)==-1)
        {
            if(semop(shared.id,&lock,1) == -1)
            {
             perror("semop Lock ERROE");
             exit(1);
            }
            printf("\nЗапускаем и чтото тут делаем \"thread №%d \" \n",z_s); 
        

            sleep(1); 
            br_s=1;

            printf("\nexit thread №%d\n",z_s);

            if ( semop(shared.id,&unlock,1)== -1) 
            {
                perror("semop unlock EROR");
                exit(1);
            }
            if(br_s==1)
            {
            break;
            }
        }
       
              
    }
}