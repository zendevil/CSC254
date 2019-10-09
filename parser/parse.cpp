/* CompletOAe reAcursive descent parser for the calculator language.
   Builds on figure 2.16.  Prints a trace of productions predicted and
   tokens matched.  Does no error recovery: prints "syntax error" and
   dies on invalid input.
   Michael L. Scott, 2008-2019.
*/

#include <iostream>
#include<set>
#include "scan.h"
#include<vector>
#include <algorithm>



class List {
public:
  bool has_val;
  bool const_len;
  List* begin;
  List* end;
  List* next;
  List* prev;
  std::string val;

  List(bool create_sublist, bool const_len) {
    if(create_sublist) {
      begin = new List(false, true);
      end = new List(false, true);
      begin->next = end;
      end->prev = begin;
    }
    this->const_len = const_len;
    has_val = false;
  }
  
  List(std::string val, bool const_len) {
    this->val = val;
    this->const_len = const_len;
    has_val = true;
  }

  List (List* l) {
    begin = l->begin;
    end = l->end;
    const_len = l->const_len;
    has_val = l->has_val;
    val = l->val;
  }

  void append(std::string elem) {
    List* n = new List(elem, true);
    end->prev->next = n;
    n->next = end;
    n->prev = end->prev;
    end->prev = n;    
  }

  void append(List* l) {
    List* t = new List(l);
    end->prev->next = t;
    t->next = end;
    t->prev = end->prev;
    end->prev = t; 
  }

  bool is_op() {
    if (val == "*" || val == "/" || val == "==" || val == "!=" || val == ">=" || val == "<=" || val == "!=" || val == "+" || val == "-" || val == ">") {
      return true;
    } 
    return false;
  }

  void print_tree() {
    if (begin->next != end) {
      
      if (!begin->next->is_op() && begin->next->next != end) {
	if (const_len) {
	  std::cout<<"(";
	} else {
	  std::cout<<std::endl;
	std::cout <<"[";
	}

      }
      
      //std::cout<<"testing print tree"<<std::endl;
      if (begin == NULL) {
	std::cerr<<"Begin is null"<<std::endl;
      }
    
      //std::cout<<"testing print tree after"<<std::endl;
      List* curr = begin->next;
      //std::cout<<"testing val "<<curr->val<<std::endl;
      while(curr != end) {
	if (!curr->has_val) {
	  //std::cout<<"printing tree"<<std::endl;
	  curr->print_tree();
	} else {
	  //std::cout<<"printing val"<<std::endl;
	  std::cout<<curr->val<<" ";
	}
	curr = curr->next;     
      }
    
      if (!begin->next->is_op() && begin->next->next != end) {

	if (const_len) {
	  std::cout<<")";
      } else {
	  std::cout <<"]";
	}
      }
      
    }
  }
  
};

const char* names[] = {"read", "write", "int_id", "real_id", "int_const", "real_const", "gets",
                       "add", "sub", "mul", "div", "lparen", "rparen", "eof", "if", "while", "end", "eq", "neq", "great", "less", "greateq", "lesseq", "int", "real", "float", "trunc"};

static token input_token;

std::vector<std::string> int_decl;
std::vector<std::string> real_decl;

std::set<token>* first[symbol_count];
std::set<token>* follow[symbol_count];
bool b_eps[symbol_count];
std::set<token> starter_set;
std::set<token>* set_union(std::set<token>& set1, std::set<token>& set2) {
  std::set<token>* v = new std::set<token>();
  std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(*v, v->begin()));
  return v;
} 

