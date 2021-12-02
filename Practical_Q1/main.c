/* Originally based on "front.c". */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Global declarations */
/* Variables */
int charClass;
char lexeme[100];
char keywords[13][13] = {"for", "if", "else", "while", "do", "int", "float", "switch", "foreach", "in", "return", "VOID", "MAIN"}; 
char nextChar;
int lexLen;
int token;
int nextToken;
int doWhile_inProcess = 0;
FILE *in_fp;

/* Function declarations */
int lookup(char ch);
void addChar(void);
void getChar(void);
void getNonBlank(void);
int lex(void);
void statement();
void expr(void);
void term(void);
void factor(void);
void switch_statement();
void forEach_statement();
void for_statement();
void while_statement();
void doWhile_statement();
void block();
void if_statement();
void assignment_statement();
void return_statement();
void program();
void error(void);

/* Character classes */
#define LETTER 0 
#define DIGIT 1 
#define SINGLEQUOTE 2
#define DOUBLEQUOTE 3
#define DECIMAL 4 
#define UNDERSCORE 5
#define UNKNOWN 99 

/* Token codes */
#define FLOAT_LIT 9
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 19
#define ADD_OP 20
#define SUB_OP 21
#define MULT_OP 22
#define DIV_OP 23
#define MOD_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define DECIMAL_POINT 27
#define LEFT_BRACK 28
#define RIGHT_BRACK 29
#define SEMICOLON 30

/* Token codes (Mostly statement-related) */
#define FOR_CODE 30
#define IF_CODE 31
#define ELSE_CODE 32
#define WHILE_CODE 33
#define DO_CODE 34
#define INT_CODE 35
#define FLOAT_CODE 36
#define SWITCH_CODE 37
#define FOREACH_CODE 38
#define IN_CODE 39
#define RETURN_CODE 40
#define VOID_CODE 41
#define MAIN_CODE 42

/******************************************************/
/* main driver */
int main(void) {
	if ((in_fp = fopen("front.in", "r")) == NULL) {
	  printf("ERROR - cannot open front.in \n");
  }
	else {
	 	getChar();
    do {
      lex();
    } while (nextToken != EOF);
	}

  printf("=========================================\n");
	return 0;
}

/******************************************************/
/* lookup - a function to look up operators and
 parentheses and return the token */
int lookup(char ch) {
	switch (ch) {
		case '(':
			addChar();
			nextToken = LEFT_PAREN;
			break;

		case ')':
			addChar();
			nextToken = RIGHT_PAREN;
			break;

    case '{':
			addChar();
			nextToken = LEFT_BRACK;
			break;

		case '}':
			addChar();
			nextToken = RIGHT_BRACK;
			break;

		case '+':
			addChar();
			nextToken = ADD_OP;
			break;

    case '-':
      addChar();
      nextToken = SUB_OP;
      break;

    case '*':
      addChar();
      nextToken = MULT_OP;
      break;

    case '/':
      addChar();
      nextToken = DIV_OP;
      break;

    case '%':
      addChar();
      nextToken = MOD_OP;
      break;

    case '=':
      addChar();
      nextToken = ASSIGN_OP;
      break;

    case '.':
      addChar();
      nextToken = DECIMAL_POINT;
      break;

    case ';':
      addChar();
      nextToken = SEMICOLON;
      break;

    default:
      addChar();
      nextToken = EOF;
      break;
  }
  return nextToken;
}

/******************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar(void) {
	if (lexLen <= 98) {
		lexeme[lexLen++] = nextChar;
		lexeme[lexLen] = '\0';
	} 
  else
	  printf("Error - lexeme is too long \n");
}

/******************************************************/
/* getChar - a function to get the next character of
 input and determine its character class */
void getChar(void) {
  nextChar = getc(in_fp);
  if (nextChar != EOF) {
    if (isalpha(nextChar)) {
      charClass = LETTER;
    }
    else if (isdigit(nextChar)) {
      charClass = DIGIT;
    }
    else if (nextChar == '_') {
      charClass = UNDERSCORE;
    }
    else {
      charClass = UNKNOWN;
    }
  } 
  else
    charClass = EOF;
}

/******************************************************/
/* getNonBlank - a function to call getChar until it
 returns a non-whitespace character */
void getNonBlank(void) {
	while (isspace(nextChar))
	  getChar();
}

