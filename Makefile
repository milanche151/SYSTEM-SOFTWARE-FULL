default:
	clear
	flex --outfile=src/lex.yy.c -l "misc/lexer.l" 
	bison --defines=inc/parser.tab.h --output=src/parser.tab.c -v "misc/parser.y" 
	gcc -o build/parser \
	-I inc \
	src/parser.tab.c \
	src/lex.yy.c \
	src/assembler.c \
	src/main.c \
	src/util.c \
	-lfl
# compile :
# 	gcc -c ./src/assembler.c -o  asembler
# 	gcc -c test3.c -o  test3
# 	gcc test3 asembler -o result