//
//  prepare.c
//  whitespace
//
//  Created by kuroneko on 2015/01/18.
//  Copyright (c) 2015年 kuroneko. All rights reserved.
//

#include "whitespace.h"
#include "hash.h"

/**
 * 読み込まれたプログラムのコメントを除いたもの
 */
static char *program = NULL;

/**
 * プログラムの長さ
 */
static int length = 0;

/**
 * プログラムの確保サイズ
 */
static size_t allocation = 0;

/**
 * ラベルのハッシュマップ
 */
static HashMap *labelMap = NULL;

/**
 * 命令の構造体に命令・コマンド・パラメータを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setInstruction( char *position , Instruction *instruction );

/**
 * 命令の構造体に命令変更パラメータを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setIMP( char *position , Instruction *instruction );

/**
 * 命令の構造体にコマンドを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setCommand( char *position , Instruction *instruction );

/**
 * 命令の構造体にスタック操作コマンドを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setStackCommand( char *position , Instruction *instruction );

/**
 * 命令の構造体に演算コマンドを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setOperationCommand( char *position , Instruction *instruction );

/**
 * 命令の構造体にヒープアクセスコマンドを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setHeapCommand( char *position , Instruction *instruction );

/**
 * 命令の構造体にフロー制御コマンドを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setControlCommand( char *position , Instruction *instruction );

/**
 * 命令の構造体に入出力コマンドを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setIOCommand( char *position , Instruction *instruction );

/**
 * 命令の構造体にパラメータを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setParameter( char *position , Instruction *instruction );

/**
 * 命令の構造体に数値パラメータを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setNumber( char *position , Instruction *instruction );

/**
 * 命令の構造体にラベルを設定する
 * @param position
 *	現在のプログラムの参照位置
 * @param instruction
 *	設定する命令構造体
 * @return
 *	処理終了時のプログラムの参照位置
 */
static char *setLabel( char *position , Instruction *instruction );

/**
 * ラベルと命令をマッピングする
 * @param label
 *	マッピングする際にキーとなるラベル
 * @param instruction
 *	マッピングする命令
 */
static void addLabel( char *label , Instruction *instruction );

/**
 * ラベルを参照している命令との関係を設定する
 * @param instruction
 *	命令セット
 */
static void setRelation( Instruction *instruction );

/**
 * エラーメッセージを表示する
 * @param message
 *	表示するメッセージ
 */
static void error( char *message );



void programClear( void ){
	if( program != NULL ){
		free( program );
	}
	allocation = 0;
	length = 0;
	return;
}

void setProgram( char *source , size_t size ){
	if( program == NULL ){
		allocation = size;
		if( ( program = ( char * ) malloc( sizeof( char ) * allocation ) ) == NULL ){
			error( "out of memory error" );
			exit( EXIT_FAILURE );
		}
	}
	else if( allocation < length + size ){
		allocation += size;
		if( ( program = ( char * ) realloc( program , sizeof( char ) * allocation ) ) == NULL ){
			error( "out of memory error" );
			exit( EXIT_FAILURE );
		}
	}
	char current;
	while( ( current = *source++ ) != '\0' ){
		if( current == '\t' || current == ' ' || current == '\n' ){
			program[length++] = current;
		}
	}
	program[length] = '\0';
	return;
}

char *getOptimizationProgram( size_t *size ){
	char *copyProgram;
	if( ( copyProgram= ( char * ) malloc( sizeof( char ) * allocation ) ) == NULL ){
		error( "out of memory error" );
		exit( EXIT_FAILURE );
	}
	*size = length;
	memcpy( copyProgram , program , allocation );
	return copyProgram;
}

Instruction *getInstruction( void ){
	if( program == NULL ){
		error( "do not have program" );
		exit( EXIT_FAILURE );
	}
	char *position = program;
	Instruction *instruction = NULL , *previous = NULL , *start = NULL;
	while( *position != '\0' ){
		instruction = ( Instruction * ) malloc( sizeof( Instruction ) );
		if( start == NULL ){
			start = instruction;
		}
		position = setInstruction( position , instruction );
		if( position == NULL ){
			free( instruction );
			break;
		}
		if( previous != NULL ){
			previous->next = instruction;
		}
		previous = instruction;
	}
	setRelation( start );
	return start;
}

void freeInstruction( Instruction *instruction ){
	if( instruction == NULL ){
		error( "do not have instruction" );
		exit( EXIT_FAILURE );
	}
	Instruction *next = instruction;
	do{
		instruction = next;
		next = instruction->next;
		instruction->next = NULL;
		instruction->jump = NULL;
		if( instruction->imp == FLOW_CONTROL && instruction->p_label != NULL ){
			free( instruction->p_label );
			instruction->p_label = NULL;
		}
		free( instruction );
	} while( next != NULL );
	if( labelMap != NULL ){
		finishHashMap( labelMap );
		labelMap = NULL;
	}
	return;
}

