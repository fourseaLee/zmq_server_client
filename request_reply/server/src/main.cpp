#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

int main () 
{
	// 准备上下文和套接字
	zmq::context_t context (1);
	zmq::socket_t socket (context, ZMQ_REP);
	socket.bind ("tcp://*:5555");

	while (true) {
		zmq::message_t request;

		// 等待客户端请求
		socket.recv (&request);
		std::cout << "收到 Hello" << std::endl;
//		std::cout <<  "msg : " << request.data() << std::endl;

		std::string req = std::string(static_cast<char*>(request.data()), request.size());
        std::cout << "msg: "  << req << std::endl;
		// 做一些“处理”
		sleep (1);

		// 应答World
		zmq::message_t reply (5);
		memcpy ((void *) reply.data (), "World", 5);
		socket.send (reply);
	}
	return 0;
}

/*
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>



int main (void)
{
	void *context = zmq_init (1);

	//  与客户端通信的套接字
	void *responder = zmq_socket (context, ZMQ_REP);
	zmq_bind (responder, "tcp://*:5555");

	while (1) {
		//  等待客户端请求
		zmq_msg_t request;
		zmq_msg_init (&request);
		zmq_recv (responder, &request, 0);
		printf ("收到 Hello\n");
		zmq_msg_close (&request);

		//  做些“处理”
		sleep (1);

		//  返回应答
		zmq_msg_t reply;
		zmq_msg_init_size (&reply, 5);
		memcpy (zmq_msg_data (&reply), "World", 5);
		zmq_send (responder, &reply, 0);
		zmq_msg_close (&reply);
	}
	//  程序不会运行到这里，以下只是演示我们应该如何结束
	zmq_close (responder);
	zmq_term (context);
	return 0;
}

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

static char *
s_recv (void *socket) 
{
	zmq_msg_t message;
	zmq_msg_init (&message);
	zmq_recv (socket, &message, 5,0);
	int size = zmq_msg_size (&message);
	char *string = (char*)malloc((size +1)*sizeof(char));
	memcpy (string, zmq_msg_data (&message), size);
	zmq_msg_close (&message);
	string [size] = 0;
	return (string);
}

int maini1 () 
{
	// 准备上下文和套接字
	zmq::context_t context =  zmq_ctx_new();
	zmq::socket_t socket (context, ZMQ_REP);
	socket.bind ("tcp://*:5555");

	while (true) 
	{
		zmq::message_t request;

		// 等待客户端请求
		socket.recv (&request);
		std::cout << "收到 hello"  << std::endl;

		// 做一些“处理”
		sleep (1);

		char buf[256];
		bool ret = zmq_recv (&socket, buf, sizeof(buf) - 1, 0);
		buf[ret] = '\0';
		printf ("recv msg %s\n", buf);

		// 应答World
		zmq::message_t reply (5);
		memcpy ((void *) reply.data (), "World", 5);
		socket.send (reply);
	}
	return 0;
}


int main()
{
	void* context = zmq_ctx_new();
	//zmq::socket_t socket(context,ZMQ_REQ);
	void* socket = zmq_socket(context,ZMQ_REQ);

	zmq_bind(socket,"tcp://*:5555");
	
	int ret = 0;
	while(true)
	{
		char buf[256];
		ret = zmq_recv (socket, buf, sizeof(buf) - 1, 0);
		buf[ret] = '\0';
		printf ("recv msg %s\n", buf);

		sleep (1);

		//  返回应答
		memcpy (buf, "World", 5);
		zmq_send (socket, buf, 5,  0);

	}

	zmq_close (socket);
	zmq_ctx_destroy(context);
	return 0;

}
*/
