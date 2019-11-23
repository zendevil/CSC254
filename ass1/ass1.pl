
form1(Prev, Next) :- 
	string_concat("(", Prev, Inter),
	string_concat(Inter, ".)", Next).
form2(Prev, Next) :- 
	string_concat("(.", Prev, Inter),
	string_concat(Inter, ")", Next).

tree(1, ['(.)']) :- !.

tree(N, L) :-
	A is N - 1, tree(A, PrevStrings1),
	maplist(form1 , PrevStrings1, List1),
	maplist(form2 , PrevStrings1, List2),
	append(List1, List2, L).
% write('Enter ; to see the next tree').	
	
