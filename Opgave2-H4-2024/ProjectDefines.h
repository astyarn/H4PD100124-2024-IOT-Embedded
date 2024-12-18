/*
 * ProjectDefines.h
 *
 * Created: 23-11-2023 22:50:48
 *  Author: ltpe
 */ 


#ifndef PROJECTDEFINES_H_
#define PROJECTDEFINES_H_

typedef enum
{
	IDLE_STATE_ENUM,
	RECEIVING_INSTRUCTION_ENUM	
} StateMachineStates;

#define SizeOfArray(arr)  (sizeof(arr)/sizeof(arr[0]))
	
#define ExternalInt3LedBitPosition	0
#define ExternalInt4LedBitPosition	1
#define ExternalInt5LedBitPosition	2
#define ExternalInt6LedBitPosition	3

#define ExternalInt0LedBitPosition	7
#define ExternalInt1LedBitPosition	7
#define ExternalInt2LedBitPosition	7
#define ExternalInt7LedBitPosition	7

#endif /* PROJECTDEFINES_H_ */