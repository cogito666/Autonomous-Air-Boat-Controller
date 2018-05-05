// MyRingBuffer.h

#ifndef _MYRINGBUFFER_h
#define _MYRINGBUFFER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
template <typename DataType>
class RingArray {
public:
	enum {
		DEFAULT_BUFFER_SIZE = 100
	};
	RingArray(bool overwrite = false, size_t size = DEFAULT_BUFFER_SIZE)
		:overwrite(overwrite), sz_buffer(size){
		buffer = new DataType[sz_buffer];
		reset();
	}

	~RingArray(){
		delete[] buffer;
	}

	bool empty() const{
		return tail < 0;
	}

	bool full() const{
		return tail == head;
	}

	void reset(){
		tail = -1;
		head = 0;
	}

	size_t size() const{
		if (tail < 0)		return 0;
		if (head > tail)	return head - tail;
		else				return sz_buffer - tail + head;
	}

	bool writable() const{
		if (overwrite || size_w() > 0)
			return true;
		return false;
	}

	DataType& operator[](size_t idx){
		if (idx >= size()){
			abort();
		}
		size_t i = tail + idx;
		if (i >= sz_buffer)
			i %= sz_buffer;
		return buffer[i];
	}

	void pop(){
		if (empty())
			return;
		tail++;
		if (tail == sz_buffer)
			tail = 0;
	}

	void push(const DataType& c){
		if (writable()){
			buffer[head] = c;
			if (tail == head){
				tail++;
				if (tail == sz_buffer)
					tail = 0;
			}
			if (tail == -1)
				tail = head;
			head++;
			if (head == sz_buffer)
				head = 0;
		}
	}
private:
	/*
	number of bytes able to be written.
	*/
	size_t size_w() const{
		if (tail == head)	return 0;//full
		if (head < tail)	return tail - head;
		else				return sz_buffer - head + tail;
	}

	/*
	number of bytes to be read
	*/
	size_t size_r() const{
		if (tail < 0)		return 0;
		if (head > tail)	return head - tail;
		else				return sz_buffer - tail + head;
	}
private:
	DataType* buffer;
	int head; //write position
	int tail; //read position
	size_t sz_buffer;
	bool overwrite;
};
#endif

