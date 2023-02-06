#ifndef POSTFIX_H
#define POSTFIX_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

// return the operator precedence
int getPrecedence(char *op) {
	if (strcmp(op, "^") == 0) {
		return 5;
	} else if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
		return 4;
	} else if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) {
		return 3;
	} else if (strcmp(op, ">") == 0 || strcmp(op, ">=") == 0 || strcmp(op, "<") == 0 || strcmp(op, "<=") == 0 || strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
		return 2;
	} else if (strcmp(op, "=") == 0) {
		return 1;
	} else {
		return 0;
	}
}

// return true if the operator is left associative
int isLeftAssociative(char *op) {
	if (strcmp(op, "^") == 0) {
		return 0;
	} else if (strcmp(op, "-u") == 0) {
		return 0;
	} else {
		return 1;
	}
}

// convert an infix expression to a postfix tokens list
int infixToPostfix(char *expr, tokenStack **tokens) {
	// define a stack for operators
	tokenStack *operatorStack = NULL;
	// define a token buffer
	char token[MAX_TOKEN_LENGTH];
	// define a token buffer for the top of the operator stack
	char topToken[MAX_TOKEN_LENGTH];

	// define a token type
	int type  = TOKEN_END;
	// define an error code
	int error = 0;

	// read the expression from left to right for a token
	while ((error = nextToken(expr, &type, token)) == ERROR_NONE && type != TOKEN_END) {
		// if the token is a number, a string or a variable, append it to the output list
		if (type == TOKEN_NUMBER || type == TOKEN_STRING || type == TOKEN_VARIABLE) {
			if ((error = appendToken(tokens, token, type)) != ERROR_NONE) {
				return error;
			}
		}
		// if the token is a function or an unary, push it onto the operator stack
		else if (type == TOKEN_FUNCTION || type == TOKEN_UNARY) {
			if ((error = pushToken(&operatorStack, token, type)) != ERROR_NONE) {
				return error;
			}
		}
		// if the token is an operator
		else if (type == TOKEN_OPERATOR) {
			// while the operator at the top of the operator stack has greater precedence than the token or the operator at the top of the operator stack is left associative and has equal precedence to the token
			while (operatorStack != NULL && (getPrecedence(operatorStack->token) > getPrecedence(token) || (getPrecedence(operatorStack->token) == getPrecedence(token) && isLeftAssociative(operatorStack->token)))) {
				if ((error = popToken(&operatorStack, topToken)) != ERROR_NONE) {
					return error;
				}
				if ((error = appendToken(tokens, topToken, type)) != ERROR_NONE) {
					return error;
				}
			}
			// push the new token onto the operator stack
			if ((error = pushToken(&operatorStack, token, type)) != ERROR_NONE) {
				return error;
			}
		}
		// if the token is a left parenthesis, push it onto the operator stack
		else if (type == TOKEN_L_PAREN) {
			if ((error = pushToken(&operatorStack, token, type)) != ERROR_NONE) {
				return error;
			}
			pLevel++;
		}
		// if the token is a right parenthesis
		else if (type == TOKEN_R_PAREN) {
			// while the operator at the top of the operator stack is not a left parenthesis pop the operator from the operator stack and append it to the output list
			while (operatorStack != NULL && operatorStack->type != TOKEN_L_PAREN) {
				if ((error = popToken(&operatorStack, topToken)) != ERROR_NONE) {
					return error;
				}
				if ((error = appendToken(tokens, topToken, type)) != ERROR_NONE) {
					return error;
				}
			}
			if (operatorStack == NULL) {
				return ERROR_SYNTAX;
			}
			if ((error = popToken(&operatorStack, token)) != ERROR_NONE) {
				return error;
			}
			// if the operator at the top of the operator stack is a function, pop the operator from the operator stack and append it to the output list
			if (operatorStack != NULL && operatorStack->type == TOKEN_FUNCTION) {
				if ((error = popToken(&operatorStack, topToken)) != ERROR_NONE) {
					return error;
				}
				if ((error = appendToken(tokens, topToken, type)) != ERROR_NONE) {
					return error;
				}
			}
			pLevel--;
		}
		// if the token is a left bracket push it onto the operator stack
		else if (type == TOKEN_L_BRACKET) {
			if ((error = pushToken(&operatorStack, token, type)) != ERROR_NONE) {
				return error;
			}
			bLevel++;
			if ((error = appendToken(tokens, token, type)) != ERROR_NONE) {
				return error;
			}
		}
		// if the token is a right bracket
		else if (type == TOKEN_R_BRACKET) {
			// while the operator at the top of the operator stack is not a left bracket pop the operator from the operator stack and append it to the output list
			while (operatorStack != NULL && operatorStack->type != TOKEN_L_BRACKET) {
				if ((error = popToken(&operatorStack, topToken)) != ERROR_NONE) {
					return error;
				}
				if ((error = appendToken(tokens, topToken, type)) != ERROR_NONE) {
					return error;
				}
			}
			if (operatorStack == NULL) {
				return ERROR_SYNTAX;
			}
			if ((error = appendToken(tokens, token, type)) != ERROR_NONE) {
				return error;
			}
			if ((error = popToken(&operatorStack, token)) != ERROR_NONE) {
				return error;
			}
			bLevel--;
		}

		// if the token is a comma
		else if (type == TOKEN_COMMA) {
			// while the operator at the top of the operator stack is not a left parenthesis or a left bracket pop the operator from the operator stack and append it to the output list
			while (operatorStack != NULL && operatorStack->type != TOKEN_L_PAREN && operatorStack->type != TOKEN_L_BRACKET) {
				if ((error = popToken(&operatorStack, topToken)) != ERROR_NONE) {
					return error;
				}
				if ((error = appendToken(tokens, topToken, type)) != ERROR_NONE) {
					return error;
				}
			}
			if (operatorStack == NULL) {
				return ERROR_SYNTAX;
			}
		}
		pType = type;
	}
	if (error != ERROR_NONE) {
		return error;
	}
	if (pLevel != 0) {
		return ERROR_SYNTAX;
	}
	if (bLevel != 0) {
		return ERROR_SYNTAX;
	}
	// while there are still operators on the operator stack, pop the operator from the operator stack and append it to the output string
	while (operatorStack != NULL) {
		if ((error = popToken(&operatorStack, topToken)) != ERROR_NONE) {
			return error;
		}
		if ((error = appendToken(tokens, topToken, type)) != ERROR_NONE) {
			return error;
		}
	}

	return ERROR_NONE;
}

#endif