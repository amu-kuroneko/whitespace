whitespace
====

# Overview

whitespace の実行プログラム

## Description

[whitespace](http://compsoc.dur.ac.uk/whitespace/) をC言語で実装したプログラム  
またwhitespaceのソースコードを見やすくするために色付けを行うプログラム

## Requirement

`OS` Mac OS X  
`gcc` Apple LLVM version 5.1 (clang-503.0.40) (based on LLVM 3.4svn)  
`Make` GNU Make 3.81  
`cURL` curl 7.39.0 (x86_64-apple-darwin12.5.0) libcurl/7.39.0 OpenSSL/1.0.1j zlib/1.2.8 libidn/1.29

## Usage

	$ make
	$ cat <whitespace program> | ./kwsc > <destination of coloring text file>
	$ ./kws -f <whitespace program>

## Author

[kuroneko](https://github.com/amu-kuroneko)
