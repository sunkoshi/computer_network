#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/OS.h>
#include <ace/SOCK_Dgram.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <ace/Task.h>
#include <ace/Future.h>
#include <ace/Auto_Ptr.h>
#include <ace/config-lite.h>
#include <ace/Message_Queue.h>
#include <ace/ipc
#include <bits/stdc++.h>
using namespace std;
class MsgQ
{
private:
    ACE_Message_Queue<ACE_NULL_SYNCH> *mq_;

public:
    MsgQ()
    {
        if (!(this->mq_ = new ACE_Message_Queue<ACE_NULL_SYNCH>()))
            printf("Error in message queue initialization \n");
        this->mq_->
    }
    int putMsg(char *ch)
    {
        ACE_Message_Block *mb = new ACE_Message_Block(ch, strlen(ch) + 1);
        mb->msg_type(2);
        if (this->mq_->enqueue_prio(mb) == -1)
        {
            ACE_DEBUG((LM_ERROR, "\nCould not enqueue on to mq!!\n"));
            return -1;
        }
        printf("Message put to MsgQ...\n");
        return 0;
    }
    int getMsg()
    {
        if (mq_->is_empty())
        {
            printf("Msq queue is empty...\n");
            return 0;
        }
        ACE_Message_Block *mb;
        mq_->peek_dequeue_head(mb);
        int type = mb->msg_type();
        if (type != 2)
        {
            cout << "Message of diff types..." << endl;
        }
        else
        {
            printf("From MsgQ: %s", mb->rd_ptr());
        }

        mb->release();
        return 0;
    }
};

int main(int argc, char *argv[])
{

    MsgQ test;
    cout << "Enter message to be put into the Queue: ";
    char *ch;
    cin >> ch;
    test.putMsg(ch);
    test.getMsg();
    while (true)
        ;
}