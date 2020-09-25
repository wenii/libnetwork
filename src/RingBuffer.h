#ifndef __LIB_NETWORK_RING_BUFFER_H__
#define __LIB_NETWORK_RING_BUFFER_H__

namespace libnetwork
{
	/**
	 * tail指向最后一个有效字节的下一个位置
	 * head指向第一个有效字节
	 * head和tail的初始位置为buf首地址位置，此时缓冲区为空
	 * head的实际位置为:head & (size - 1)
	 * tail的实际位置为:tail & (size - 1)
	 * 缓冲区空的情况:tail - head == 0
	 * 缓冲区满的情况:tail - head == size
	 * 缓冲区的大小:tail - head
	 */
	class RingBuffer
	{
	public:
		
		// buffer大小
		static const unsigned int BUFFER_SIZE = 64 * 1024;
		static const unsigned int BUFFER_SIZE_1M = 1024 * 1024;	

	public:
		
		// 创建对象
		static RingBuffer* create(unsigned int size);

		
		// 销毁对象
		void destroy() { delete this; }

	public:
		
		// 缓存被使用大小
		int getUsedSize();

		
		// 可可入缓存大小
		int getWritableSize();

		
		// 写数据
		// 返回写数据多少
		int write(const char* buff, int size);
		
		// 获取缓存尾地址(用于写)
		char* getTail();
		
		// 获取尾部可写入的大小
		int getTailWritableSize();

		// 设置尾部写入偏移量
		void setWriteOffset(int offset);

		// 获取缓存头地址（用于读）
		const char* getHead();

		// 获取以head开始的可读缓存大小
		int getReadableSize();

		// 设置读偏移量
		void setReadOffset(int offset);

		// 数据向左对齐
		void alignToLeft(int offset);

		// 清理buffer
		void clear();

		// buffer是否空
		bool isEmpty();

		// buffer是否满
		bool isFull();

	private:
		
		// 构造函数
		RingBuffer();

		
		// 析构函数
		~RingBuffer();

	private:
		volatile unsigned int _head;				// buffer输入端
		volatile unsigned int _tail;				// buffer输出端
		volatile unsigned int _size;				// 缓冲区大小
		char* _buf;									// 缓冲区

	};
}	// namespace libnetwork


#endif // !__LIB_NETWORK_RING_BUFFER_H__
