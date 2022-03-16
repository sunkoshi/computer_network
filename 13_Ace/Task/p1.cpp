#include <ace/OS.h>
#include <ace/Task.h>
#include <ace/Message_Block.h>
// The Consumer Task.
class Consumer : public ACE_Task<ACE_MT_SYNCH>
{
public:
    int open(void *)
    {
        ACE_DEBUG((LM_DEBUG, "(%t) Producer task opened \n")); // Activate the Task
        activate(THR_NEW_LWP, 1);
        return 0;
    }
    // The Service Processing routine
    int svc(void)
    {
        // Get ready to receive message from Producer
        ACE_Message_Block *mb = 0;
        do
        {
            mb = 0;
            // Get message from underlying queue getq(mb);
            ACE_DEBUG((LM_DEBUG,
                       "(%t)Got message: %d from remote task\n", *mb->rd_ptr()));
        } while (*mb->rd_ptr() < 10);
        return 0;
    }
    int close(u_long)
    {
        ACE_DEBUG((LM_DEBUG, "Consumer closes down \n"));
        return 0;
    }
};
class Producer : public ACE_Task<ACE_MT_SYNCH>
{
public:
    Producer(Consumer *consumer) : consumer_(consumer), data_(0)
    {
        mb_ = new ACE_Message_Block((char *)&data_, sizeof(data_));
    }
    int open(void *)
    {
        ACE_DEBUG((LM_DEBUG, "(%t) Producer task opened \n"));
        // Activate the Task
        activate(THR_NEW_LWP, 1);
        return 0;
    }
    // The Service Processing routine
    int svc(void)
    {
        while (data_ < 11)
        {
            // Send message to consumer
            ACE_DEBUG((LM_DEBUG,
                       "(%t)Sending message: %d to remote task\n", data_));
            consumer_->putq(mb_);
            // Go to sleep for a sec.
            ACE_OS::sleep(1);
            data_++;
        }
        return 0;
    }
    int close(u_long)
    {
        ACE_DEBUG((LM_DEBUG, "Producer closes down \n"));
        return 0;
    }

private:
    char data_;
    Consumer *consumer_;
    ACE_Message_Block *mb_;
};
int main(int argc, char *argv[])
{
    Consumer *consumer = new Consumer;
    Producer *producer = new Producer(consumer);
    producer->open(0);
    consumer->open(0);
    // Wait for all the tasks to exit. ACE_Thread_Manager::instance()->wait();
}