void first_follow() {
  std::set<token>* first_s = new std::set<token>();
  first_s->insert(t_id); first_s->insert(t_read); first_s->insert(t_write); first_s->insert(t_if); first_s->insert(t_while);
  std::set<token>* follow_sl = new std::set<token>();
  follow_sl->insert(t_end); follow_sl->insert(t_eof);
  std::set<token>* first_ro = new std::set<token>();
  first_ro->insert(t_eq); first_ro->insert(t_neq); first_ro->insert(t_less); first_ro->insert(t_great); first_ro->insert(t_lesseq); first_ro->insert(t_greateq);
  std::set<token>* first_ao = new std::set<token>();
  first_ao->insert(t_add); first_ao->insert(t_sub);
  std::set<token>* first_mo = new std::set<token>(); 
  first_mo->insert(t_mul); first_mo->insert(t_div);
  std::set<token>* first_f = new std::set<token>();
  first_f->insert(t_lparen); first_f->insert(t_id); first_f->insert(t_intconst);first_f->insert(t_realconst);
  std::set<token>* first_d = new std::set<token>();
  first_d->insert(t_int); first_d->insert(t_real);
   
  first[s_s] = first_s;
  follow[s_sl] = follow_sl;
  first[s_ro] = first_ro;
  first[s_ao] = first_ao;
  first[s_mo] = first_mo;
  first[s_sl] = first[s_s];
  first[s_sl]->insert(t_int);
  first[s_sl]->insert(t_real);
  first[s_p] = first[s_sl];
  follow[s_p] = new std::set<token>(); // empty set
  follow[s_s] = first[s_sl];
  follow[s_s]->insert(t_end);
  follow[s_s]->insert(t_eof);
  first[s_f] = first_f; 
  first[s_t] = first[s_f];
  first[s_e] = first[s_t];
  first[s_c] = first[s_e];
  follow[s_c] = first[s_sl];
  follow[s_e] = set_union(*(first[s_ro]), *(follow[s_s]));
  follow[s_e]->insert(t_rparen);
  first[s_tt] = first[s_ao];
  follow[s_t] = set_union(*first[s_tt], *follow[s_e]);
  follow[s_t]->insert(t_eof);
  first[s_ft] = first[s_mo];
  follow[s_f] = set_union(*first[s_ft], *follow[s_t]);
  follow[s_tt] = follow[s_e];
  
  follow[s_ft] = follow[s_t];
  follow[s_ro] = first[s_e];
  follow[s_ao] = first[s_t];
  follow[s_mo] = first[s_f];

  first[s_d] = first_d;
  follow[s_d] = set_union(*first[s_sl], *follow[s_sl]);


}

void eps() {
    for(int s = 0; s < symbol_count; s++) {
    b_eps[s] = false;
  }
  b_eps[s_sl] = true;
  b_eps[s_tt] = true;
  b_eps[s_ft] = true;

}

void starter_set_init() {
  starter_set.insert(t_while);
  starter_set.insert(t_if);
  starter_set.insert(t_lparen);
}
bool contains(token t, std::set<token> set) {
  return set.find(t) != set.end();
}

void delete_token() {
  std::cout<<"Token deleted "<<input_token<<std::endl;
  input_token = scan();
}

void error () {
  std::cout<<"syntax error "<<token_image<<std::endl;
  //exit (1);
}

void print_set(std::set<token> s) {
  std::set<token>::iterator itr;
  for (itr = s.begin(); itr != s.end(); ++itr) { 
    std::cout << '\t' << *itr; 
  }
  std::cout<<std::endl;
}

void check_for_error(symbol s, std::set<token> follow_set) {

  if (!(contains(input_token, *first[s]) || (b_eps[s] && (contains(input_token, follow_set))))) {
    //std::cout<<"input_token "<<input_token<<std::endl;
    error ();
    while(!(contains(input_token, *first[s]) || contains(input_token, follow_set) || contains(input_token, starter_set) || input_token == t_eof)) {
      delete_token();
    }
  }
}


std::string match (token expected) {
  std::string curr_token_image = (input_token == t_intconst || input_token == t_realconst || input_token == t_id) ? "\""+(std::string)token_image+"\"": token_image;
  if (input_token == expected) {
    //std::cout<<"matched "<<names[input_token];
    //if (input_token == t_id || input_token == t_intconst || input_token == t_realconst)
    //std::cout<<": "<<token_image;
    
    // std::cout<<std::endl;
    input_token = scan ();
  }
  else error ();
  //std::cout<<"Returning token image "<<token_image<<std::endl;
    return curr_token_image;
}

List* program (std::set<token> follow_set);
List* stmt_list (std::set<token> follow_set);
List* decl (std::set<token> follow_set);
List* stmt (std::set<token> follow_set);
List* cond(std::set<token> follow_set);
List* expr (std::set<token> follow_set);
List* term_tail (std::set<token> follow_set);
List* term (std::set<token> follow_set);
List* factor_tail (std::set<token> follow_set);
List* factor (std::set<token> follow_set);
List* r_op(std::set<token> follow_set);
List* add_op (std::set<token> follow_set);
List* mul_op (std::set<token> follow_set);

