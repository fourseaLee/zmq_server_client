#include <zmq.hpp>
#include <unistd.h>
#include <iostream>

int main (void)
{
	zmq::context_t context (1);

	// 获取任务的套接字
	zmq::socket_t receiver(context, ZMQ_PULL);
	receiver.connect("tcp://localhost:5557");

	// 发送结果的套接字
	zmq::socket_t sender(context, ZMQ_PUSH);
	sender.connect("tcp://localhost:5558");

	// 循环处理任务
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

		pFile = fopen("/home/ubtun/code/ZeroMQ_Demo_Worker2/2.jpg", "wb" );
		fwrite (buf, 1, size, pFile);

		sleep (1);

		//PULL
		int rc;
		zmq_msg_t sendmessage; // 创建消息结构
		zmq_msg_init_size (&sendmessage, size); // 以字符串长度(不包括'\0')初始化成消息
		memcpy (zmq_msg_data (&sendmessage), buf, size); // 将字符串的内容(不包括'\0')拷贝给消息
		rc = zmq_msg_send(&sendmessage,&sender,0); // 发送消息

		zmq_msg_close (&sendmessage); // 释放和销毁消息
	}
	receiver.close ();
	sender.close ();
	zmq_term (&context);
	return 0;
}
