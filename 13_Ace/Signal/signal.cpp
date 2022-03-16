#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <ace/Signal.h>

void handler(int)
{
  printf("SIGNINT found, exiting...");
  exit(0);
}
int main()
{
  ACE_Sig_Action sa((ACE_SignalHandler)handler, SIGINT);
  while (true)
    ;
  return 0;
}
