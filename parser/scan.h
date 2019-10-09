/* Definitions the scanner shares with the parser
    Michael L. Scott, 2008-2019.
*/

typedef enum {t_read, t_write, t_id, t_intconst, t_realconst, t_gets,
	      t_add, t_sub, t_mul, t_div, t_lparen, t_rparen, t_eof, t_if, t_while, t_end, t_eq, t_neq, t_great, t_less, t_greateq, t_lesseq, t_int, t_real, t_float, t_trunc} token;

typedef enum {s_p, s_sl, s_s, s_c, s_e, s_t, s_f, s_tt, s_ft, s_ro, s_ao, s_mo, s_d, symbol_count} symbol; 

#define MAX_TOKEN_LEN 100
extern char token_image[MAX_TOKEN_LEN];

extern token scan();