/******************************************************/
/* lex - A simple lexical analyzer */
int lex(void) {
  lexLen = 0;
  getNonBlank();
  switch (charClass) {
    case LETTER:
      addChar();
      getChar();
      while (charClass == LETTER || charClass == DIGIT || charClass == UNDERSCORE) {
        addChar();
        getChar();
      }

      // FOR_CODE:
      if (strcmp(lexeme, keywords[0]) == 0) {
        nextToken = FOR_CODE;
        for_statement();
        break;
      }

      // IF_CODE:
      else if (strcmp(lexeme, keywords[1]) == 0) {
        nextToken = IF_CODE;
        if_statement();
        break;
      }

      // ELSE_CODE:
      else if (strcmp(lexeme, keywords[2]) == 0) {
        nextToken = ELSE_CODE;
        break;
      }

      // WHILE_CODE:
      else if (strcmp(lexeme, keywords[3]) == 0) {
        nextToken = WHILE_CODE;
        if(doWhile_inProcess != 0) {
          while_statement();
        }
        break;
      }

      // DO_CODE:
      else if (strcmp(lexeme, keywords[4]) == 0) {
        nextToken = DO_CODE;
        doWhile_statement();
        break;
      }

      // INT_CODE:
      else if (strcmp(lexeme, keywords[5]) == 0) {
        nextToken = INT_CODE;
        break;
      }

      // FLOAT_CODE:
      else if (strcmp(lexeme, keywords[6]) == 0) {
        nextToken = FLOAT_CODE;
        break;
      }

      // SWITCH_CODE:
      else if (strcmp(lexeme, keywords[7]) == 0) {
        nextToken = SWITCH_CODE;
        switch_statement();
        break;
      }

      // FOREACH_CODE:
      else if (strcmp(lexeme, keywords[8]) == 0) {
        nextToken = FOREACH_CODE;
        forEach_statement();
        break;
      }

      // IN_CODE:
      else if (strcmp(lexeme, keywords[9]) == 0) {
        nextToken = IN_CODE;
        break;
      }

      // RETURN_CODE:
      else if (strcmp(lexeme, keywords[10]) == 0) {
        nextToken = RETURN_CODE;
        return_statement();
        break;
      }

      // VOID_CODE:
      else if (strcmp(lexeme, keywords[11]) == 0) {
        nextToken = VOID_CODE;
        program();
        break;
      }

      // MAIN_CODE:
      else if (strcmp(lexeme, keywords[12]) == 0) {
        nextToken = MAIN_CODE;
        break;
      }

      // User-Defined Identifiers
      else {
        nextToken = IDENT;
        break;
      }
      
    case UNDERSCORE:
      // User-Defined Identifiers
      addChar();
      getChar();
      while (charClass == LETTER || charClass == DIGIT || charClass == UNDERSCORE) {
        addChar();
        getChar();
      }
      nextToken = IDENT;
      break;

    /* Integer & Float literals */
    case DIGIT:
      addChar();
      getChar();
      while (charClass == DIGIT) {
        addChar();
        getChar();
      }

      if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
        addChar();
        getChar();
        nextToken = FLOAT_LIT;
        break;
      }

      if(nextChar == 'E' || nextChar == 'e') {
        addChar();
        getChar();
        if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
          addChar();
          getChar();
          nextToken = FLOAT_LIT;
          break;
        }
        if(nextChar == '-'){
          addChar();
          getChar();
          while(charClass == DIGIT){
            addChar();
            getChar();
            if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
              addChar();
              getChar();
              nextToken = FLOAT_LIT;
              break;
            }
          }
          nextToken = FLOAT_LIT;
          break;
        } 
        else if(charClass == DIGIT){
          while(charClass == DIGIT){
            addChar();
            getChar();
            if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
              addChar();
              getChar();
              nextToken = FLOAT_LIT;
              break;
            }
          }
          nextToken = FLOAT_LIT;
          break;
        }
      }
      
      if (charClass == UNKNOWN && nextChar == '.'){
        addChar();
        getChar();
        while (charClass == DIGIT) {
          addChar();
          getChar();
          if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
            addChar();
            getChar();
            nextToken = FLOAT_LIT;
            break;
          }
          if(nextChar == 'E' || nextChar == 'e'){
            addChar();
            getChar();
            if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
              addChar();
              getChar();
              nextToken = FLOAT_LIT;
              break;
            }
            if(nextChar == '-'){
              addChar();
              getChar();
              while(charClass == DIGIT){
                addChar();
                getChar();
                if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
                  addChar();
                  getChar();
                  nextToken = FLOAT_LIT;
                  break;
                }
              }
              nextToken = FLOAT_LIT;
              break;
            } 
            else if(charClass == DIGIT){
              while(charClass == DIGIT){
                addChar();
                getChar();
                if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
                  addChar();
                  getChar();
                  nextToken = FLOAT_LIT;
                  break;
                }
              }
              nextToken = FLOAT_LIT;
              break;
            }
          }
        }
        nextToken = FLOAT_LIT;
      } 
      else {
        nextToken = INT_LIT;
      }
      break;
      
    case UNKNOWN:
      if (nextChar == '.'){
        addChar();
        getChar();
        if(charClass == DIGIT){
          addChar();
          getChar();
          while(charClass == DIGIT){
            addChar();
            getChar();
            if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
              addChar();
              getChar();
              nextToken = FLOAT_LIT;
              break;
            }
            else if(nextChar == 'E' || nextChar == 'e'){
              addChar();
              getChar();
              if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
                addChar();
                getChar();
                nextToken = FLOAT_LIT;
                break;
              }
              if(nextChar == '-'){
                addChar();
                getChar();
                while(charClass == DIGIT){
                  addChar();
                  getChar();
                  if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
                    addChar();
                    getChar();
                    nextToken = FLOAT_LIT;
                    break;
                  }
                }
                nextToken = FLOAT_LIT;
                break;
              } 
              else if(charClass == DIGIT){
                while(charClass == DIGIT){
                  addChar();
                  getChar();
                  if (nextChar == 'f' || nextChar == 'F' || nextChar == 'l' || nextChar == 'L'){
                    addChar();
                    getChar();
                    nextToken = FLOAT_LIT;
                    break;
                  }
                }
                nextToken = FLOAT_LIT;
                break;
              }
            }
            nextToken = FLOAT_LIT;
          }
        } 
        else {
          nextToken = DECIMAL_POINT;
        } 
      } 
      else {
        lookup(nextChar);
        getChar();
      }
      break;
  
    /* EOF */
    case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = '\0';
        break;
  } 
  /* End of switch */
  printf("Next token is: %d, Next lexeme is %s\n",
  nextToken, lexeme);
  return nextToken;
  /* End of function lex */
}

