default:
	clear
	flex -l "misc/lexer.l"
	bison -dv "misc/parser.y" 
	gcc -o parser parser.tab.c lex.yy.c -lfl
compile :
	gcc -c ./src/assembler.c -o  asembler
	gcc -c test3.c -o  test3
	gcc test3 asembler -o result