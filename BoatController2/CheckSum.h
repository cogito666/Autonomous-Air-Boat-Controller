#ifndef CHECK_SUM_H_
#define CHECK_SUM_H_
template <typename ResultIntType>
class IfaceCheckSum{
public:

	virtual void push(const uint8_t& c) = 0;
	virtual void reset() = 0;

	/*
	return true, if expected(received) check-sum coincides with computed one.
	*/
	bool compare(const ResultIntType& expected) const{
		return result == expected;
	}

	const ResultIntType& getResult() const{
		return result;
	}
protected:
	ResultIntType result;
};

template <typename ResultIntType>
class CheckSumAdding :public IfaceCheckSum<ResultIntType>{
public:
	CheckSumAdding(){
		reset();
	}

	void push(const uint8_t& c){
		this->result += c;
	}

	void reset(){
		this->result = 0;
	}
};

template <typename ResultIntType>
class CheckSumXOR :public IfaceCheckSum<ResultIntType>{
public:
	CheckSumXOR(){
		reset();
	}

	void push(const uint8_t& c){
		this->result ^= c;
	}

	void reset(){
		this->result = 0;
	}
};

#endif