default:
	clear
	mkdir -p build
	flex --outfile=src/lex.yy.c \
		-l "misc/lexer.l" 
	bison --defines=inc/parser.tab.h \
		--output=src/parser.tab.c \
		--debug \
		-v "misc/parser.y" 
	gcc -o build/assembler \
		-I inc \
		-g src/parser.tab.c \
		-g src/lex.yy.c \
		-g src/assembler.c \
		-g src/instr.c \
		-g src/asm_main.c \
		-g src/util.c \
		-lfl
	gcc -o build/linker \
		-I inc \
		-g src/linker.c \
		-g src/ld_main.c \
		-g src/util.c \
		-lfl
	gcc -o build/emulator \
		-I inc \
		-g src/emu_main.c \
		-g src/emulator.c \
		-g src/util.c