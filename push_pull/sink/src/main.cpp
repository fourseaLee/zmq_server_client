#include <unistd.h>
#include <string>
#include <iostream>
#include <zmq.hpp>

int main (void)
{
	// 准备上下文和套接字
	zmq::context_t context(1);
	zmq::socket_t receiver(context, ZMQ_PULL);
	receiver.bind( "tcp://*:5558");

	// 等待开始信号
	//    char *string = s_recv (receiver);
	//    free (string);

	while (1) {
		zmq_msg_t message; // 创建消息结构
		zmq_msg_init (&message); // 初始化空消息
		zmq_msg_recv (&message, &receiver,0); // 接收消息
		int size = zmq_msg_size (&message); // 计算消息的大小
		char *buf = new char[size + 1]; // 分配buf为指向size + 1大小的heap空间，那个多出来的1字节是'\0'的空间
		memcpy (buf, zmq_msg_data (&message), size); // 通过zmq_msg_data(1)获得消息的data地址，拷贝到字符串中
		zmq_msg_close (&message); // 释放或销毁消息
		buf [size] = 0; // 设置'\0'

		FILE * pFile;

		pFile = fopen("/home/ubtun/code/ZeroMQ_Demo _Sink/2.jpg", "wb" );
		fwrite (buf, 1, size, pFile);

		sleep (1);
	}

	receiver.close ();
	context.close ();
	return 0;
}
