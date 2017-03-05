#include<iostream>
#include<semaphore.h>
#include<pthread.h>
#include<queue>
#include<unistd.h>
using namespace std;

#define maxbarbers 3
#define maxcustomers 50
#define maxcapacity 10
#define maxfilmfares 4
// os books are unlimited

sem_t capacity; 					// for capacity of customers
sem_t customers; 					// for customer wait
sem_t barbers;  					// for barber wait
sem_t filmfare; 					// for filmfare
sem_t customer[maxcustomers]; 				// for all customers
sem_t barber[maxbarbers]; 				// for all barbers
sem_t paymoney; 					// for customer to pay money to barber
sem_t submit_money; 					// for barber to submit money to cash counter
sem_t haircut;
sem_t acknowledge;

pthread_mutex_t customer_queue_mutex;
pthread_mutex_t barber_queue_mutex;
pthread_mutex_t print_mutex;

queue<int> customer_queue;  				// Customer Queue who are reading filmfare and waiting for HairCut
queue<int> barber_queue;    				// Barber Queue for Submitting cash to cash counter


// Initialize Semaphores and mutex
void initialize()
{
    pthread_mutex_init(&print_mutex,NULL);
    pthread_mutex_lock(&print_mutex);
    cout<<"Initializing variables ... \n";
    pthread_mutex_unlock(&print_mutex);

    sem_init(&capacity, 0, maxcapacity);
    sem_init(&filmfare, 0, maxfilmfares);

    for(int i=0; i<maxcustomers; i++)
        sem_init(&customer[i], 0, 0);

    sem_init(&paymoney, 0, 0);
    sem_init(&customers, 0, 0);

    for(int i=0; i<maxbarbers; i++)
        sem_init(&barber[i], 0, 0);

    sem_init(&barbers, 0, 0);
    sem_init(&haircut, 0, 0);
    sem_init(&submit_money, 0, 0);

    pthread_mutex_init(&customer_queue_mutex, NULL);
    pthread_mutex_init(&barber_queue_mutex, NULL);

}

void read_filmfare(int customer_no){

    pthread_mutex_lock(&print_mutex);
    cout<<"Customer "<<customer_no+1<<" is reading Filmfare.\n";
    pthread_mutex_unlock(&print_mutex);
}

void read_os(int customer_no){

    pthread_mutex_lock(&print_mutex);
    cout<<"Customer "<<customer_no+1<<" is reading OS Book.\n";
    pthread_mutex_unlock(&print_mutex);
}


void sleeping(int barber_no)
{
    pthread_mutex_lock(&print_mutex);
    cout<<"Barber "<<barber_no+1<<" is sleeping.\n";
    pthread_mutex_unlock(&print_mutex);
}

void getting_haircut(int customer_no){

    pthread_mutex_lock(&print_mutex);
    cout<<"Customer "<<customer_no+1<<" is getting HairCut.\n";
    pthread_mutex_unlock(&print_mutex);
}

void leave_shop(int customer_no){

    pthread_mutex_lock(&print_mutex);
    cout<<"Customer "<<customer_no+1<<" is leaving the shop.\n";
    pthread_mutex_unlock(&print_mutex);
}

void pay_money(int customer_no){

    pthread_mutex_lock(&print_mutex);
    cout<<"Customer "<<customer_no+1<<" is paying for Haircut.\n";
    pthread_mutex_unlock(&print_mutex);
}

void money_submit(int barber_no){

    pthread_mutex_lock(&print_mutex);
    cout<<"Barber "<<barber_no+1<<" submitted money to Cash Counter.\n";
    pthread_mutex_unlock(&print_mutex);
}

