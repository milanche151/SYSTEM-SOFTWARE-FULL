default:
	clear
	flex -l lexer.l
	bison -dv parser.y 
	gcc -o parser parser.tab.c lex.yy.c -lfl