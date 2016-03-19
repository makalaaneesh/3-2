letter[a-zA-Z]
digit[0-9]

%{

int wordcount = 0;
%}

%%
{letter}+ wordcount++;
[^{letter}] printf("%s is a special character\n", yytext);

%%

int yywrap(){
	return 1;
}
int main(int argc, char *argv[]){
	yyin = fopen(argv[1], "r");
	yylex();
	printf("wordcount is %d\n", wordcount);
	fclose(yyin);
	return 0;
}