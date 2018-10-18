/*******************************************************************************
 * Copyright (c) 2012, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
# include <errno.h>
# include <unistd.h>
# include <sys/ipc.h>
# include <sys/msg.h>
# include <sys/types.h>
# include "msq.h"
#define ADDRESS     "tcp://10.108.67.72:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "topic01"
#define QOS         1
#define TIMEOUT     10000L
int main(int argc, char* argv[])
{
    int ret;
    int msqid;
    int recv_type = 1;
    Msg clientMsg;
    struct msqid_ds ds_buf;
    /*msgget*/
    msqid = msgget((key_t)MSQ_KEY, 0660|IPC_CREAT|IPC_EXCL);
    if( msqid == -1)
    {
        if(errno == EEXIST)
        {
            /*Message queue has aleardy existed */
            printf("msgget() warning: %s\n", strerror(errno));
            msqid = msgget((key_t)MSQ_KEY, 0660|IPC_CREAT); /*access the mq*/
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
    /*msgrcv*/
    while(1) {
        /*recv_type=1*/
        memset(clientMsg.msgText, 0x00, sizeof(clientMsg.msgText));

        ret = msgrcv(msqid, (void *) (&clientMsg), sizeof(clientMsg.msgText), recv_type, 0);  /*BLOCK*/
        if (ret == -1) {
            printf("msgrcv() error: %s\n", strerror(errno));
            break;
        }
        /*output*/
        printf("clientMsg: %s\n", clientMsg.msgText);

        MQTTClient client;
        char *username = "rabbitmq"; //添加的用户名
        char *password = "rabbitmq"; //添加的密码
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        MQTTClient_deliveryToken token;
        int rc;
        MQTTClient_create(&client, ADDRESS, CLIENTID,
                          MQTTCLIENT_PERSISTENCE_NONE, NULL);
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;
        conn_opts.username = "rabbitmq"; //将用户名写入连接选项中
        conn_opts.password = "rabbitmq"; //将密码写入连接选项中
        if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
            printf("Failed to connect, return code %d\n", rc);
            exit(EXIT_FAILURE);
        }
        pubmsg.payload =clientMsg.msgText;
        pubmsg.payloadlen = (int)strlen(clientMsg.msgText);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        printf("Waiting for up to %d seconds for publication of %s\n"
               "on topic %s for client with ClientID: %s\n",
               (int) (TIMEOUT / 1000), clientMsg.msgText, TOPIC, CLIENTID);
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        printf("Message with delivery token %d delivered\n", token);
        MQTTClient_disconnect(client, 10000);
        MQTTClient_destroy(&client);
        return rc;
        if(memcmp(clientMsg.msgText, "exit", 4) == 0)
        {
            printf("over and exit\n");
            break;
        }
        sleep(0.1);
    }

    /*msgctl IPC_RMID*/
    ret = msgctl(msqid, IPC_RMID, &ds_buf);
    if(ret !=0 )
    {
        printf("msgctl() error: %s\n", strerror(errno));
    }
    printf("msgctl() success. MQ is deleted\n");

    return 0;
}
