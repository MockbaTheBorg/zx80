#ifndef EVAL_H
#define EVAL_H

#include <math.h>

// evaluate an unary
int evalUnary(tokenStack **resultStack) {
	// define a token buffer for operator
	char op[MAX_TOKEN_LENGTH];
	int opType = TOKEN_END;
	// define a token buffer for operand 1
	char op1[MAX_TOKEN_LENGTH];
	int op1Type = TOKEN_END;
	// define a token buffer for the result
	char result[MAX_TOKEN_LENGTH];
	int resultType = TOKEN_END;
	// define an error code
	int error	   = 0;

	// pop the operator from the evaluation stack
	if ((error = popToken(resultStack, op, &opType)) != ERROR_NONE) {
		return error;
	}
	// pop the first operand from the evaluation stack
	if ((error = popToken(resultStack, op1, &op1Type)) != ERROR_NONE) {
		return error;
	}
	// if the operator is a minus unary
	if (strcmp(op, "-u") == 0) {
		// if the operand is a number
		if (op1Type == TOKEN_NUMBER) {
			// negate the number
			sprintf(result, "%f", -atof(op1));
		}
	}
	// if the operator is a plus unary
	else if (strcmp(op, "+u") == 0) {
		// if the operand is a number
		if (op1Type == TOKEN_NUMBER) {
			// copy the number
			sprintf(result, "%f", -atof(op1));
		}
	} else {
		return ERROR_UNKNOWN_OPERATOR;
	}
	// push the result onto the evaluation stack
	if ((error = pushToken(resultStack, result, TOKEN_NUMBER)) != ERROR_NONE) {
		return error;
	}
	return ERROR_NONE;
}

// evaluate an operator
int evalOperator(tokenStack **resultStack) {
	// define a token buffer for operator
	char op[MAX_TOKEN_LENGTH];
	int opType = TOKEN_END;
	// define a token buffer for operand 1
	char op1[MAX_TOKEN_LENGTH];
	int op1Type = TOKEN_END;
	// define a token buffer for operand 2
	char op2[MAX_TOKEN_LENGTH];
	int op2Type = TOKEN_END;
	// define a token buffer for the result
	char result[MAX_TOKEN_LENGTH];
	int resultType = TOKEN_END;
	// define an error code
	int error	   = 0;

	// pop the operator from the evaluation stack
	if ((error = popToken(resultStack, op, &opType)) != ERROR_NONE) {
		return error;
	}
	// pop the second operand from the evaluation stack
	if ((error = popToken(resultStack, op2, &op2Type)) != ERROR_NONE) {
		return error;
	}
	// pop the first operand from the evaluation stack
	if ((error = popToken(resultStack, op1, &op1Type)) != ERROR_NONE) {
		return error;
	}
	// if the operator is a plus
	if (strcmp(op, "+") == 0) {
		// if both operands are numbers
		if (op1Type == TOKEN_NUMBER && op2Type == TOKEN_NUMBER) {
			// add the numbers
			sprintf(result, "%f", atof(op1) + atof(op2));
			// set the result type to number
			resultType = TOKEN_NUMBER;
		}
	}
	// if the operator is a minus
	else if (strcmp(op, "-") == 0) {
		// if both operands are numbers
		if (op1Type == TOKEN_NUMBER && op2Type == TOKEN_NUMBER) {
			// subtract the numbers
			sprintf(result, "%f", atof(op1) - atof(op2));
			// set the result type to number
			resultType = TOKEN_NUMBER;
		}
	}
	// if the operator is a multiplication
	else if (strcmp(op, "*") == 0) {
		// if both operands are numbers
		if (op1Type == TOKEN_NUMBER && op2Type == TOKEN_NUMBER) {
			// multiply the numbers
			sprintf(result, "%f", atof(op1) * atof(op2));
			// set the result type to number
			resultType = TOKEN_NUMBER;
		}
	}
	// if the operator is a division
	else if (strcmp(op, "/") == 0) {
		// if both operands are numbers
		if (op1Type == TOKEN_NUMBER && op2Type == TOKEN_NUMBER) {
			// divide the numbers
			sprintf(result, "%f", atof(op1) / atof(op2));
			// set the result type to number
			resultType = TOKEN_NUMBER;
		}
	}
	// if the operator is a power
	else if (strcmp(op, "^") == 0) {
		// if both operands are numbers
		if (op1Type == TOKEN_NUMBER && op2Type == TOKEN_NUMBER) {
			// raise the first operand to the power of the second operand
			sprintf(result, "%f", pow(atof(op1), atof(op2)));
			// set the result type to number
			resultType = TOKEN_NUMBER;
		}
	}
	// if the operator is a modulo
	else if (strcmp(op, "%") == 0) {
		// if both operands are numbers
		if (op1Type == TOKEN_NUMBER && op2Type == TOKEN_NUMBER) {
			// calculate the modulo of the first operand by the second operand
			sprintf(result, "%f", fmod(atof(op1), atof(op2)));
			// set the result type to number
			resultType = TOKEN_NUMBER;
		}
	} else {
		return ERROR_UNKNOWN_OPERATOR;
	}
	// push the result onto the evaluation stack
	if ((error = pushToken(resultStack, result, TOKEN_NUMBER)) != ERROR_NONE) {
		return error;
	}
	return ERROR_NONE;
}

// function to evaluate an expression
int eval(char *expr, tokenStack **resultStack) {
	// define a token
	char *token = NULL;
	// define a token type
	int type	= TOKEN_END;

	// define a token stack for the postfix expression
	tokenStack *postfix = NULL;

	// define an error code
	int error = 0;

	error = infixToPostfix(expr, &postfix);
	if (error != ERROR_NONE) {
		return error;
	}

	while (postfix != NULL) {
        token = postfix->token;
		type = postfix->type;
		// if the token is a number, a string or a variable, push it onto the evaluation stack
		if (type == TOKEN_NUMBER || type == TOKEN_STRING || type == TOKEN_VARIABLE) {
			if ((error = pushToken(resultStack, token, type)) != ERROR_NONE) {
				return error;
			}
		}
		// if the token is an unary
		else if (type == TOKEN_UNARY) {
			// push the token onto the evaluation stack
			if ((error = pushToken(resultStack, token, type)) != ERROR_NONE) {
				return error;
			}
			// evaluate the unary
			if ((error = evalUnary(resultStack)) != ERROR_NONE) {
				return error;
			}
		}
		// if the token is an operator
		else if (type == TOKEN_OPERATOR) {
			// push the token onto the evaluation stack
			if ((error = pushToken(resultStack, token, type)) != ERROR_NONE) {
				return error;
			}
			// evaluate the operator
			if ((error = evalOperator(resultStack)) != ERROR_NONE) {
				return error;
			}
		}
		postfix = postfix->next;
	}
}
#endif