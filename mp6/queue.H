#include "utils.H"
#include "thread.H"
struct Node
{
   Thread* thread;
   Node *next; 
   Node(Thread *t)
   {
      thread = t;
      next = NULL;
   }
};

class Blockqueue
{
   private:
    Node* head; 
    Node* tail;

   public:
   int qsize;
   Blockqueue()
   {
      head  = NULL;
      tail = NULL;
   }

   void push(Thread * thread)
   {
      Node* temp = new Node(thread);
      qsize++;
      if (tail == NULL) {
            head = tail = temp;
           
      }
      else
      {
        tail->next = temp;
        tail = temp; 
      }
   }

   Thread* pop()
   {
   	if (head == NULL)
            return NULL;
      
        qsize --;
        Node* temp = head;
        Thread *t  = temp->thread;
        head = head->next;
        if (head == NULL)
            tail = NULL;
        delete (void *)temp;
        return t;
   }

   
};