//********Runner function for customer threads***********//
void *Customer(void *CId) {

    	long CustomerId=(long) CId;
    	pthread_mutex_lock(&print_mutex);
    	cout<<"\nCreating Customer "<<CustomerId+1<<"\n";

    	pthread_mutex_unlock(&print_mutex);
    	//wait if the capacity is full
    	sem_wait(&capacity);


    	//read OS Book 
    	read_os(CustomerId);
    	// wait for filmfare
	sem_wait(&filmfare);
	read_filmfare(CustomerId);
	pthread_mutex_lock(&customer_queue_mutex);
   	customer_queue.push(CustomerId);
   	sem_post(&customers);
   	pthread_mutex_unlock(&customer_queue_mutex);
	sem_wait(&customer[CustomerId]);
	
	sem_wait(&haircut);
	sem_post(&filmfare);
	sem_post(&paymoney);
	pay_money(CustomerId);
	sem_post(&capacity);
	// Customer leaves Shop after paying money
	sem_wait(&acknowledge);
	leave_shop(CustomerId);
}

//*********Runner function for Barber Threads***********//
void *Barber(void* BId){

    long BarberId=(long)BId;
    int front_customer;

    while(true){
                // Barber is sleeping
                sleeping(BarberId);

                // wait for customers to come
                sem_wait(&customers);

                // get the customer from queue
                pthread_mutex_lock(&customer_queue_mutex);
                front_customer=customer_queue.front();
                customer_queue.pop();
                pthread_mutex_unlock(&customer_queue_mutex);

                // call the customer who is reading filmfare for longest time
                sem_post(&customer[front_customer]);
                
                // Customer getting Haircut
                pthread_mutex_lock(&print_mutex);
                cout<<"Barber "<<BarberId+1<<" is cutting hair of customer "<<front_customer+1<<"\n";
                pthread_mutex_unlock(&print_mutex);
                sleep(5);
		sem_post(&haircut);


                //Waiting for customer to pay money
                sem_wait(&paymoney);
                pthread_mutex_lock(&print_mutex);
                cout<<"Barber "<<BarberId+1<<" received money from customer "<<front_customer+1<<"\n";
                pthread_mutex_unlock(&print_mutex);
		sem_post(&acknowledge);

                //Add Barber to queue
                pthread_mutex_lock(&barber_queue_mutex);
                barber_queue.push(BarberId);
                sem_post(&barbers);
                pthread_mutex_unlock(&barber_queue_mutex);

                sem_wait(&barber[BarberId]);
                sem_post(&submit_money);
                money_submit(BarberId);

        }
}

//************runner function for cash counter**************//
void *CashCounter(void *args){

    int front_barber;

    while(true){

            // Wait for barbers to come
            sem_wait(&barbers);
            // Take Barber from Queue
            pthread_mutex_lock(&barber_queue_mutex);
            front_barber=barber_queue.front();
            barber_queue.pop();
            pthread_mutex_unlock(&barber_queue_mutex);

            sem_post(&barber[front_barber]);

            // wait for barber to submit money
            sem_wait(&submit_money);

    }
}


int main(){

    
    initialize();              				//Initializing the semaphore and mutexes
    
    pthread_t barber_threads[maxbarbers]; 		//Barber threads declaration
    pthread_t cust_threads[maxcustomers];		// Customer threads declaration
    pthread_t cash_thread;				//cash thread declaration



    long i=0;

    pthread_mutex_lock(&print_mutex);
    cout<< "\nCreating barber threads\n";
    pthread_mutex_unlock(&print_mutex);

	while (i < maxbarbers){

             pthread_create(&barber_threads[i],NULL,Barber,(void *)i);
             i++;
 	}

 	pthread_create(&cash_thread,NULL,CashCounter,NULL);

    	pthread_mutex_lock(&print_mutex);
   	cout<< "\nCreating customer threads\n";
    	pthread_mutex_unlock(&print_mutex);

    	i=0;
	char ch;
	cin>>ch;

	while(ch=='c' && i<maxcustomers) {
            
            pthread_create(&cust_threads[i],NULL,Customer,(void *)i);
            i++;
	    cin>>ch;

    }

}
