#ifndef CIRCULAR_CONTAINER_H_
#define CIRCULAR_CONTAINER_H_
#if defined(WIN32)
#include <stdint.h>
#else
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#endif
namespace DataFilters {
	template <typename T, int Capacity>
	class CircularBuffer {
	public:
		CircularBuffer() {
			clear();
		}

		~CircularBuffer() {

		}

		bool empty() const {
			return size() == 0;
		}

		bool full() const {
			return capacity() == 0;
		}

		/*
		number of space available.
		*/
		size_t capacity() const {
			return Capacity - size();
		}

		void clear() {
			_tail = -1;
			_head = 0;
		}

		/*
		number of active elements.
		*/
		size_t size() const {
			if (_tail < 0)			return 0;//empty
			else if (_head > _tail)	return _head - _tail;
			else					return Capacity - _tail + _head;
		}

		/*
		make sure this is not empty.
		*/
		const T& front() const {
			return _buffer[_tail];
		}

		/*
		make sure this is not empty.
		*/
		T& front() {
			return _buffer[_tail];
		}

		/*
		make sure this is not empty.
		*/
		const T& back() const {
			int idx = _head - 1;
			if (idx < 0)	idx = Capacity - 1;
			return _buffer[idx];
		}

		/*
		make sure this is not empty.
		*/
		T& back() {
			int idx = _head - 1;
			if (idx < 0)	idx = Capacity - 1;
			return _buffer[idx];
		}

		void pop_front() {
			if (empty())//if empty, do nothing.
				return;
			if (++_tail == Capacity)
				_tail = 0;
			if (_tail == _head)
				clear();
		}

		void pop_back() {
			if (empty())//if empty, do nothing.
				return;
			if (--_head < 0)
				_head = Capacity - 1;
			if (_head == _tail) //now buffer is empty
				clear();
		}

		/*
		push back 'val' into buffer.
		if the buffer is full, do nothing.
		*/
		void push_back(const T& val) {
			if (full()) {
				_buffer[_head] = val;
				if (++_head == Capacity)
					_head = 0;
				_tail = _head;//_tail is overwritten by _head.
			}
			else {
				_buffer[_head] = val;
				if (empty())
					_tail = _head;
				if (++_head == Capacity)
					_head = 0;
			}
		}

		/*
		Accessing 'idx'-th element counting from the 'tail'.
		calling with 'idx' greater than size() is undefined.
		*/
		T& operator[](const size_t& idx) {
			size_t i = _tail + idx;
			if (i >= Capacity)
				i %= Capacity;
			return _buffer[i];
		}

		/*
		Accessing 'idx'-th element counting from the 'tail'.
		calling with 'idx' greater than size() is undefined.
		*/
		const T& operator[](const size_t& idx) const {
			size_t i = _tail + idx;
			if (i >= Capacity)
				i %= Capacity;
			return _buffer[i];
		}
	private:
		T _buffer[Capacity];
		int _head;
		int _tail;
	};
}
#endif