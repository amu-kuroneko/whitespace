//
//  show.c
//  whitespace
//
//  Created by kuroneko on 2015/01/18.
//  Copyright (c) 2015年 kuroneko. All rights reserved.
//

#include "whitespace.h"

/**
 * 逆アセンブルしたスタック操作コマンドの表示を行う
 * @param instruction
 *	表示する命令
 */
static void showStack( Instruction *instruction );

/**
 * 逆アセンブルした演算コマンドの表示を行う
 * @param instruction
 *	表示する命令
 */
static void showOperation( Instruction *instruction );

/**
 * 逆アセンブルしたヒープアクセスコマンドの表示を行う
 * @param instruction
 *	表示する命令
 */
static void showHeap( Instruction *instruction );

/**
 * 逆アセンブルしたフロー制御コマンドの表示を行う
 * @param instruction
 *	表示する命令
 */
static void showFlowControl( Instruction *instruction );

/**
 * 逆アセンブルした入出力コマンドの表示を行う
 * @param instruction
 *	表示する命令
 */
static void showIO( Instruction *instruction );



void disassemble( Instruction *instruction ){
	while( instruction != NULL ){
		switch( instruction->imp ){
			case STACK:
				fprintf( stdout , "%-15s: " , "stack" );
				showStack( instruction );
				break;
				
			case OPERATION:
				fprintf( stdout , "%-15s: " , "operation" );
				showOperation( instruction );
				break;
				
			case HEAP:
				fprintf( stdout , "%-15s: " , "heap" );
				showHeap( instruction );
				break;
				
			case FLOW_CONTROL:
				fprintf( stdout , "%-15s: " , "flow control" );
				showFlowControl( instruction );
				break;
				
			case IO:
				fprintf( stdout , "%-15s: " , "io" );
				showIO( instruction );
				break;
				
			default:
				break;
		}
		fputc( '\n' , stdout );
		instruction = instruction->next;
	}
	return;
}

static void showStack( Instruction *instruction ){
	switch( instruction->c_stack ){
		case PUSH_NUMBER:
			fprintf( stdout , "%-20s: 0x%08lx( %ld )" , "push number" , instruction->p_value , instruction->p_value );
			break;
			
		case TOP_COPY:
			fprintf( stdout , "%-20s" , "top copy" );
			break;
			
		case N_COPY:
			fprintf( stdout , "%-20s: 0x%08lx( %ld )" , "n position copy" , instruction->p_value , instruction->p_value );
			break;
			
		case PUSH_EXCHANGE:
			fprintf( stdout , "%-20s" , "exchange" );
			break;
			
		case TOP_DESTRUCTION:
			fprintf( stdout , "%-20s" , "top destruction" );
			break;
			
		case N_SLIDE:
			fprintf( stdout , "%-20s: 0x%08lx( %ld ) " , "n slide and keep top" , instruction->p_value , instruction->p_value );
			
		default:
			break;
	}
	return;
}


static void showOperation( Instruction *instruction ){
	switch( instruction->c_operation ){
		case ADDTION:
			fprintf( stdout , "%-20s" , "addtion" );
			break;
			
		case SUBTRACTION:
			fprintf( stdout , "%-20s" , "subtraction" );
			break;
			
		case MULTIPLICATION:
			fprintf( stdout , "%-20s" , "multiplication" );
			break;
			
		case DIVISION:
			fprintf( stdout , "%-20s" , "division" );
			break;
			
		case MODULO:
			fprintf( stdout , "%-20s" , "modulo" );
			break;
			
		default :
			break;
	}
	return;
}

static void showHeap( Instruction *instruction ){
	switch( instruction->c_heap ){
		case TO_ADDRESS:
			fprintf( stdout , "%-20s" , "to address" );
			break;
			
		case TO_STACK:
			fprintf( stdout , "%-20s" , "to stack" );
			break;
			
		default:
			break;
	}
	return;
}

static void showFlowControl( Instruction *instruction ){
	switch( instruction->c_control ){
		case LABEL_DEFINE:
			fprintf( stdout , "%-20s: %s" , "label define" , instruction->p_label );
			break;
			
		case CALL_ROUTINE:
			fprintf( stdout , "%-20s: %s" , "call sub routine" , instruction->p_label );
			break;
			
		case JUMP:
			fprintf( stdout , "%-20s: %s" , "jump" , instruction->p_label );
			break;
			
		case ZERO_JUMP:
			fprintf( stdout , "%-20s: %s" , "if zero jump" , instruction->p_label );
			break;
			
		case MINUS_JUMP:
			fprintf( stdout , "%-20s: %s" , "if minus jump" , instruction->p_label );
			break;
			
		case END_ROUTINE:
			fprintf( stdout , "%-20s" , "end sub routine" );
			break;
			
		case FINISH:
			fprintf( stdout , "%-20s" , "finish" );
			break;
	}
	return;
}

static void showIO( Instruction *instruction ){
	switch( instruction->c_io ){
		case PUT_CHAR:
			fprintf( stdout , "%-20s" , "put charactor" );
			break;
			
		case PUT_NUMBER:
			fprintf( stdout , "%-20s" , "put number" );
			break;
			
		case GET_CHAR:
			fprintf( stdout , "%-20s" , "get charactor" );
			break;
			
		case GET_NUMBER:
			fprintf( stdout , "%-20s" , "get number" );
			break;
			
		default:
			break;
	}
	return;
}
