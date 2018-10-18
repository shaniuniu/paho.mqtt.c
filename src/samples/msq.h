//
// Created by syy on 18-10-9.
//

#ifndef ECLIPSE_PAHO_C_MSQ_H
#define ECLIPSE_PAHO_C_MSQ_H
# define MAX_MSG_LEN 1024
# define MSQ_KEY 21012
typedef struct MSGQUEUE
{
    long msgType;
    char msgText[MAX_MSG_LEN];

}Msg;
#endif //ECLIPSE_PAHO_C_MSQ_H