/******************************************************/
/* Identifies the type of statement that can be used. */
void statement() {
  switch (nextToken) {
    case SWITCH_CODE:
      switch_statement();
      break;
    case FOREACH_CODE:
      forEach_statement();
      break;
    case FOR_CODE:
      for_statement();
      break;
    case WHILE_CODE:
      while_statement();
      break;
    case DO_CODE:
      doWhile_statement();
      break;
    case IF_CODE:
      if_statement();
      break;
    case IDENT:
      assignment_statement();
      break;
    case RETURN_CODE:
      return_statement();
      break;
    default:
      error();
  }
}

/* Made up of terms, which themselves are made of factors. */
void expr(void) {
	printf("Enter <expr>\n");
	/* Parse the first term */
	term();
	/* As long as the next token is + or -, get
	the next token and parse the next term. */
  while (nextToken == ADD_OP || nextToken == SUB_OP) {
    lex();
    term();
  }
  printf("Exit <expr>\n");
}

/* term
Parses strings in the language generated by the rule:
<term> -> <factor> {(* | / | % ) <factor>}
*/
void term(void) {
	printf("Enter <term>\n");
	/* Parse the first factor. */
	factor();
	/* As long as the next token is *, /, or %, get the
	 next token and parse the next factor. */
  while (nextToken == MULT_OP || nextToken == DIV_OP || nextToken == MOD_OP) {
    lex();
    factor();
  }
  printf("Exit <term>\n");
}

/* factor
Parses strings in the language generated by the rule:
<factor> -> id | int_constant | ( <expr> )
*/
void factor(void) {
	printf("Enter <factor>\n");
	/* Determine which RHS. */
  if (nextToken == IDENT || nextToken == INT_LIT) {
    /* Get the next token. */
    lex();
  }
  /* If the RHS is ( <expr> ), call lex to pass over the
    left parenthesis, call expr, and check for the right
    parenthesis. */
  else if (nextToken == LEFT_PAREN) {
    lex();
    expr();
    if (nextToken == RIGHT_PAREN) {
      lex();
    }
    else {
      error();
    }
  }
  /* It was not an id, an integer literal, or a left
    parenthesis. */
  else {
    error();
  }
  printf("Exit <factor>\n");
}

/* Syntax of switch statements. C-based. */
void switch_statement() {
  if (nextToken != SWITCH_CODE) {
    error();
  }
  else {
    printf("Next token is: %d, Next lexeme is %s\n",
    nextToken, lexeme);
    lex();
    if (nextToken != LEFT_PAREN) {
      error();
    }
    else {
      lex();
      expr();
      if (nextToken != RIGHT_PAREN) {
        error();
      }
      else {
        lex();
        block();
      }
    }
  }
}

