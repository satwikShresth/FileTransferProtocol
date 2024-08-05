//Mark Boady
//Drexel University 2021

//Go Style Channels with locks/condition variables
// "Do not communicate by sharing memory; instead, share memory by communicating."
//https://golang.org/doc/effective_go#concurrency
#pragma  once
#include <mutex>
#include <condition_variable>
#include <queue>

//Generic Channel
template <class X>
class channel{
	public:
		//Destructor
		virtual ~channel(){};
		//Send a Message
		virtual void send(X value)=0;
		//Recieve a Message
		virtual X receive()=0;
		//Close the Channel
		virtual void close()=0;
		//Check if closed
		virtual bool isClosed()=0;
};

/*
	Unbuffered channels sync both threads
	the send must complete before either
	thread moves forward
*/
template <class X>
class unbufferedChannel: public channel<X>{
	private:
		//Is this channel open or closed?
		bool open;
		//Communication Space
		bool senderReady;
		bool receiverReady;
		//Place Holder
		X tempVal;
		//Mutex and condition for communication
		mutable std::mutex cMut;
		std::condition_variable sender;
		std::condition_variable receiver;
	public:
		//Constructor
		unbufferedChannel();
		//Destructor
		~unbufferedChannel();
		//Send a Message
		void send(X value);
		//Recieve a Message
		X receive();
		//Close the Channel
		void close();
		//Check if closed
		bool isClosed();
};

/*
	Buffered Threads can store extra
	values in the buffer
 */
template <class X>
class bufferedChannel: public channel<X>{
	private:
		//Is the channel open
		bool open;
		//Buffer is a queue
		std::queue<X>* buffer;
		int maxSize;
		//Safety
		mutable std::mutex buffMut;
		std::condition_variable sender;
		std::condition_variable receiver;
	public:
		//Constructor
		bufferedChannel();
		//Alternative Constructor
		bufferedChannel(int size);
		//Destructor
		~bufferedChannel();
		//Send a Message
		void send(X value);
		//Recieve a Message
		X receive();
		//Close the Channel
		void close();
		//Check if closed
		bool isClosed();
};

/*--------------------------------------*/
/*       Make Channel Function          */
/*--------------------------------------*/
template <class X>
channel<X>* makeChannel(int size=0){
	if(size==0){
		return new unbufferedChannel<X>();
	}
	return new bufferedChannel<X>(size);
}

/*--------------------------------------*/
/* Implementation of Unbuffered Template*/
/*--------------------------------------*/
//The channel starts empty
template <class X>
unbufferedChannel<X>::unbufferedChannel(){
	//Channel is open
	open = true;
	//We have not done anything
	senderReady = false;
	receiverReady = false;
}

//Destructor has nothing interesting to do
template <class X>
unbufferedChannel<X>::~unbufferedChannel(){
}

//Send a message
template <class X>
void unbufferedChannel<X>::send(X value){
	std::unique_lock<std::mutex> lk(cMut);
	if(!open){
		throw std::runtime_error(
			"Send on Closed Channel.");
	}
	//Wait on a receiver
	sender.wait(lk,[this]{
		return receiverReady;});
	tempVal = value;
	senderReady=true;
	receiver.notify_all();
	//Wait until picked up
	sender.wait(lk,[this]{
		return !receiverReady;});
	//Done
	senderReady=false;
	receiver.notify_all();
}

//Receive a message
template <class X>
X unbufferedChannel<X>::receive(){
	std::unique_lock<std::mutex> lk(cMut);
	if(!open){
		throw std::runtime_error(
			"Receive on Closed Channel.");
	}
	//Wait on something to receive
	receiverReady=true;
	sender.notify_all();
	//Release lock so sender can act
	receiver.wait(lk,[this]{
		return senderReady || !open;});
	//Waiting on a channel that is closed
	if(!open){
		throw std::runtime_error(
			"Receive on Closed Channel.");
	}
	//Get value
	X myVal = tempVal;
	receiverReady=false;
	sender.notify_all();
	//Make sure they cleared out
	receiver.wait(lk,[this]{
		return !senderReady;});
	return myVal;
}


//Close the Channel
template <class X>
void unbufferedChannel<X>::close(){
	std::lock_guard<std::mutex> lk(cMut);
	open=false;
	receiver.notify_all();
}
//Check if closed
template <class X>
bool unbufferedChannel<X>::isClosed(){
	std::lock_guard<std::mutex> lk(cMut);
	return !open;
}

/*--------------------------------------*/
/* Implementation of Buffered Template  */
/*--------------------------------------*/
//Constructor
template <class X>
bufferedChannel<X>::bufferedChannel(){
	open=true;
	maxSize = 1;//Default
	buffer = new std::queue<X>();
}

//Alternative Constructor
template <class X>
bufferedChannel<X>::bufferedChannel(int size){
	open=true;
	maxSize = size;
	buffer = new std::queue<X>();
}

//Destructor
template <class X>
bufferedChannel<X>::~bufferedChannel(){
	delete buffer;
}

//Close the Channel
template <class X>
void bufferedChannel<X>::close(){
	std::lock_guard<std::mutex> lk(buffMut);
	open=false;
	receiver.notify_all();
}

//Check if closed
template <class X>
bool bufferedChannel<X>::isClosed(){
	std::lock_guard<std::mutex> lk(buffMut);
	//Not truly closed till empty
	return (!open && buffer->size()==0);
}

//Send a Message
template <class X>
void bufferedChannel<X>::send(X value){
	std::unique_lock<std::mutex> lk(buffMut);
	//We cannot send if closed
	if(!open){
		throw std::runtime_error(
			"Receive on Closed Channel.");
	}
	//Wait if the buffer is full
	sender.wait(lk,[this]{
		return buffer->size() < maxSize;});
	//Add to queue
	buffer->push(value);
	//Mission Accomplished!
	receiver.notify_one();
	return;
}

//Recieve a Message
template <class X>
X bufferedChannel<X>::receive(){
	std::unique_lock<std::mutex> lk(buffMut);
	//We cannot receive if closed
	if(!open && buffer->size()==0){
		throw std::runtime_error(
			"Receive on Closed Channel.");
	}
	//Wait for data
	receiver.wait(lk,[this]{
		return buffer->size()>0 || !open;});
	//We might be waiting when a close happens
	if(!open && buffer->size()==0){
		throw std::runtime_error(
			"Receive on Closed Channel.");
	}
	//Get data
	X data = buffer->front();
	buffer->pop();
	//Release
	sender.notify_one();
	//Return value
	return data;
}

