//Linux下可编译通过
#include <cstdio>
#include <cassert>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <zmq.h>
#include <string.h>
#include<iostream>

using namespace std;

#define printError()    printf("%s[%d]: %d: %s\n", __FILE__, __LINE__, \
		zmq_errno(), zmq_strerror(zmq_errno()));


static void create_broker(void* context, const char* front_addr, const char* back_addr);
static void create_worker(void* context, const char* addr);
static void create_client(void* context, const char* addr);


int main(int argc, char* argv[])
{
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	int version = ZMQ_MAKE_VERSION(major, minor, patch);
	if (version < ZMQ_VERSION)
	{
		printf("zmq library expect version %d but get %d.\n", ZMQ_VERSION, version);
		return 1;
	}

	void *context = zmq_ctx_new();
	const char* front_addr = "tcp://127.0.0.1:5559";
	const char* back_addr = "tcp://127.0.0.1:5560";

	// create broker
	auto broker = make_shared<thread>(create_broker, context, front_addr, back_addr);

	// create worker
	vector<shared_ptr<thread>> workers;
	for (int i = 0; i < 1; i++)
	{
		auto worker = make_shared<thread>(create_worker, context, back_addr);
		workers.emplace_back(worker);
	}

	// create clients
	vector<shared_ptr<thread>> clients;
	for (int i = 0; i < 1; i++)
	{
		auto client = make_shared<thread>(create_client, context, front_addr);
		clients.emplace_back(client);
	}

	if (broker)
	{
		broker->join();
	}
	for (auto worker : workers)
	{
		worker->join();
	}
	for (auto client : clients)
	{
		client->join();
	}

	zmq_ctx_destroy(context);

	return 0;
}


void create_worker(void* context, const char* addr)
{
	assert(context && addr);
	void* responder = zmq_socket(context, ZMQ_REP);
	int r = zmq_connect(responder, addr);
	if (r == -1)
	{
		printError();
		zmq_close(responder);
		return;
	}

	//auto thread_id = this_thread::get_id().hash();
	auto thread_id =  pthread_self();
	printf("worker %u started...\n", thread_id);
	while (true)
	{
		zmq_msg_t msgIn;
		zmq_msg_init(&msgIn);
		r = zmq_msg_recv(&msgIn, responder, 0);
		if (r == -1)
		{
			printError();
			break;
		}
		printf("[%u]Received request: %s\n", thread_id, (char*)zmq_msg_data(&msgIn));
		zmq_msg_close(&msgIn);

		//发送消息
		zmq_msg_t msgOut;
		zmq_msg_init_size (&msgOut, 6);
		memcpy(zmq_msg_data (&msgOut), "WORLD", 6);
		r = zmq_msg_send(&msgOut,responder,0);
		if (r == -1)
		{
			printError();
			break;
		}
		//cout << "send:" << (char*)zmq_msg_data(&msgOut) << endl;
		zmq_msg_close (&msgOut); // 释放和销毁消息
		this_thread::sleep_for(chrono::milliseconds(1));
	}
	zmq_close(responder);
	printf("worker %u closed...\n", thread_id);
}


void create_broker(void* context, const char* front_addr, const char* back_addr)
{
	assert(context && front_addr && back_addr);
	void* frontend = zmq_socket(context, ZMQ_ROUTER);
	void* backend = zmq_socket(context, ZMQ_DEALER);
	assert(frontend && backend);
	int r = zmq_bind(frontend, front_addr);
	assert(r == 0);
	r = zmq_bind(backend, back_addr);
	assert(r == 0);

	printf("broker started...\n");
	zmq_proxy(frontend, backend, NULL);

#if 1
	zmq_pollitem_t items[] =
	{
		{ frontend, 0, ZMQ_POLLIN, 0 },
		{ backend, 0, ZMQ_POLLIN, 0 },
	};
	while (true)
	{
		zmq_msg_t message;
		r = zmq_poll(items, 2, -1);
		if (r == -1)
		{
			printError();
			break;
		}
		if (items[0].revents & ZMQ_POLLIN)
		{
			while (true)
			{
				zmq_msg_init(&message);
				r = zmq_msg_recv(&message, frontend, 0);
				if (r == -1)
				{
					printError();
					break;
				}
				int more = zmq_msg_more(&message);
				r = zmq_msg_send(&message, backend, more ? ZMQ_SNDMORE : 0);
				if (r == -1)
				{
					printError();
					zmq_msg_close(&message);
					break;
				}
				zmq_msg_close(&message);
				if (!more)
					break;
			}
		}
		if (items[1].revents & ZMQ_POLLIN)
		{
			while (true)
			{
				zmq_msg_init(&message);
				r = zmq_msg_recv(&message, backend, 0);
				if (r == -1)
				{
					printError();
					zmq_msg_close(&message);
					break;
				}
				int more = zmq_msg_more(&message);
				r = zmq_msg_send(&message, frontend, more ? ZMQ_SNDMORE : 0);
				if (r == -1)
				{
					printError();
					zmq_msg_close(&message);
					break;
				}
				zmq_msg_close(&message);
				if (!more)
					break;
			}
		}
	}
#endif

	zmq_close(frontend);
	zmq_close(backend);
	printf("broker[%d] closed...\n", pthread_self());
}


void create_client(void* context, const char* addr)
{
	assert(context && addr);
	void* requester = zmq_socket(context, ZMQ_REQ);
	assert(requester);
	int r = zmq_connect(requester, addr);
	if (r == -1)
	{
		printError();
		zmq_close(requester);
		return;
	}
	auto thread_id =  pthread_self();
	printf("client %u started...\n", thread_id);
	int request_nbr = 10;
	for (int i = 0; i < request_nbr; i++)
	{
		zmq_msg_t msgOut;
		zmq_msg_init_size (&msgOut, 6);
		memcpy (zmq_msg_data (&msgOut), "hello", 6);
		r = zmq_msg_send(&msgOut,requester,0);
		if (r == -1)
		{
			printError();
			break;
		}
		//cout << "send:" << (char*)zmq_msg_data(&msgOut) << endl;
		zmq_msg_close (&msgOut);

		zmq_msg_t msgIn;
		zmq_msg_init(&msgIn);
		r = zmq_msg_recv(&msgIn, requester, 0);
		if (r == -1)
		{
			printError();
			break;
		}
		printf("[%u]Received reply %d [%s]\n", thread_id, i, (char*)zmq_msg_data(&msgIn));
		// cout << "recv:" << (char*)zmq_msg_data(&msgIn) << endl;
		zmq_msg_close(&msgIn);

		this_thread::sleep_for(chrono::milliseconds(10));
	}
	zmq_close(requester);
	printf("client %u closed...\n", thread_id);
}



