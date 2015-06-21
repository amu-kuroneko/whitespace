//
//  execute.c
//  whitespace
//
//  Created by kuroneko on 2015/01/18.
//  Copyright (c) 2015年 kuroneko. All rights reserved.
//

#include "whitespace.h"

/**
 * 現在参照している命令
 * プログラムカウンタ的役割を果たす
 */
static Instruction *current = NULL;

/**
 * ヒープ
 */
static long *heap = NULL;

/**
 * ヒープの確保容量
 */
static size_t heapAllocation = 0;

/**
 * スタック
 */
static long *stack = NULL;

/**
 * スタックの確保容量
 */
static size_t stackAllocation = 0;

/**
 * スタックの現在の参照位置
 */
static int stackPointer = 0;

/**
 * 命令を実行する際の基本的処理
 * @param instruction
 *	実行する命令
 * @return
 *	プログラムが終了の場合に true を返す
 */
static bool baseProcess( Instruction *instruction );

/**
 * スタック操作を行う
 * @param instruction
 *	実行する命令
 */
static void stackProcess( Instruction *instruction );

/**
 * 演算を行う
 * @param instruction
 *	実行する命令
 */
static void operationProcess( Instruction *instruction );

/**
 * ヒープアクセスを行う
 * @param instruction
 *	実行する命令
 */
static void heapProcess( Instruction *instruction );

/**
 * フロー制御を行う
 * @param instruction
 *	実行する命令
 * @return
 *	プログラムが終了する場合に true を返す
 */
static bool flowControlProcess( Instruction *instruction );

/**
 * 入出力処理を行う
 * @param instruction
 *	実行する命令
 */
static void ioProcess( Instruction *instruction );

/**
 * ヒープに値を設定する
 * @param address
 *	値を保存するヒープのアドレス
 * @param value
 *	ヒープに保存する値
 */
static void setHeapValue( int address , long value );

/**
 * ヒープから値を取得する
 * @param address
 *	値を取得するヒープのアドレス
 * @return
 *	取得した値
 */
static long getHeapValue( int address );

/**
 * スタックの値を積む
 * @param value
 *	スタックに積む値
 */
static void push( long value );

/**
 * スタックから値を取り出す
 * @return
 *	取り出した値
 */
static long pop( void );

/**
 * スタックの n 番目の値を取得する
 * @param position
 *	n 番目の n
 *	0 を指定すると 1個目の値が取得される
 * @return
 *	取得した値
 */
static long getStackValue( int position );

/**
 * スタックの1個目の値を取得する
 * @return
 *	取得した値
 */
static long getStackTop( void );

/**
 * サブルーチンの呼び出しを行う
 * @param instruction
 *	サブルーチンの命令
 * @return
 *	プログラム終了時に true を返す
 */
static bool callSubRoutine( Instruction *instruction );

/**
 * プログラムの実行時エラーを通知する
 * エラーが通知されるとプログラムは終了する
 */
static void error( char *message );



void execute( Instruction *instruction ){
	current = instruction;
	while( current != NULL ){
		if( baseProcess( current ) ){
			break;
		}
	}
	return;
}

void stackClear( void ){
	if( stack != NULL ){
		free( stack );
		stack = NULL;
		stackAllocation = 0;
		stackPointer = 0;
	}
	return;
}

void heapClear( void ){
	if( heap != NULL ){
		free( heap );
		heap = NULL;
		heapAllocation = 0;
	}
	return;
}

static bool baseProcess( Instruction *instruction ){
	switch( instruction->imp ){
		case STACK:
			stackProcess( instruction );
			break;

		case OPERATION:
			operationProcess( instruction );
			break;

		case HEAP:
			heapProcess( instruction );
			break;

		case FLOW_CONTROL:
			return flowControlProcess( instruction );

		case IO:
			ioProcess( instruction );
			break;

		default:
			error( "execute: illegal imp" );
			break;
	}
	return false;
}

static void stackProcess( Instruction *instruction ){
	long firstTemporary , secondTemporary;
	switch( instruction->c_stack ){
		case PUSH_NUMBER:
			push( instruction->p_value );
			break;

		case TOP_COPY:
			push( getStackTop() );
			break;

		case N_COPY:
			push( getStackValue( ( int ) instruction->p_value ) );
			break;

		case PUSH_EXCHANGE:
			firstTemporary = pop();
			secondTemporary = pop();
			push( firstTemporary );
			push( secondTemporary );
			break;

		case TOP_DESTRUCTION:
			pop();
			break;

		case N_SLIDE:
			firstTemporary = pop();
			stackPointer -= ( int ) instruction->p_value;
			push( firstTemporary );
			break;

		default:
			error( "execute: illegal stack command" );
			break;
	}
	current = instruction->next;
	return;
}