List* program (std::set<token> follow_set) {
  check_for_error(s_p, follow_set);
  List* tree = new List(true, true);
  tree->append("program");
  switch (input_token) {
  case t_id:
  case t_read:
  case t_write:
  case t_eof:
  case t_if:
  case t_while:
  case t_int:
  case t_real: {
    //std::cout<<"predict program --> stmt_list eof"<<std::endl;
    std::set<token> follow;
    follow.insert(t_eof);
    tree->append(stmt_list (follow));
    match (t_eof);
  } break;
    default: std::cout<<"error at program"<<std::endl;

  }
  //std::cout<<"printing program tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

List* stmt_list (std::set<token> follow_set) {
  check_for_error(s_sl, follow_set);
  List* tree = new List(true, false);
  std::set<token> f;
    f.insert(t_end);
  switch (input_token) {
  case t_id:
  case t_read:
  case t_write:
  case t_if:
  case t_while: 
    //std::cout<<"predict stmt_list --> stmt stmt_list"<<std::endl;
    tree->append(stmt (*first[s_sl]));
    tree->append(stmt_list (f));
    break;
  case t_int:
  case t_real:
    tree->append(decl(*follow[s_d]));
    tree->append(stmt_list(f));
    break;
  case t_eof:
  case t_end:
    // std::cout<<"predict stmt_list --> epsilon"<<std::endl;
    break;          /*  epsilon production */
    default: std::cout<<"stmt list error"<<std::endl;
  }
  //std::cout<<"printing stmt_list tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

List* decl(std::set<token> follow_set) {
  check_for_error(s_d, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_int:
    //std::cout<<"predict decl --> int id"<<std::endl;
    match(t_int);
    // if token is not in real or int decl lists. 
    if (std::find(int_decl.begin(), int_decl.end(), token_image) == int_decl.end() &&std::find(real_decl.begin(), real_decl.end(), token_image) == real_decl.end()) {
      tree->append("int");
      int_decl.push_back(token_image);
      tree->append(match (t_id));
    } else {
      std::cerr<<"Redeclaration of "<<token_image<<std::endl;
    }
    break;
  case t_real:
    //std::cout<<"predict decl --> real id"<<std::endl;
     match(t_real);
    if (std::find(int_decl.begin(), int_decl.end(), token_image) == int_decl.end() &&std::find(real_decl.begin(), real_decl.end(), token_image) == real_decl.end()) {
      tree->append("real");
      real_decl.push_back(token_image);
      tree->append(match (t_id));
    } else {
      std::cerr<<"Redeclaration of "<<token_image<<std::endl;
    }  
    break;
  default: std::cerr<<"decl error"<<std::endl;
  }
  return tree;
}

List* stmt (std::set<token> follow_set) {
  check_for_error(s_s, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_id:
    //std::cout<<"predict stmt --> id gets expr"<<std::endl;
    tree->append(":=");
    tree->append(match (t_id));
    match (t_gets);
    tree->append(expr (*first[s_sl]));
    break;
  case t_read:
    //std::cout<<"predict stmt --> read id"<<std::endl;
    tree->append(match (t_read));
    //std::cout<<"printing tree after adding literal"<<std::endl;
    //tree->print_tree();
    tree->append(match (t_id));
    break;
  case t_write:
    //std::cout<<"predict stmt --> write expr"<<std::endl;
    tree->append(match (t_write));
    tree->append(expr (*first[s_sl]));
    break;
  case t_if: {
    //std::cout<<"predict stmt --> if cond stmt_list end"<<std::endl;
    tree->append(match (t_if));
    tree->append(cond(*first[s_sl]));
    std::set<token> follow;
    follow.insert(t_end);
    tree->append(stmt_list(follow));
    match(t_end);
  }
    break;
	    
  case t_while: {
    //std::cout<<"predict stmt --> while cond stmt_list end"<<std::endl;
    tree->append(match (t_while));
    tree->append(cond(*first[s_sl]));
    std::set<token> follow;
    follow.insert(t_end);
    tree->append(stmt_list(follow));
    match(t_end);
    } break;
  default:  std::cout<<"error at stmt"<<std::endl;
    //error ();
    // return;
  }
  //std::cout<<"printing stmt tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

List* cond (std::set<token> follow_set) {
  check_for_error(s_c, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_lparen:
  case t_id:
  case t_intconst:
  case t_realconst: {
    //std::cout<<"predict cond --> expr ro expr"<<std::endl;
    List* t = expr(*first[s_ro]); 
    tree->append(r_op(*follow[s_ro]));
    tree->append(t);
    tree->append(expr(*first[s_sl]));
  } break;
  default: std::cout<<"Error at cond"<<std::endl;
  }
  //std::cout<<"printing cond tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

// first element of tail contains the operator that we put at
// the beginning of the new list
List* combine(List* head, List* tail) { 
  //std::cout<<"print head"<<std::endl;
  //head->print_tree();
  //std::cout<<std::endl;
  //std::cout<<"print tail"<<std::endl;
  //tail->print_tree();
  //std::cout<<std::endl;
  List* n = new List(true, true);
  bool tail_epsilon = false;

  if (tail->begin->next != tail->end) { // if tail is not epsilon
    //std::cout<<"Tail is not epsilon"<<std::endl;

    n->append(tail->begin->next); // add the operator

    // std::cout<<tail->begin->next->val<<std::endl;
  } else {
    //std::cout<<"Tail is epsilon"<<std::endl;
      tail_epsilon = true;
  }
  n->append(head);
  // List* curr = head->begin->next;
  
  // while (curr != head->end) {  
  //   n->append(curr);
  //   curr = curr->next;
  // }

  
  if (!tail_epsilon) {
    List* c = tail->begin->next->next; // start from one after the op
    while (c != tail->end) {
      n->append(c);
      c = c->next;
    }
  }
  return n;
}

List* expr(std::set<token> follow_set) {
  check_for_error(s_e, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_lparen:
  case t_id:
  case t_intconst:
  case t_realconst:
  case t_while:
    //std::cout<<"predict expr --> term term_tail"<<std::endl;
    tree = combine(term (*first[s_tt]), term_tail (*follow[s_e]));
    break;
  default: std::cout<<"error at expr"<<std::endl;error ();
    // return;
  }
  //std::cout<<"printing expr tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

List* term_tail (std::set<token> follow_set) {
  check_for_error(s_tt, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_add:
  case t_sub:
    //std::cout<<"predict term_tail --> add_op term term_tail"<<std::endl;
    tree->append(add_op (*follow[s_ao]));
    tree->append(combine(term (*follow[s_t]), term_tail (*follow[s_tt])));
    break;
  case t_rparen:
  case t_id:
  case t_read:
  case t_write:
  case t_eof:
  case t_if:
  case t_while:
  case t_less:
  case t_great:
  case t_lesseq:
  case t_greateq:
  case t_eq:
  case t_end:
  case t_int:
  case t_real:
    //std::cout<<"predict term_tail --> epsilon"<<std::endl;
    break;          /*  epsilon production */
  default: std::cout<<"error at term tail"<<std::endl;
    //error ();
    // return;
  }
  //std::cout<<"printing tt tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

List* term (std::set<token> follow_set) {
  check_for_error(s_t, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_id:
  case t_intconst:
  case t_realconst:
  case t_lparen:
  {
    //std::cout<<"predict term --> factor factor_tail"<<std::endl;
    List* f = factor (*follow[s_f]);
    
    tree = combine(f, factor_tail (*follow[s_ft]));
  } break;
  default: std::cout<<"Error at term"<<std::endl;//error ();
    // return;
  }
  //std::cout<<"printing t tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

List* factor_tail (std::set<token> follow_set) {
  check_for_error(s_ft, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_mul:
  case t_div:
    // std::cout<<"predict factor_tail --> mul_op factor factor_tail"<<std::endl;
    tree->append(mul_op (*follow[s_mo]));
    tree->append(combine(factor (*follow[s_f]), factor_tail (*follow[s_ft])));
    break;
  case t_add:
  case t_sub:
  case t_rparen:
  case t_id:
  case t_read:
  case t_write:
  case t_eof:
  case t_while:
  case t_if:
  case t_less:
  case t_great:
  case t_lesseq:
  case t_greateq:
  case t_eq:
  case t_end:
  case t_int:
  case t_real:
    //std::cout<<"predict factor_tail --> epsilon"<<std::endl;
    break;          /*  epsilon production */
  default: std::cout<<"factor tail error"<<std::endl;
    std::cout<<token_image<<std::endl;
    //error ();
    //return;
  }
  //std::cout<<"printing ft tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

List* factor (std::set<token> follow_set) {
  check_for_error(s_f, follow_set);
  List* tree = new List(true, true);
  std::set<token> follow;
  follow.insert(t_rparen);
  switch (input_token) {
  case t_id :
    //std::cout<<"predict factor --> id"<<std::endl;
    tree->append("id");
    tree->append(match (t_id));
    break;
  case t_intconst:
    //std::cout<<"predict factor --> int_const"<<std::endl;
    tree->append("int_const");
    tree->append(match (t_intconst));
    break;
  case t_realconst:
    //std::cout<<"predict factor --> real_const"<<std::endl;
    tree->append("real_const");
    tree->append(match (t_realconst));
    break;
  case t_lparen: {
    //std::cout<<"predict factor --> lparen expr rparen"<<std::endl;
    match (t_lparen);
    tree->append(expr (follow));
    match (t_rparen);
  } break;
  case t_float:
    tree->append("float");
    match (t_lparen);
    tree->append(expr (follow));
    match (t_rparen);
    break;
  case t_trunc:
    tree->append("trunc");
    match (t_lparen);
    tree->append(expr (follow));
    match (t_rparen);
    break;
    
  default: std::cout<<"factor error"<<std::endl;
    // error ();
    // return;
  }
  //std::cout<<"printing f tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

List* add_op (std::set<token> follow_set) {
  check_for_error(s_ao, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_add:
    //std::cout<<"predict add_op --> add"<<std::endl;
    //tree->append(match (t_add));
    match (t_add);
    tree->append("+");
    break;
  case t_sub:
    //std::cout<<"predict add_op --> sub"<<std::endl;
    //tree->append(match (t_sub));
    match (t_sub);
    tree->append("-");
    break;
  default: std::cout<<"add_op error"<<std::endl;
    //error ();
    // return;
  }
  //std::cout<<"printing addop tree"<<std::endl;
  // tree->print_tree();
  return tree;
}

List* r_op (std::set<token> follow_set) {
  check_for_error(s_ro, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_eq:
    // std::cout<<"predict r_op --> equal"<<std::endl;
    //tree->append(match (t_eq));
    match (t_eq);
    tree->append("==");
    break;
  case t_neq:
    // std::cout<<"predict r_op --> notequal"<<std::endl;
    //tree->append(match (t_neq));
    match (t_neq);
    tree->append("!=");
    break;
  case t_less:
    //std::cout<<"predict r_op --> less"<<std::endl;
    //tree->append(match (t_less));
    match (t_less);
    tree->append("<");
    break;	    
  case t_great:
    // std::cout<<"predict r_op --> great"<<std::endl;
    //tree->append(match (t_great));
    match (t_great);
    tree->append(">");
    break;
  case t_lesseq:
    // std::cout<<"predict r_op --> lessequal"<<std::endl;
    //tree->append(match (t_lesseq));
    match (t_lesseq);
    tree->append("<=");
    break;
  case t_greateq:
    //std::cout<<"predict r_op --> greatequal"<<std::endl;
    //tree->append(match (t_greateq));
    match (t_greateq);
    tree->append(">=");
    break;
	    
  default: std::cout<<"r_op error"<<std::endl;
    //error ();
    // return;
  }
  //std::cout<<"printing rop tree"<<std::endl;
  //tree->print_tree();
  return tree;
}


List* mul_op (std::set<token> follow_set) {
  check_for_error(s_mo, follow_set);
  List* tree = new List(true, true);
  switch (input_token) {
  case t_mul:
    //printf ("predict mul_op --> mul\n");
    //tree->append(match (t_mul));
    match (t_mul);
    tree->append("*");
    break;
  case t_div:
    //printf ("predict mul_op --> div\n");
    //tree->append(match (t_div));
    match (t_div);
    tree->append("/");
    break;
  default: std::cout<<"mul op error"<<std::endl;
    //error ();
    //return;
  }
  //std::cout<<"printing mulop tree"<<std::endl;
  //tree->print_tree();
  return tree;
}

int main () {
  first_follow();
  eps();
  starter_set_init();
  input_token = scan ();
  std::set<token> follow; // empty set 
  program (follow)->print_tree();
  std::cout<<std::endl;
  return 0;
}


