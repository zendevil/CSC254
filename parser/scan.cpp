/* Simple ad-hoc scanner for the calculator language.
    Michael L. Scott, 2008-2019.
*/

#include "scan.h"
#include <iostream>
#include <vector>
char token_image[MAX_TOKEN_LEN];
token scan() {
    static int c = ' ';
        /* next available char; extra (int) width accommodates EOF */
    int i = 0;              /* index into token_image */

    /* skip white space */
    while (isspace(c)) {
        c = getchar();
    }
    if (c == EOF)
        return t_eof;
    if (isalpha(c)) {
        do {
            token_image[i++] = c;
            if (i >= MAX_TOKEN_LEN) {
	      std::cout<<"max token length exceeded"<<std::endl;
                exit(1);
            }
            c = getchar();
        } while (isalpha(c) || isdigit(c) || c == '_');
        token_image[i] = '\0';
        if (!strcmp(token_image, "read")) return t_read;
        else if (!strcmp(token_image, "write")) return t_write;
        else if (!strcmp(token_image, "if")) return t_if;
        else if (!strcmp(token_image, "end")) return t_end;
        else if (!strcmp(token_image, "while")) return t_while;
	else if (!strcmp(token_image, "int")) return t_int;
	else if (!strcmp(token_image, "real")) return t_real;
	else if (!strcmp(token_image, "float")) return t_float;
	else if (!strcmp(token_image, "trunc")) return t_trunc;
	else return t_id;
    }
    else if (isdigit(c)) {
      bool is_real = false;
        do {
            token_image[i++] = c;
            c = getchar();
        } while (isdigit(c));
	
	if (c == '.') {
	  is_real = true;
	  do {
	    token_image[i++] = c;
	    c = getchar();
	  } while (isdigit(c));
	}
	
        token_image[i] = '\0';
	
	if (is_real) {
	  return t_realconst;
	}
        return t_intconst;
    } else switch (c) {
        case ':':
            if ((c = getchar()) != '=') {
	      std::cerr<<stderr<<" error"<<std::endl;
                exit(1);
            } else {
                c = getchar();
                return t_gets;
            }
            break;
        case '+': c = getchar(); return t_add;
        case '-': c = getchar(); return t_sub;
        case '*': c = getchar(); return t_mul;
        case '/': c = getchar(); return t_div;
        case '(': c = getchar(); return t_lparen;
        case ')': c = getchar(); return t_rparen;
	  
      case '=':
	if ((c = getchar()) != '=') {
	  std::cerr<<stderr<<" error"<<std::endl;
	  exit(1);
	} else {
	  c = getchar();
	  return t_eq;
	}
	break;
    case '!':
	if ((c = getchar()) != '=') {
	  std::cerr<<stderr<<" error"<<std::endl;
	  exit(1);
	} else {
	  c = getchar();
	  return t_neq;
	}
	break;

	  case '<':
	if ((c = getchar()) != '=') {
	  return t_less;
	} else {
	  c = getchar();
	  return t_lesseq;
	}
	break;

	  case '>':
	if ((c = getchar()) != '=') {
	  return t_great;
	} else {
	  c = getchar();
	  return t_greateq;
	}
	break;

	  case '$':
	if ((c = getchar()) != '$') {
	  std::cerr<<stderr<<" error"<<std::endl;
	  exit(1);
	} else {
	  c = getchar();
	  return t_eof;
	}
	break;	
        default:
	  std::cout<<"error"<<std::endl;
            exit(1);
    }
}