static void operationProcess( Instruction *instruction ){
	long right = pop();
	long left = pop();
	switch( instruction->c_operation ){
		case ADDTION:
			push( left + right );
			break;

		case SUBTRACTION:
			push( left - right );
			break;

		case MULTIPLICATION:
			push( left * right );
			break;

		case DIVISION:
			push( left / right );
			break;

		case MODULO:
			push( left % right );
			break;

		default:
			error( "execute: illegal operation command" );
			break;

	}
	current = instruction->next;
	return;
}

static void heapProcess( Instruction *instruction ){
	long address , value;
	switch( instruction->c_heap ){
		case TO_ADDRESS:
			value = pop();
			address = pop();
			setHeapValue( ( int ) address , value );
			break;

		case TO_STACK:
			address = pop();
			push( getHeapValue( ( int ) address ) );
			break;

		default:
			error( "execute: illegal heap command" );
			break;
	}
	current = instruction->next;
	return;
}

static bool flowControlProcess( Instruction *instruction ){
	switch( instruction->c_control ){
		case LABEL_DEFINE:
			current = instruction->next;
			break;

		case CALL_ROUTINE:
			if( callSubRoutine( instruction->jump ) ){
				return true;
			}
			current = instruction->next;
			break;

		case JUMP:
			current = instruction->jump;
			break;

		case ZERO_JUMP:
			current = pop() == 0 ? instruction->jump : instruction->next;
			break;

		case MINUS_JUMP:
			current = pop() < 0 ? instruction->jump : instruction->next;
			break;

		case END_ROUTINE:
			current = instruction->next;
			break;

		case FINISH:
			return true;

		default:
			current = instruction->next;
			break;
	}
	return false;
}

static void ioProcess( Instruction *instruction ){
	char buffer[BUFFER_SIZE];
	switch( instruction->c_io ){
		case PUT_CHAR:
			fputc( ( char ) ( pop() & 0xFF ) , stdout );
			fflush( stdout );
			break;

		case PUT_NUMBER:
			fprintf( stdout , "%ld" , pop() );
			fflush( stdout );
			break;

		case GET_CHAR:
			if( ! feof( stdin ) ){
				setHeapValue( ( int ) getStackTop() , fgetc( stdin ) );
			}
			break;

		case GET_NUMBER:
			if( ! feof( stdin ) ){
				setHeapValue( ( int ) getStackTop() , atoi( fgets( buffer , BUFFER_SIZE - 1 , stdin ) ) );
			}
			break;

		default:
			error( "execute: illegal io command" );
			break;
	}
	current = instruction->next;
	return;
}

static void setHeapValue( int address , long value ){
	if( heapAllocation <= address ){
		while( heapAllocation <= address ){
			heapAllocation += HEAP_ALLOCATION_SIZE;
		}
		if( heap == NULL ){
			if( ( heap = ( long * ) malloc( sizeof( long ) * heapAllocation ) ) == NULL ){
				error( "execute: out of memory error" );
			}
		}
		else if( ( heap = ( long * ) realloc( heap , sizeof( long ) * heapAllocation ) ) == NULL ){
			error( "execute: out of memory error" );
		}
	}
	heap[address] = value;
	return;
}
static long getHeapValue( int address ){
	if( heapAllocation <= address ){
		error( "execute: do not allocation in heap" );
	}
	return heap[address];
}

static void push( long value ){
	if( stack == NULL ){
		stackAllocation = STACK_ALLOCATION_SIZE;
		if( ( stack = ( long * ) malloc( sizeof( long ) * stackAllocation ) ) == NULL ){
			error( "execute: out of memory error" );
		}
	}
	else if( stackAllocation == stackPointer ){
		stackAllocation += STACK_ALLOCATION_SIZE;
		if( ( stack = ( long * ) realloc( stack , sizeof( long ) * stackAllocation ) ) == NULL ){
			error( "execute: out of memory error" );
		}
	}
	stack[stackPointer++] = value;
	return;
}

static long pop( void ){
	if( stackPointer == 0 ){
		error( "do not have value in stack" );
	}
	return stack[--stackPointer];
}

static long getStackValue( int position ){
	if( position < 0 && stackPointer <= position ){
		error( "do not have value in stack" );
	}
	return stack[stackPointer-position-1];
}

static long getStackTop( void ){
	return getStackValue( 0 );
}

static bool callSubRoutine( Instruction *instruction ){
	current = instruction;
	while( current != NULL ){
		if( current->imp == FLOW_CONTROL && current->c_control == END_ROUTINE ){
			break;
		}
		else if( baseProcess( current ) ){
			return true;
		}
	}
	return false;
}

static void error( char *message ){
	fputs( message , stderr );
	fputc( '\n' , stderr );
	fflush( stdout );
	exit( EXIT_FAILURE );
}

