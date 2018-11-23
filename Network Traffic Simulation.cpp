#include <string>
#include <iostream>
#include <stdlib.h>
#include <fstream>
using namespace std;

#define BUFFER_SIZE 100
#define INTERNETSPEED 11 //Mbps (Megabits per second) (bits per microsecond)
struct Packet{
	long arrivalTime;
	int size;
	long enterQueueTimestamp;
};

struct queue
{
	int rear, front;
	
	int size;
	Packet *arr;
	
	queue(int s) 
    { 
       front = rear = -1; 
       size = s; 
       arr = new Packet[s]; 
    } 
	
	void enQueue(Packet p);
	Packet deQueue();
	bool isEmpty();
	bool isFull();
};
//enqueue a Packet
void queue::enQueue(Packet p)
{
	if((front == 0 && rear == size -1) || (rear == (front-1)%(size-1)))
	{
		cout << "Queue is full";
		return;
	}
	else if(front == -1)
	{
		front = rear = 0;
		arr[rear] = p;
	}
	else if((rear == (size - 1)) && (front != 0))
	{
		rear = 0;
		arr[rear] = p;
	}
	else
	{
		rear++;
		arr[rear] = p;
	}
}
//dequeue a Packet
Packet queue::deQueue()
{
	struct Packet p;
	
	if(front == -1) 
    { 
        printf("\nQueue is Empty"); 
        return p; 
    } 
  
     
	p = arr[front]; 
    arr[front] = p; 
    if(front == rear) 
    { 
        front = -1; 
        rear = -1; 
    } 
    else if(front == size-1) 
        front = 0; 
    else
        front++; 
  
    return p; 

}
//tests if queue is empty
bool queue::isEmpty()
{
	if(front == -1)
		return true;
	else
		return false;
}
//tests if queue is full.
bool queue::isFull()
{
	if((front == 0 && rear == size -1) || (rear == (front-1)%(size-1)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void acceptpackets(int buffersize, int internetSpeed)
{
	//cout << "B = " << buffersize << " Speed = " << internetSpeed << endl;
	long microsecondTimer = 0; //simulation timer
	long secondaryTimer = 0; //timer used to see when a packet is done transmitting
	double filetime; //time the packet arives from file (in seconds)
	int size; //size of packet from file
	long convertedtime; //time convert to long as microseconds
	bool acceptflag = true; //flag that decides whether to accept no packet or not.
	bool transmitting = false; //flag that states whether we are currently transmitting a packet
	Packet transmittedPacket; //packet currently being transmitted
	queue buffer(buffersize); //routers buffer
	long packetsDropped = 0; //keeps track of dropped packets
	long packets = 0; //keeps track of total packets
	long packetsdelivered = 0; //keeps track of packets delivered
	long long queueDelaySum = 0; //sumation of all the time spent for each packet it queue
	
	ifstream packetfile("soccer.txt"); //file supplied by prof
	
	while(1)
	{
		if(acceptflag == true) //if we need to accept packet do so
		{
			if(!(packetfile >> filetime >> size))
			{
				//cout << "All packets finished" << endl;
				break;
			}
			convertedtime = filetime * 1000000;
			acceptflag = false;
			packets++;
		}
		if(microsecondTimer >= convertedtime) //if current accepted packet has arrived put into buffer and accept new one
		{
			struct Packet p;
			p.arrivalTime = convertedtime;
			p.size = size;
			p.enterQueueTimestamp = microsecondTimer;
			if(buffer.isFull()) //if buffer is full drop packet
			{
				packetsDropped++;
			}
			else //else put packet in buffer
			{
				buffer.enQueue(p);
			}
			acceptflag = true;
		}
		if(!buffer.isEmpty() && !transmitting) //if buffer isn't empty and we are not currently transmitting a packet
		{
			transmittedPacket = buffer.deQueue();
			queueDelaySum += (microsecondTimer - transmittedPacket.enterQueueTimestamp);
			transmitting = true;
		}
		if(transmitting) //if a packet is currently transmitting
		{
			if(secondaryTimer >= (transmittedPacket.size*8) / internetSpeed) //check to see if packet is finished
			{
				transmitting = false;
				secondaryTimer = 0;
				packetsdelivered++;
				queueDelaySum += (microsecondTimer - transmittedPacket.enterQueueTimestamp);
			}
			secondaryTimer++; //else continue transmitting
		}
		microsecondTimer++; //continue with main clock
	}
	double percentage = ((double)packetsDropped/packets)*100;
	
	cout << "        " <<  packets << "             " << packetsdelivered  << "          " <<  packetsDropped  << "            "
			<< percentage << "%             " << queueDelaySum / packetsdelivered << " microseconds" << endl;
	packetfile.close();
}

int main()
{ //tests different configurations and prints them out.
	cout.precision(2);
	cout << "Speed = 11 Mbps... changing B and testing" << endl;
	cout << "         (Incoming packets)  (Delivered Packets) (Packets Lost) (Packet Loss %)  (Average Queue Delay)" << endl;
	cout << "B = 10 ";
	acceptpackets(10, 11);
	cout << "B = 100 ";
	acceptpackets(100, 11);
	cout << "B = 1000 ";
	acceptpackets(1000, 11);
	
	cout << endl << "B = 100... changing speed and testing" << endl;
	cout << "                   (Incoming packets)(Delivered Packets)(Packets Lost)(Packet Loss %)(Average Queue Delay)" << endl;
	cout << "Speed = 6 Mbps ";
	acceptpackets(100, 6);
	cout << "Speed = 30 Mbps ";
	acceptpackets(100, 30);
	cout << "Speed = 54 Mbps ";
	acceptpackets(100, 54);
	return 1;
}