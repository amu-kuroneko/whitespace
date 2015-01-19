//
//  hash.h
//  kuroneko-lib
//
//  Created by kuroneko on 2014/02/27.
//  Copyright (c) 2014年 kuroneko. All rights reserved.
//

#ifndef kuroneko_hash_h
#define kuroneko_hash_h

	// include
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>

	// define

	/**
	 * ハッシュ値の最大値
	 */
	#define K_UTIL_HASH_SIZE 64

	/**
	 * ハッシュ管理を行う値のタイプ
	 */
	enum{
		HASH_MAP_TYPE_VOID = 0 ,		// void 型 ポインタで扱う場合はこの型を指定する
		HASH_MAP_TYPE_CHAR = 1 ,		// char 型
		HASH_MAP_TYPE_SHORT = 2 ,		// short 型
		HASH_MAP_TYPE_INTEGER = 3 ,		// int 型
		HASH_MAP_TYPE_LONG = 4 ,		// long 型
		HASH_MAP_TYPE_LONG_LONG = 5 ,	// long long 型
		HASH_MAP_TYPE_FLOAT = 6 ,		// float 型
		HASH_MAP_TYPE_DOUBLE = 7 ,		// double 型
		HASH_MAP_TYPE_STRING = 8		// 文字列 型
	} typedef ValueType;

	/**
	 * ハッシュ内部で管理しているキーと値の構造体
	 * ハッシュ値が衝突した場合はその後はリスト構造となる
	 */
	struct KHashKeyValue{
		char *aKeyword;					// キー
		void *aValue;					// 値
		unsigned int aValueSize;		// 値のサイズ
		struct KHashKeyValue *aNext;	// ハッシュ値が衝突した場合の次の値
	} typedef HashKeyValue;

	/**
	 * ハッシュマップの構造体
	 */
	struct{
		HashKeyValue **values;			// キーと値の構造体の配列
		ValueType aType;				// 管理する値のタイプ
		unsigned int aSize;				// 管理している値の数
		unsigned int aLength;			// ハッシュ値の最大長
	} typedef HashMap;

	/**
	 * 新しい HashMap を生成する
	 * @param aType
	 *  設定する値のタイプ
	 * @return
	 *  新しい HashMap
	 */
	HashMap* newHashMap( const ValueType aType );

	/**
	 * HashMap を開放する
	 * @param aMap
	 *  開放する HashMap
	 */
	void finishHashMap( HashMap *aMap );

	/**
	 * キーと値を関連付けて保存する
	 * @param aMap
	 *  保存する HashMap
	 * @param aKey
	 *  キー文字列
	 * @param aValue
	 *  値のポインタ
	 * @param aValueSize
	 *  値のサイズ
	 *  文字列であれば文字数
	 *  それ以外であれば sizeof で取得出来る値
	 *  ただしポインタのサイズではない
	 */
	void putHashMapValue( HashMap *aMap , char *aKey , void *aValue , const int aValueSize );

	/**
	 * ハッシュマップ内の全ての値を取得する
	 * @param aMap
	 *	取得する HashMap
	 * @param container
	 *	ハッシュマップ内の値を保存する配列
	 *	予め aMap->aSize の大きさ分のメモリを確保しておく必要がある
	 *	aMap->aType が HASH_MAP_TYPE_VOID の場合はポインタが格納される
	 *	aMap->aType が HASH_MAP_TYPE_STRING の場合はハッシュマップ内にコピーされた文字列のポインタが格納される
	 *	aMap->aType がそれ以外の場合は、それぞれの型に対応したリテラル数が格納される
	 */
	void getAllHashValue( HashMap *aMap , void *container );

	/**
	 * 指定したキーが登録されているかを確認する
	 * @param aMap
	 *  検索する HashMap
	 * @param aKey
	 *  キー文字列
	 * @return
	 *  キーが存在する場合に true を返す
	 */
	void *getHashValue( HashMap *aMap , char *aKey );

	/**
	 * 指定したキーに対応する値を取得する
	 * @param aMap
	 *  値を取得する HashMap
	 * @param aKey
	 *  キー文字列
	 * @return
	 *  対応する値が存在する場合に、その値を返す
	 *  存在しない場合は NULL を返す
	 */
	bool hasHashKeyword( HashMap *aMap , char *aKey );

	/**
	 * Map サイズの再設定を行う
	 * @param aMap
	 *  再設定を行う HashMap
	 * @param aLength
	 *  変更サイズ
	 */
	void resizeHashMap( HashMap *aMap , int aLength );

	/**
	 * HashMap の内容を全て標準出力で表示する
	 * @param aMap
	 *  表示する HashMap
	 */
	void showHashMap( HashMap *aMap );

#endif
