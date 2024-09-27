/*
 * MessageQueue.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: wangyifeng
 */

#include <pch.h>
#include "MessageQueue.h"
#include <stdio.h>
#include <string.h>

Message::Message()
{
	type_ = "null";
	len_ = 0;
}
Message::Message(void* pdata, int len)
{
	// TODO Auto-generated constructor stub
	type_ = "null";
	len_ = len;
	if (len_ > 0)
	{
		pdata_ = new char[len];
		memcpy(pdata_, pdata, len);
	}

}
Message::Message(string& str, string cmdtype)
{
	type_ = cmdtype;
	strData_ = str;
	// TODO Auto-generated constructor stub

}

Message::~Message()
{
	// TODO Auto-generated destructor stub
	if (len_ > 0)
	{
		delete[] pdata_;
	}
}
MessageQueue::MessageQueue()
{
	// TODO Auto-generated constructor stub
	//puts("new MessageQueue");
}

MessageQueue::~MessageQueue()
{
	// TODO Auto-generated destructor stub
	//puts("~MessageQueue");
}

void MessageQueue::push(void* pdata, int len)
{

	Message* item = new Message(pdata, len);
	push(item);

}
void MessageQueue::push(Message* transmit)
{

	lock_queue.lock();

	message_queue.push_back(transmit);

	lock_queue.unlock();

	sem_queue.release();

}

void MessageQueue::push(string& strcmd, string cmdtype)
{
	Message* tmp = new Message(strcmd, cmdtype);
	lock_queue.lock();

	//message_queue.push(tmp); // for std::queue

	message_queue.push_back(tmp);

	lock_queue.unlock();

	sem_queue.release();

}

int MessageQueue::prepop(string cmd) {
	int idx = -1;
	lock_queue.lock();

	for (int i = 0; i < message_queue.size(); i++)
	{
		if (message_queue[i]->type_ == cmd)
		{
			idx = i;

			Message* transmit = message_queue[i];
			//message_queue.pop(); // for std::queue
			message_queue.erase(message_queue.begin()+i);
			delete transmit;
			break;
		}
	}

	lock_queue.unlock();

	return idx;
}


int MessageQueue::peek(string cmd)
{
	int idx = -1;
	lock_queue.lock();

	for (int i = 0; i < message_queue.size(); i++)
	{
		if (message_queue[i]->type_ == cmd)
		{
			idx = i;
			break;
		}
	}

	lock_queue.unlock();

	return idx;
}


Message* MessageQueue::pop()
{
	sem_queue.wait();

	lock_queue.lock();

	Message* transmit = message_queue.front();
	//message_queue.pop(); // for std::queue
	message_queue.erase(message_queue.begin());
	//printf("-------data queue size = %d\n", message_queue.size());

	lock_queue.unlock();

	return transmit;
}
Message* MessageQueue::trypop(int mseconds)
{
	sem_queue.wait_sec(mseconds);

	lock_queue.lock();

	Message* transmit = NULL;
	if (!message_queue.empty())
	{
		transmit = message_queue.front();
		//message_queue.pop();
		message_queue.erase(message_queue.begin());

	}

	lock_queue.unlock();

	return transmit;
}

int MessageQueue::size()
{
	return message_queue.size();
}
void MessageQueue::clear()
{
	lock_queue.lock();
	while (message_queue.size()>2)
	{
		sem_queue.wait();
		Message*  transmit = message_queue.front();
		//message_queue.pop();
		message_queue.erase(message_queue.begin());

		if(transmit!=NULL)
		delete transmit;
	}
	lock_queue.unlock();
}