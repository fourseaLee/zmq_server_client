#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
/*
int main()
{
	void *context = zmq_init (1);

	//  连接至服务端的套接字
	std::cout << "正在连接至hello world服务端...\n" << std::endl;
	void *requester = zmq_socket (context, ZMQ_REQ);
	zmq_connect (requester, "tcp://1270.0.0.1:5555");

	int request_nbr;
	for (request_nbr = 0; request_nbr != 10; request_nbr++) 
	{
		zmq_msg_t request;
		zmq_msg_init_size (&request, 5);
		memcpy (zmq_msg_data (&request), "Hello", 5);
		std::cout << "正在发送 Hello " <<request_nbr << "...\n";
		zmq_send (requester, &request, 0,ZMQ_DONTWAIT);


		zmq_msg_close (&request);

		zmq_msg_t reply;
		zmq_msg_init (&reply);
		zmq_recv (requester, &reply, 0, ZMQ_DONTWAIT);
		
		std::cout << "接收到 World " << request_nbr << "\n";
		int size = zmq_msg_size (&reply);
		char *string = (char*)malloc((size +1)*sizeof(char));
		memcpy (string, zmq_msg_data (&reply), size);
		std::cout << "message: ";
		for (int i = 0; i <  size; i++)
			std::cout << string[i];
		zmq_msg_close (&reply);
		string [size] = 0;
		//std::cout  <<  "message: " << string << std::endl;

	}
	zmq_close (requester);
	zmq_term (context);
	return 0;
}*/

int main()
{
	void* context = zmq_ctx_new();
	void* requster = zmq_socket(context,ZMQ_REQ);
	zmq_connect (requster, "tcp://127.0.0.1:5555");
	int ret;
	while(true)
	{
		zmq_send (requster, "hello", 5, 0);
		sleep (1);
		char buf[256];
		ret = zmq_recv (requster, buf, sizeof(buf) - 1, 0);
		buf[ret] = '\0';
		printf ("recv msg %s\n", buf);
	}

	zmq_close (requster);
	zmq_ctx_destroy(context);
	return 0;
}
