#pragma once// This is a generic parser object that takes a string and attempts to build//	a sequence of tokens from the input.#include	"Tuple.h"class	Parser	{	public:				Parser( char* inString=0L );				virtual	~Parser();			Tuple&	GetToken();		Tuple&	GetNextToken();		// Lookahead.		char*	GetFormat()				{ return mFormat; }		void	SetDecimal( char dc )	{ mDecimal = dc; }				void	SetThousands( char tc )	{ mThousands = tc; }					protected:		void	SetStream( char* inString );		void	lexical( char* tokStr );		char	peekChar(short i=0)		{ return mCurrPos[i]; }	// lookahead		char	getChar()				{ return *mCurrPos++; }		void	pushBack()				{ mCurrPos--; }							private:			bool		mHasLookAhead;		Tuple		mLookAhead;		char*		mInString;		char*		mCurrPos;		char		mFormat[20];		char		mThousands;		char		mDecimal;						};				