/* Syntax of for-each statements. Mostly Python-based. */
void forEach_statement() {
  if (nextToken != FOREACH_CODE) {
    error();
  }
  else {
    printf("Next token is: %d, Next lexeme is %s\n",
    nextToken, lexeme);
    lex();
    if (nextToken != LEFT_PAREN) {
      error();
    }
    else {
      lex();
      if(nextToken != IDENT) {
        error();
      }
      else {
        lex();
        if(nextToken != IN_CODE) {
          error();
        }
        else {
          lex();
          if(nextToken != IDENT) {
            error();
          }
          else {
            lex();
            if (nextToken != RIGHT_PAREN) {
              error();
            }
            else {
              lex();
              block();
            }
          }
        }
      }
    }
  }
}

/* Syntax of for-loop statements. C-based. */
void for_statement() {
  if (nextToken != FOR_CODE) {
    error();
  }
  else {
    printf("Next token is: %d, Next lexeme is %s\n",
    nextToken, lexeme);
    lex();
    if (nextToken != LEFT_PAREN) {
      error();
    }
    else {
      lex();
      expr();
      if (nextToken != SEMICOLON) {
        error();
      }
      else {
        lex();
        expr();
        if (nextToken != SEMICOLON) {
          error();
        }
        else {
          lex();
          expr();
          if (nextToken != RIGHT_PAREN) {
            error();
          }
          else {
            lex();
            block();
          }
        }
      }
    }
  }
}

/* Syntax of while-loop statements. C-based. */
void while_statement() {
  if (nextToken != WHILE_CODE) {
    error();
  }
  else {
    printf("Next token is: %d, Next lexeme is %s\n",
    nextToken, lexeme);
    lex();
    if (nextToken != LEFT_PAREN) {
      error();
    }
    else {
      lex();
      expr();
      if (nextToken != RIGHT_PAREN) {
        error();
      }
      else {
        lex();
        block();
      }
    }
  }
}

/* Syntax of do-while statements. C-based. */
void doWhile_statement() {
  if (nextToken != DO_CODE) {
    error();
  }
  else {
    printf("Next token is: %d, Next lexeme is %s\n",
    nextToken, lexeme);
    lex();
    doWhile_inProcess = 0;
    block();
    if (nextToken != WHILE_CODE) {
      error();
    }
    else {
      lex();
      if (nextToken != LEFT_PAREN) {
        error();
      }
      else {
        lex();
        expr();
        if (nextToken != RIGHT_PAREN) {
          error();
        }
        else {
          lex();
          doWhile_inProcess = 1;
        }
      }
    }
  }
}

/* Syntax of block statements. Form was directly provided in Assessment #2. */
void block() {
  if (nextToken != LEFT_BRACK) {
    error();
  }
  else {
    lex();
    while (nextToken != RIGHT_BRACK) {
      statement();
      if(nextToken != SEMICOLON) {
        error();
      }
      else {
        lex();
      }
    }
    if(nextToken != RIGHT_BRACK) {
      error();
    }
    else {
      lex();
    }
  }
}

/* Syntax of if-else statements. C-based. */
void if_statement() {
  if (nextToken != IF_CODE) {
    error();
  }
  else {
    printf("Next token is: %d, Next lexeme is %s\n",
    nextToken, lexeme);
    lex();
    if (nextToken != LEFT_PAREN) {
      error();
    }
    else {
      lex();
      expr();
      if (nextToken != RIGHT_PAREN) {
        error();
      }
      else {
        lex();
        block();
        if (nextToken == ELSE_CODE) {
          lex();
          block();
        }
      }
    }
  }
}

/* Syntax of assignment statements. C-based. */
void assignment_statement() {
  if (nextToken != IDENT) {
    error();
  }
  else {
    lex();
    if (nextToken != ASSIGN_OP) {
      error();
    }
    else {
      lex();
      expr();
    }
  }
}

/* Syntax of return statements. C-based. */
void return_statement() {
  if (nextToken != RETURN_CODE) {
    error();
  }
  else {
    printf("Next token is: %d, Next lexeme is %s\n",
    nextToken, lexeme);
    lex();
    if (nextToken != EOF) {
      expr();
    }
  }
}

/* Syntax of general programs. Form was directly provided in Assessment #2. */
void program() {
  if (nextToken != VOID_CODE) {
    error();
  }
  else {
    printf("Next token is: %d, Next lexeme is %s\n",
    nextToken, lexeme);
    lex();
    if (nextToken != MAIN_CODE) {
      error();
    }
    else {
      lex();
      if (nextToken != LEFT_PAREN) {
        error();
      }
      else {
        lex();
        if (nextToken != RIGHT_PAREN) {
          error();
        }
        else {
          lex();
          block();
        }
      }
    }
  }
}

void error(void) {
 exit(1);
}
