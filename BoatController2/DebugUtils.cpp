// 
// 
// 

#include "DebugUtils.h"

//// handle diagnostic informations given by assertion and abort program execution:
void __assert_func(const char *__file, int __lineno, const char *__func, const char *__sexp) {
	// transmit diagnostic informations through serial link. 
	char assert_msg[200];
	sprintf(assert_msg, "Assert(%s) failed in func(%s) at file(%s: %d)",
		__sexp, __func, __file, __lineno);
	__DEBUG_UTILS_SERIAL.println(assert_msg);
	__DEBUG_UTILS_SERIAL.flush();
	abort();
}



