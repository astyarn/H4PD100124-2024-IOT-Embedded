/*
 * ProjectDefines.h
 *
 * Created: 23-11-2023 22:50:48
 *  Author: ltpe
 */ 


#ifndef PROJECTDEFINES_H_
#define PROJECTDEFINES_H_

#define SizeOfArray(arr)  (sizeof(arr)/sizeof(arr[0]))
	
#define DISPLAY_LINE_1 0

#define MAX_NUMBER_OF_LINES_IN_DISPLAY	8

typedef void (*Function_Pointer_With_One_Char_Parameter)(char);

#define DisplayBufferSize	64 

#endif /* PROJECTDEFINES_H_ */