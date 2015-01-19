//
//  whitespace.h
//  whitespace
//
//  Created by kuroneko on 2015/01/18.
//  Copyright (c) 2015年 kuroneko. All rights reserved.
//

#ifndef whitespace_whitespace_h
#define whitespace_whitespace_h

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>

	/**
	 * ファイル読込時のオプション
	 */
	#define FILE_OPTION "-f"

	/**
	 * 文字入力を受け付ける場合等で使用するバッファサイズ
	 */
	#define BUFFER_SIZE 1024

	/**
	 * ヒープ領域の確保サイズ
	 * 足りなくなったら、このサイズを追加して更に確保する
	 */
	#define HEAP_ALLOCATION_SIZE 256

	/**
	 * スタック領域の確保サイズ
	 * 足りなくなったら、このサイズを追加して更に確保する
	 */
	#define STACK_ALLOCATION_SIZE 256

	/**
	 * 命令変更パラメータ
	 */
	enum{
		STACK ,			// スタック操作
		OPERATION ,		// 演算
		HEAP ,			// ヒープアクセス
		FLOW_CONTROL ,	// フロー制御
		IO				// 入出力
	} typedef IMPType;

	/**
	 * スタック操作コマンド
	 */
	enum{
		PUSH_NUMBER ,		// スタックにプッシュする
		TOP_COPY ,			// スタックの1個目の値をプッシュする
		N_COPY ,			// スタックのn個目の値をプッシュする
		PUSH_EXCHANGE ,		// スタックの1個目と2個目の値を入れ替える
		TOP_DESTRUCTION ,	// スタックの1個目の値を削除する
		N_SLIDE				// スタックの1個目の値を残してn個スライスする
	} typedef Stack;

	/**
	 * 演算コマンド
	 */
	enum{
		ADDTION ,			// 足し算
		SUBTRACTION ,		// 引き算
		MULTIPLICATION ,	// 掛け算
		DIVISION ,			// 割り算
		MODULO				// 余剰
	} typedef Operation;

	/**
	 * ヒープアクセスコマンド
	 */
	enum{
		TO_ADDRESS ,	// ヒープに値を保存する
		TO_STACK		// ヒープの値をスタックにプッシュする
	} typedef Heap;

	/**
	 * フロー制御コマンド
	 */
	enum{
		LABEL_DEFINE ,	// ラベル定義
		CALL_ROUTINE ,	// サブルーチン呼び出し
		JUMP ,			// 無条件ジャンプ
		ZERO_JUMP ,		// スタックの1個目が0の場合にジャンプ
		MINUS_JUMP ,	// スタックの1個目が負の場合にジャンプ
		END_ROUTINE ,	// サブルーチン終了
		FINISH			// プログラム終了
	} typedef Control;

	/**
	 * 入出力コマンド
	 */
	enum{
		PUT_CHAR ,		// スタックの1個目の値を文字として出力
		PUT_NUMBER ,	// スタックの1個目の値を数値として出力
		GET_CHAR ,		// 入力された文字をスタックの1個目の値のアドレスに保存
		GET_NUMBER		// 入力された数値をスタックの1個目の値のアドレスに保存
	} typedef IOControl;

	/**
	 * 命令を保持する構造体
	 */
	struct instruction{
		IMPType imp;				// 命令パラメータ
		union{
			Stack stack;			// スタック操作コマンド
			Operation operation;	// 演算コマンド
			Heap heap;				// ヒープアクセスコマンド
			Control control;		// フロー制御コマンド
			IOControl io;			// 入出力コマンド
		} command;
		union{
			long value;				// 数値パラメータ
			char *label;			// ラベル
		} parameter;
		struct instruction *next;	// 次の命令
		struct instruction *jump;	// ジャンプ時やサブルーチン呼び出し時に実行する命令
	} typedef Instruction;

	// Instruction のエイリアス
	#define c_stack command.stack
	#define c_operation command.operation
	#define c_heap command.heap
	#define c_control command.control
	#define c_io command.io
	#define p_value parameter.value
	#define p_label parameter.label
	// ここまで


	// prepare.c

	/**
	 * 読み込んだプログラムを破棄する
	 */
	void programClear( void );

	/**
	 * プログラムの読込みを行う
	 * @param source
	 *	プログラムのソースコード
	 * @param size
	 *	プログラムのサイズ
	 */
	void setProgram( char *source , size_t size );

	/**
	 * コメントを削除した、読み込んだプログラムを取得する
	 * @param size
	 *	最適化されたプログラムのサイズが格納される
	 * @return
	 *	最適化されたプログラムソース
	 */
	char *getOptimizationProgram( size_t *size );

	/**
	 * 読み込んだプログラムから命令セットを作成して取得する
	 * @return
	 *	命令セット
	 */
	Instruction *getInstruction( void );

	/**
	 * 命令セットを開放する
	 * @param instruction
	 *	開放する命令セット
	 */
	void freeInstruction( Instruction *instruction );

	/**
	 * ラベルから命令を取得する
	 * @param label
	 *	ラベル
	 * @return
	 *	指定したラベルの命令
	 */
	Instruction *getInstructionAtLabel( char *label );


	// execute.c

	/**
	 * プログラムを実行する
	 * @param instruction
	 *	実行する命令セット
	 */
	void execute( Instruction *instruction );

	/**
	 * プログラムの実行により確保されたスタックを破棄する
	 */
	void stackClear( void );

	/**
	 * プログラムの実行により確保されたヒープを破棄する
	 */
	void heapClear( void );


	// show.c

	/**
	 * プログラムの逆アセンブルを行う
	 * @param instruction
	 *	逆アセンブルを行う命令セット
	 */
	void disassemble( Instruction *instruction );

#endif
