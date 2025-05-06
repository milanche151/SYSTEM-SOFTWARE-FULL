default:
	clear
	flex --outfile=src/lex.yy.c \
		-l "misc/lexer.l" 
	bison --defines=inc/parser.tab.h \
		--output=src/parser.tab.c \
		-v "misc/parser.y" 
	gcc -o build/assembler \
		-I inc \
		-g src/parser.tab.c \
		-g src/lex.yy.c \
		-g src/assembler.c \
		-g src/instr.c \
		-g src/main.c \
		-g src/util.c \
		-lfl
# compile :
# 	gcc -c ./src/assembler.c -o  asembler
# 	gcc -c test3.c -o  test3
# 	gcc test3 asembler -o result