Instruction *getInstructionAtLabel( char *label ){
	Instruction *instruction = getHashValue( labelMap , label );
	if( instruction == NULL ){
		error( "do not have instruction at label" );
		exit( EXIT_FAILURE );
	}
	return instruction;
}

static char *setInstruction( char *position , Instruction *instruction ){
	memset( instruction , 0 , sizeof( Instruction ) );
	if( ( position = setIMP( position , instruction ) ) != NULL ){
		if( ( position = setCommand( position , instruction ) ) != NULL ){
			return setParameter( position , instruction );
		}
	}
	return NULL;
}

static char *setIMP( char *position , Instruction *instruction ){
	switch( *position++ ){
		case ' ':
			instruction->imp = STACK;
			break;
			
		case '\t':
			switch( *position++ ){
				case ' ':
					instruction->imp = OPERATION;
					break;
					
				case '\t':
					instruction->imp = HEAP;
					break;
					
				case '\n':
					instruction->imp = IO;
					break;
					
				default:
					error( "end program" );
					return NULL;
			}
			break;
			
		case '\n':
			instruction->imp = FLOW_CONTROL;
			break;
			
		default:
			error( "end program" );
			return NULL;
	}
	return position;
}

static char *setCommand( char *position , Instruction *instruction ){
	switch( instruction->imp ){
		case STACK:
			position = setStackCommand( position , instruction );
			break;
			
		case OPERATION:
			position = setOperationCommand( position , instruction );
			break;
			
		case HEAP:
			position = setHeapCommand( position , instruction );
			break;
			
		case FLOW_CONTROL:
			position = setControlCommand( position , instruction );
			break;
			
		case IO:
			position = setIOCommand( position , instruction );
			break;
			
		default:
			error( "do not set IMP" );
			return NULL;
	}
	return position;
}

static char *setStackCommand( char *position , Instruction *instruction ){
	switch( *position++ ){
		case ' ':
			instruction->c_stack = PUSH_NUMBER;
			break;
			
		case '\t':
			switch( *position++ ){
				case ' ':
					instruction->c_stack = N_COPY;
					break;
					
				case '\n':
					instruction->c_stack = N_SLIDE;
					break;
					
				default:
					error( "illegal command." );
					return NULL;
			}
			
		case '\n':
			switch( *position++ ){
				case ' ':
					instruction->c_stack = TOP_COPY;
					break;
					
				case '\t':
					instruction->c_stack = PUSH_EXCHANGE;
					break;
					
				case '\n':
					instruction->c_stack = TOP_DESTRUCTION;
					break;
					
				default:
					error( "illegal command." );
					return NULL;
			}
			break;
			
		default:
			error( "illegal command" );
			return NULL;
	}
	return position;
}

static char *setOperationCommand( char *position , Instruction *instruction ){
	switch( *position++ ){
		case ' ':
			switch( *position++ ){
				case ' ':
					instruction->c_operation = ADDTION;
					break;
					
				case '\t':
					instruction->c_operation = SUBTRACTION;
					break;
					
				case '\n':
					instruction->c_operation = MULTIPLICATION;
					break;
					
				default:
					error( "illegal command" );
					return NULL;
			}
			break;
			
		case '\t':
			switch( *position++ ){
				case ' ':
					instruction->c_operation = DIVISION;
					break;
					
				case '\t':
					instruction->c_operation = MODULO;
					break;
					
				default:
					error( "illegal command" );
					return NULL;
			}
			break;
			
		default:
			error( "illegal command" );
			return NULL;
	}
	return position;
}

static char *setHeapCommand( char *position , Instruction *instruction ){
	switch( *position++ ){
		case ' ':
			instruction->c_heap = TO_ADDRESS;
			break;
			
		case '\t':
			instruction->c_heap = TO_STACK;
			break;
			
		default:
			error( "illegal command" );
			return NULL;
	}
	return position;
}

static char *setControlCommand( char *position , Instruction *instruction ){
	switch( *position++ ){
		case ' ':
			switch( *position++ ){
				case ' ':
					instruction->c_control = LABEL_DEFINE;
					break;
					
				case '\t':
					instruction->c_control = CALL_ROUTINE;
					break;
					
				case '\n':
					instruction->c_control = JUMP;
					break;
					
				default:
					error( "illegal command" );
					return NULL;
			}
			break;
			
		case '\t':
			switch( *position++ ){
				case ' ':
					instruction->c_control = ZERO_JUMP;
					break;
					
				case '\t':
					instruction->c_control = MINUS_JUMP;
					break;
					
				case '\n':
					instruction->c_control = END_ROUTINE;
					break;
					
				default:
					error( "illegal command" );
					return NULL;
			}
			break;
			
		case '\n':
			if( *position++ == '\n' ){
				instruction->c_control = FINISH;
			}
			else{
				error( "illegal command" );
				return NULL;
			}
			break;
			
		default:
			error( "illegal command" );
			return NULL;
	}
	return position;
}

