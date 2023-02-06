#ifndef TOKEN_H
#define TOKEN_H

// define the maximum length of the token
#define MAX_TOKEN_LENGTH 512

// enumerate the token types
enum tokenTypes {
	TOKEN_END,
	TOKEN_UNARY,
	TOKEN_NUMBER,
	TOKEN_STRING,
	TOKEN_OPERATOR,
	TOKEN_VARIABLE,
	TOKEN_FUNCTION,
	TOKEN_COMMA,
	TOKEN_L_PAREN,
	TOKEN_R_PAREN,
	TOKEN_L_BRACKET,
	TOKEN_R_BRACKET,
	TOKEN_ERROR
};

// enumerate the token text
char *tokenText[] = {"End of expression",
					 "Unary operator",
					 "Number",
					 "String",
					 "Operator",
					 "Variable",
					 "Function",
					 "Comma",
					 "Left parenthesis",
					 "Right parenthesis",
					 "Left bracket",
					 "Right bracket",
					 "Error"};

// enumerate the error codes
enum errorCodes {
	ERROR_NONE,
	ERROR_INVALID_CHARACTER,
	ERROR_INVALID_NUMBER,
	ERROR_INVALID_STRING,
	ERROR_INVALID_VARIABLE,
	ERROR_INVALID_FUNCTION,
	ERROR_UNBALANCED_PAREN,
	ERROR_UNBALANCED_BRACKET,
	ERROR_UNBALANCED_QUOTE,
	ERROR_OUT_OF_MEMORY,
	ERROR_STACK_UNDERFLOW,
	ERROR_UNKNOWN_OPERATOR,
	ERROR_SYNTAX
};

// enumerate the error messages
char *errorMessages[] = {"No error",
						 "Invalid character",
						 "Invalid number",
						 "Invalid string",
						 "Invalid variable",
						 "Invalid function",
						 "Unbalanced parenthesis",
						 "Unbalanced brackets",
						 "Unbalanced quotes",
						 "Out of memory",
						 "Stack underflow",
						 "Unknown operator",
						 "Syntax error"};

// return the error message for the given error code
char *getErrorMessage(int error) {
	return errorMessages[error];
}

// define the operators
char op_start[] = "+-*/^#<>=!";
char op_chars[] = "=>";

char unaries[] = "+-!";
char spaces[]  = " \t\r\n";

