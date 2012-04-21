#include	<stdio.h>
#include	<stdlib.h>


int main(int argc, char **argv)
{
	FILE*	input	= NULL;
	FILE*	output	= stdout;
	int	tmp=0;
	int	i=0;

	if(argc<3)
	{
		puts("Syntax: rcconv [input] [variable]\n");
		exit(0);
	}


	input=fopen(argv[1],"r");


	fprintf(output,"const char %s[] = {\n",argv[2]);

	for(i=0;!feof(input);++i)
	{
		tmp=0;
		fread(&tmp,1,1,input);
		fprintf(output,"0x%X,", tmp);
		if(!(i%16))
			fputs("\n",output);
	}


	fputs("};\n",output);




	return EXIT_SUCCESS;
}
