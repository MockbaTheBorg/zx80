#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "postfix.h"
#include "eval.h"

// main program
// evaluate each expression and print the result
int main(int argc, char *argv[]) {
	// define a few unique test expressions
	char *exprs[] = {
		"234",
		"-49",
		"234.567",
		"2+3",
		"2^3",
		"2+3*4",
		"2+3*4^5",
		"(13 + 2) / 3",
		"\"abc\"",
		"'abd\\\"asra'"
//		"int(13 / 4)",
//		"int(13 / -(2+2)) + 1",
//		"int(13 / 4) + 1.0",
//		"$a[18].1",
//		"\"abc\\\"def\\\"ghi\"",
//		"$a>2",
//		"$a>=2",
//		"$a==int(13 / 4)",
//		"$a!=int(13 / 4)",
//		"$abc=2",
//		"2 + $a[17, int(3/-7)]"
    };

	// local variables
	int i;
	int error;

	// create a linked list for result tokens
	tokenStack *tokens = NULL;

	// evaluate the expressions
	for (i = 0; i < sizeof(exprs) / sizeof(char *); i++) {
		pExpr  = 0;
		pType  = TOKEN_END;
		pLevel = 0;
		bLevel = 0;
		printf("%s = ", exprs[i]);
		error = eval(exprs[i], &tokens);
		if (error != ERROR_NONE) {
			printf("Error: %s at %d\n", errorMessages[error], pExpr);
			for (int j = 0; j < pExpr; j++) {
				printf(" ");
			}
			printf("^\n");
		} else {
			printf("[");
			while (tokens != NULL) {
				printf("%s", tokens->token);
				tokens = tokens->next;
				if (tokens != NULL) {
					printf(" ");
				}
			}
			printf("]\n");
		}
	}
	return 0;
}

// q: how much is 2+3*4^5?
// a: 2 3 4 5 ^ * + = 258