char num_chars[] = "0123456789.";
char var_start[] = "$";
char var_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";
char fun_start[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char fun_chars[] = "abcdefghijklmnopqrstuvwxyz";
char str_start[] = "'";
char dtr_start[] = "\"";

char l_paren[]	 = "(";
char r_paren[]	 = ")";
char l_bracket[] = "[";
char r_bracket[] = "]";
char comma[]	 = ",";

// integer to track the current position in the input string
int pExpr = 0;

// integer to track the previous token type
int pType = TOKEN_END;

// integers to track parentheses and brackets level
int pLevel = 0;
int bLevel = 0;

// stack to contain the tokens
typedef struct tokenStack {
	char *token;
	int type;
	struct tokenStack *next;
	struct tokenStack *prev;
} tokenStack;

// push a token onto the stack
int pushToken(tokenStack **stack, char *token, int type) {
	tokenStack *newNode;
	// allocate memory for the new node
	newNode = (tokenStack *)malloc(sizeof(tokenStack));
	if (newNode == NULL) {
		return ERROR_OUT_OF_MEMORY;
	}
	// allocate memory for the token
	newNode->token = (char *)malloc(strlen(token) + 1);
	if (newNode->token == NULL) {
		free(newNode);
		return ERROR_OUT_OF_MEMORY;
	}
	// copy the token to the new node
	strcpy(newNode->token, token);
	newNode->type = type;
	// push the new node onto the stack
	newNode->next = *stack;
	newNode->prev = NULL;
	if (*stack != NULL) {
		(*stack)->prev = newNode;
	}
	*stack = newNode;
	// return success
	return ERROR_NONE;
}

// pop a token from the stack
int popToken(tokenStack **stack, char *token, int* type) {
	tokenStack *nextNode;
	// if the stack is empty, return an error
	if (*stack == NULL) {
		return ERROR_STACK_UNDERFLOW;
	}
	// copy the token from the top of the stack
	strcpy(token, (*stack)->token);
	*type = (*stack)->type;
	// pop the top node off the stack
	nextNode = (*stack)->next;
	free((*stack)->token);
	free(*stack);
	*stack = nextNode;
	// return success
	return ERROR_NONE;
}

// peek at the top of the stack
int peekToken(tokenStack *stack, char *token) {
	// if the stack is empty, return an error
	if (stack == NULL) {
		return ERROR_STACK_UNDERFLOW;
	}
	// copy the token from the top of the stack
	strcpy(token, stack->token);
	// return success
	return ERROR_NONE;
}

// append at token at the end of the stack
int appendToken(tokenStack **stack, char *token, int type) {
	tokenStack *newNode;
	tokenStack *lastNode;
	// allocate memory for the new node
	newNode = (tokenStack *)malloc(sizeof(tokenStack));
	if (newNode == NULL) {
		return ERROR_OUT_OF_MEMORY;
	}
	// allocate memory for the token
	newNode->token = (char *)malloc(strlen(token) + 1);
	if (newNode->token == NULL) {
		free(newNode);
		return ERROR_OUT_OF_MEMORY;
	}
	// copy the token to the new node
	strcpy(newNode->token, token);
	newNode->type = type;
	// append the new node to the stack
	newNode->next = NULL;
	newNode->prev = NULL;
	if (*stack == NULL) {
		*stack = newNode;
	} else {
		lastNode = *stack;
		while (lastNode->next != NULL) {
			lastNode = lastNode->next;
		}
		lastNode->next = newNode;
		newNode->prev  = lastNode;
	}
	// return success
	return ERROR_NONE;
}

// free the memory of the stack
void freeStack(tokenStack *stack) {
	tokenStack *nextNode;
	while (stack != NULL) {
		nextNode = stack->next;
		free(stack->token);
		free(stack);
		stack = nextNode;
	}
}

// function to check if the previous token was an operator, left parenthesis, left bracket, comma or the beginning of the expression
// This is used for:
// - unary operators
int pTokenValid1() {
	return pType == TOKEN_OPERATOR || pType == TOKEN_L_PAREN || pType == TOKEN_L_BRACKET || pType == TOKEN_COMMA || pType == TOKEN_END;
}

// function to check if the previous token was an unary, an operator, left parenthesis, left bracket, comma or the beginning of the expression
// This is used for:
// - numbers
// - strings
// - variables
// - functions
int pTokenValid2() {
	return pType == TOKEN_UNARY || pType == TOKEN_OPERATOR || pType == TOKEN_L_PAREN || pType == TOKEN_L_BRACKET || pType == TOKEN_COMMA || pType == TOKEN_END;
}

// function to check if the previous token was an unary, an operator or a function
// This is used at the end of the expression
int pTokenValid3() {
	return pType == TOKEN_UNARY || pType == TOKEN_OPERATOR || pType == TOKEN_FUNCTION;
}

// function to check if the previous token was a number, a string, a variable, a function, a right parenthesis or a right bracket
// This is used for:
// - commas
int pTokenValid4() {
	return pType == TOKEN_NUMBER || pType == TOKEN_STRING || pType == TOKEN_VARIABLE || pType == TOKEN_FUNCTION || pType == TOKEN_R_PAREN || pType == TOKEN_R_BRACKET;
}

// function to check if the previous token was an unary, an operator, a function, a left parenthesis, left bracket, comma or the beginning of the expression
// This is used for:
// - left parenthesis
int pTokenValid5() {
	return pType == TOKEN_UNARY || pType == TOKEN_OPERATOR || pType == TOKEN_FUNCTION || pType == TOKEN_L_PAREN || pType == TOKEN_L_BRACKET || pType == TOKEN_COMMA || pType == TOKEN_END;
}

// function to check if the previous token was a number, a string, a variable, a right parenthesis or a right bracket
// This is used for:
// - operators
// - right parenthesis
// - right bracket
int pTokenValid6() {
	return pType == TOKEN_NUMBER || pType == TOKEN_STRING || pType == TOKEN_VARIABLE || pType == TOKEN_R_PAREN || pType == TOKEN_R_BRACKET;
}

// function to get the next token from the input string
int nextToken(char *expr, char *token, int *type) {
	int pToken = 0;
	int dCount = 0;
	token[0]   = '\0';
	*type	   = TOKEN_ERROR;
	char c	   = expr[pExpr];

	while (strchr(spaces, c) != NULL && c != '\0') {
		c = expr[++pExpr];
	}

	if (c == '\0') {
		if (pTokenValid3()) {
			return ERROR_INVALID_CHARACTER;
		}
		if (pLevel != 0) {
			return ERROR_UNBALANCED_PAREN;
		}
		if (bLevel != 0) {
			return ERROR_UNBALANCED_BRACKET;
		}
		*type = TOKEN_END;
		return ERROR_NONE;
	}

	if (strchr(unaries, c) != NULL && c != '\0') {
		if (pTokenValid1()) {
			token[pToken++] = c;
			token[pToken++] = 'u';
			pExpr++;
			token[pToken] = '\0';
			*type		  = TOKEN_UNARY;
			pType		  = TOKEN_UNARY;
			return ERROR_NONE;
		}
	}

	if (strchr(num_chars, c) != NULL) {
		if (!pTokenValid2()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		c				= expr[++pExpr];
		while (strchr(num_chars, c) != NULL && pToken < MAX_TOKEN_LENGTH && c != '\0') {
			if (c == '.') {
				dCount++;
			}
			if (dCount > 1) {
				return ERROR_INVALID_NUMBER;
			}
			token[pToken++] = c;
			c				= expr[++pExpr];
		}
		*type = TOKEN_NUMBER;
	} else if (strchr(str_start, c) != NULL) {
		if (!pTokenValid2()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		c				= expr[++pExpr];
		while (strchr(str_start, c) == NULL && pToken < MAX_TOKEN_LENGTH && c != '\0') {
			token[pToken++] = c;
			c				= expr[++pExpr];
		}
		if (c == '\0' || pToken >= MAX_TOKEN_LENGTH) {
			return ERROR_UNBALANCED_QUOTE;
		}
		token[pToken++] = c;
		c				= expr[++pExpr];
		*type			= TOKEN_STRING;
	} else if (strchr(dtr_start, c) != NULL) {
		if (!pTokenValid2()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		c				= expr[++pExpr];
		while (strchr(dtr_start, c) == NULL && pToken < MAX_TOKEN_LENGTH && c != '\0') {
			if (c == '\\') {
				c = expr[++pExpr];
			}
			token[pToken++] = c;
			c				= expr[++pExpr];
		}
		if (c == '\0' || pToken >= MAX_TOKEN_LENGTH) {
			return ERROR_UNBALANCED_QUOTE;
		}
		token[pToken++] = c;
		c				= expr[++pExpr];
		*type			= TOKEN_STRING;
	} else if (strchr(op_start, c) != NULL) {
		if (!pTokenValid6()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		c				= expr[++pExpr];
		while (strchr(op_chars, c) != NULL && pToken < MAX_TOKEN_LENGTH && c != '\0') {
			token[pToken++] = c;
			c				= expr[++pExpr];
		}
		*type = TOKEN_OPERATOR;
	} else if (strchr(var_start, c) != NULL) {
		if (!pTokenValid2()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		c				= expr[++pExpr];
		while (strchr(var_chars, c) != NULL && pToken < MAX_TOKEN_LENGTH && c != '\0') {
			token[pToken++] = c;
			c				= expr[++pExpr];
		}
		*type = TOKEN_VARIABLE;
	} else if (strchr(fun_start, c) != NULL) {
		if (!pTokenValid2()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		c				= expr[++pExpr];
		while (strchr(fun_chars, c) != NULL && pToken < MAX_TOKEN_LENGTH && c != '\0') {
			token[pToken++] = c;
			c				= expr[++pExpr];
		}
		*type = TOKEN_FUNCTION;
	} else if (strchr(comma, c) != NULL) {
		if (!pTokenValid4()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		pExpr++;
		*type = TOKEN_COMMA;
	} else if (strchr(l_paren, c) != NULL) {
		if (!pTokenValid5()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		pLevel++;
		pExpr++;
		*type = TOKEN_L_PAREN;
	} else if (strchr(r_paren, c) != NULL) {
		if (!pTokenValid6()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		pLevel--;
		if (pLevel < 0) {
			return ERROR_UNBALANCED_PAREN;
		}
		pExpr++;
		*type = TOKEN_R_PAREN;
	} else if (strchr(l_bracket, c) != NULL) {
		if (!pType == TOKEN_VARIABLE) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		bLevel++;
		pExpr++;
		*type = TOKEN_L_BRACKET;
	} else if (strchr(r_bracket, c) != NULL) {
		if (!pTokenValid6()) {
			return ERROR_INVALID_CHARACTER;
		}
		token[pToken++] = c;
		bLevel--;
		if (bLevel < 0) {
			return ERROR_UNBALANCED_BRACKET;
		}
		pExpr++;
		*type = TOKEN_R_BRACKET;
	} else {
		return ERROR_INVALID_CHARACTER;
	}

	token[pToken] = '\0';
	return ERROR_NONE;
}

// function to tokenize an expression
int tokenize(char *expr, tokenStack **tokens) {
	int type;
	int error;
	char token[MAX_TOKEN_LENGTH];
	// initialize the stack
	*tokens = NULL;
	// tokenize the expression
	while ((error = nextToken(expr, token, &type)) == ERROR_NONE && type != TOKEN_END) {
		// append the token to the stack
		error = appendToken(tokens, token, type);

		pType = type;
		if (error != ERROR_NONE) {
			return error;
		}
	}
}

#endif