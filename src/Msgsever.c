//
// Created by syy on 18-10-11.
//
# include <stdio.h>
# include <string.h>
# include <errno.h>
# include <unistd.h>
# include <sys/ipc.h>
# include <sys/msg.h>
# include <sys/types.h>
# define MAX_MSG_LEN 1024
typedef struct MSGQUEUE
{
    long msgType;
    char msgText[MAX_MSG_LEN];

}Msg;
int main(void)
{
    char *msgpath="/home/ubuntu/h17/ipc/";
    key_t key=ftok(msgpath,3);
    int ret;
    int msqid;
    Msg serverMsg;
    /*msgget*/
    msqid = msgget((key_t)key, 0666|IPC_CREAT);
    if( msqid == -1)
    {
        if(errno == EEXIST)
        {
            /*Message queue has aleardy existed */
            printf("msgget() warning: %s\n", strerror(errno));
            msqid = msgget((key_t)key, 0666|IPC_CREAT); /*access the mq*/
            if(msqid == -1)
            {
                printf("msgget() error: %s\n", strerror(errno));
                return -1;
            }

        }
        else
        {
            /*msgget error*/
            printf("msgget() error: %s\n", strerror(errno));
            return -1;
        }
    }
    printf("msgget() success. shmid=[%d]\n", msqid);



    /*msgsnd*/
    while(1)
    {
        /*init serverMsg using default msgType 1*/
        serverMsg.msgType = 1;
        memset(serverMsg.msgText, 0x00, sizeof(serverMsg.msgText));

        /*input*/
        printf("serverMsg.msgText: ");
        if(fgets(serverMsg.msgText, MAX_MSG_LEN,stdin)== NULL)
        {
            printf("gets() encounters an error or EOF\n");
            fflush(stdin);
            break;
        }
        fflush(stdin);
        clearerr(stdin);
        printf("\n");

        ret = msgsnd(msqid, (void*)(&serverMsg), sizeof(serverMsg.msgText), 0);  /*BLOCK*/
        if(ret !=0 )
        {
            printf("msgsnd() error: %s\n", strerror(errno));
            break;
        }

        /*exit*/
        if(memcmp(serverMsg.msgText, "end", 3) == 0)
        {
            printf("over and exit\n");
            break;
        }

        sleep(0.1);
    }

    /*msgctl IPC_RMID done by client*/

    return 0;
}