static char *setIOCommand( char *position , Instruction *instruction ){
	switch( *position++ ){
		case ' ':
			switch( *position++ ){
				case ' ':
					instruction->c_io = PUT_CHAR;
					break;
					
				case '\t':
					instruction->c_io = PUT_NUMBER;
					break;
					
				default:
					error( "illegal command" );
					return NULL;
			}
			break;
			
		case '\t':
			switch( *position++ ){
				case ' ':
					instruction->c_io = GET_CHAR;
					break;
					
				case '\t':
					instruction->c_io = GET_NUMBER;
					break;
					
				default:
					error( "illegal command" );
					return NULL;
			}
			break;
			
		default:
			error( "illegal command" );
			return NULL;
			
	}
	return position;
}

static char *setParameter( char *position , Instruction *instruction ){
	if( instruction->imp == STACK && instruction->c_stack == PUSH_NUMBER ){
		position = setNumber( position , instruction );
	}
	else if( instruction->imp == FLOW_CONTROL ){
		if( instruction->c_control != END_ROUTINE && instruction->c_control != FINISH ){
			position = setLabel( position , instruction );
			if( instruction->c_control == LABEL_DEFINE ){
				addLabel( instruction->p_label , instruction );
			}
		}
	}
	return position;
}

static char *setNumber( char *position , Instruction *instruction ){
	bool minus = false;
	if( *position == '\t' || *position == ' ' ){
		minus = *position++ == '\t';
	}
	else{
		error( "illegal number parameter. do not have sign" );
		return NULL;
	}
	long number = 0;
	while( true ){
		switch( *position++ ){
			case '\t':
				number <<= 1;
				number++;
				break;
				
			case ' ':
				number <<= 1;
				break;
				
			case '\n':
				goto END_SET_NUMBER;
				
			default:
				error( "illegal number parameter." );
				return NULL;
		}
	}
	
END_SET_NUMBER:
	number &= 0x7FFFFFFFFFFFFFFF;
	if( minus ){
		number *= -1;
	}
	instruction->p_value = number;
	number &= 0xFF;
	return position;
}

static char *setLabel( char *position , Instruction *instruction ){
	char buffer[BUFFER_SIZE];
	memset( buffer , 0 , BUFFER_SIZE );
	int index = 0 , count = 0;
	size_t size;
	while( BUFFER_SIZE - index ){
		index = count >> 3;
		switch( *position++ ){
			case '\t':
				buffer[index] <<= 1;
				buffer[index]++;
				break;
				
			case ' ':
				buffer[index] <<= 1;
				break;
				
			case '\n':
				goto END_SET_LABEL;
				
			default:
				error( "illegal label parameter" );
				return NULL;
		}
		count++;
	}
	error( "illegal label parameter: overflow." );
	return NULL;
	
END_SET_LABEL:
	size = sizeof( char ) * ( ( count >> 3 ) + 2 );
	if( ( instruction->p_label = ( char * ) malloc( size ) ) == NULL ){
		error( "out of memory error" );
		exit( EXIT_FAILURE );
	}
	memcpy( instruction->p_label , buffer , size );
	return position;
}

static void addLabel( char *label , Instruction *instruction ){
	if( labelMap == NULL ){
		labelMap = newHashMap( HASH_MAP_TYPE_VOID );
	}
	putHashMapValue( labelMap , label , instruction , sizeof( Instruction * ) );
	return;
}

static void setRelation( Instruction *instruction ){
	Instruction *stack[labelMap->aSize];
	int count = 0;
	while( instruction->next != NULL ){
		if( instruction->imp == FLOW_CONTROL ){
			switch( instruction->c_control ){
				case LABEL_DEFINE:
					stack[count++] = instruction;
					break;
					
				case END_ROUTINE:
					stack[--count]->jump = instruction;
					break;

				case CALL_ROUTINE:
					// FALL THROUGH
					
				case JUMP:
					// FALL THROUGH
					
				case ZERO_JUMP:
					// FALL THROUGH
					
				case MINUS_JUMP:
					instruction->jump = getInstructionAtLabel( instruction->p_label );
					break;
										
				default:
					break;
			}
		}
		instruction = instruction->next;
	}
	return;
}

static void error( char *message ){
	fputs( message , stderr );
	fputc( '\n' , stderr );
	return;
}
