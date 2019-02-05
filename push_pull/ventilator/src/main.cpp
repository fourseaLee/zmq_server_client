#include <unistd.h>
#include <iostream>
#include <zmq.hpp>

int main (void)
{
	///任务分发器
	zmq::context_t context(1);

	// 用于发送消息的套接字
	zmq::socket_t sender (context, ZMQ_PUSH);
	sender.bind( "tcp://*:5557");

	// 用于发送开始信号的套接字
	zmq::socket_t sink(context, ZMQ_PUSH);
	sink.connect("tcp://localhost:5558");

	printf ("准备好worker后按任意键开始: ");
	getchar ();
	printf ("正在向worker分配任务...\n");

	//    // 发送开始信号
	//    s_send (sink, "0");

	int task_nbr;
	for (task_nbr = 0; task_nbr < 7; task_nbr++)
	{
		FILE* pFile;   //文件指针
		long lSize;   // 用于文件长度
		char* buffer; // 文件缓冲区指针
		size_t result;  // 返回值是读取的内容数量
		pFile = fopen("/home/ubtun/code/ZeroMQ_Demo_Client/3.jpg" , "rb");
		if (pFile == NULL)
			fputs("File error", stderr);
		fseek(pFile , 0 , SEEK_END); // 指针移到文件末位
		lSize = ftell(pFile);  // 获得文件长度
		rewind(pFile);
		buffer = new char[lSize]; // 分配缓冲区，按前面的 lSize
		if (buffer == NULL)
			fputs("Memory error", stderr);
		result = fread(buffer, 1, lSize, pFile); // 返回值是读取的内容数量
		if (result != lSize)
			fputs("Reading error", stderr);

		int rc;
		zmq_msg_t message; // 创建消息结构
		zmq_msg_init_size (&message, lSize); // 以字符串长度(不包括'\0')初始化成消息
		memcpy (zmq_msg_data (&message), buffer, lSize); // 将字符串的内容(不包括'\0')拷贝给消息
		rc = zmq_msg_send(&message,&sender,0); // 发送消息

		zmq_msg_close (&message); // 释放和销毁消息
	}

	sink.close();
	sender.close();
	context.close();
	return 0;
}

