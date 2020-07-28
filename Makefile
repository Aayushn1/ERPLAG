all: compiler

compiler: lexer.c parser.c ast.c sym.c typechecker.c codegen.c driver.c
	gcc -g lexer.c parser.c ast.c sym.c typechecker.c codegen.c driver.c -o compiler

clean:
	rm -rf *o compiler