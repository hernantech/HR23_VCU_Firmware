/*
******************************************************************************
	General Purpose Error Handler Functions
	Generic Board Library
	Erik Stafl
	12/13/2015

	Written for Tiva Series MCUs
	Language: C++

	Copyright (c) 2013-2015 Stafl Systems, LLC.  All rights reserved.
******************************************************************************
*/

#ifndef ERROR_H_
#define ERROR_H_

// Log an Error Condition
void error(char* description, char* filename, unsigned int line_number);

#endif /* ERROR_H_ */
