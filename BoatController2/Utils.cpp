// 
// 
// 

#include "Utils.h"


bool isNumeric(const String& val){
	unsigned int len = val.length();
	bool dot = false;
	bool sign = false;
	int num_cnt = 0;
	for (unsigned int i = 0; i < len; i++){
		if (val[i] > '9' || val[i] < '0'){
			switch (val[i]){
			case '-': case '+':
				if (sign || dot){
					return false;
				}
				sign = true;
				break;
			case '.':
				if (dot){
					return false;
				}
				dot = true;
				break;
			default:
				return false;
			}
		}
		else{
			num_cnt++;
		}
	}
	if (num_cnt == 0)
		return false;
	return true;
}

bool isInteger(const String& val){
	unsigned int len = val.length();
	bool sign = false;
	int num_cnt = 0;
	bool white_space = false;
	for (unsigned int i = 0; i < len; i++){
		if (val[i] > '9' || val[i] < '0'){
			switch (val[i]){
			case '-': case '+':
				if (num_cnt > 0){
					return false;
				}
				sign = true;
				break;
			case ' ':
				if (num_cnt){
					white_space = true;
				}
				break;
			default:
				//other character
				return false;
			}
		}
		else{
			if (white_space){
				return false;
			}
			num_cnt++;
		}
	}
	if (num_cnt == 0)
		return false;
	return true;
}