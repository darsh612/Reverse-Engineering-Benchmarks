#include <stdio.h>
#include <stdlib.h>

void main(int argc, char **argv)
{ 
	
	int long unsigned	iterations;		/* number of iterations in the loop*/
	int					dist;			/* max distance in bytes between two subsequent branch instruction */										
	int					branches;		/* number of branches inside a loop */
	
	char file_name[30];					/* name of the generated C file */
	FILE * fout;						/* generated C file */
    int m;								/* number of iterations in millions, part of the file name */

	int j, k;							/* loop indexes */
    int num_mov, num_cmp, num_clc;		/* number of mov, cmp and clc instructions in the distance code*/

	if (argc != 4) {
		printf("Usage is: %s number_of_iterations distance number_of_branches\n",argv[0]);
		exit(1);
	}

	iterations = atoi(argv[1]);
	dist = atoi(argv[2]);
	branches = atoi(argv[3]);

    
	/* generate file name */

	m = iterations/100;
	if (iterations%100 != 0) {
		fprintf(stderr,"Number of iterations must be hundreds\n");
		exit(1);
	}

	sprintf(file_name, "T1I%dH%dB%d.cpp",m,dist,branches);
    
	/* open output file */
	
	fout=fopen (file_name,"w");
	
	/* write the content of the program */
	fprintf(fout, "#define _GNU_SOURCE\n");
	fprintf(fout, "#include <iostream>\n");
	fprintf(fout, "#include <cstdint>\n");
	fprintf(fout, "#include <random>\n");
	fprintf(fout, "#include <x86intrin.h>\n");
	fprintf(fout, "#include <windows.h>\n");
	fprintf(fout, "#include <fstream>\n");
	fprintf(fout, "#include <sstream>\n");
	fprintf(fout,"\n\n");
	fprintf(fout,"using namespace std;\n");
	fprintf(fout,"\n");
	fprintf(fout,"int main(void) { \n\t");
	fprintf(fout,"SetThreadAffinityMask(GetCurrentThread(), 1);\n\t");
	fprintf(fout,"int branches = %d;\n\t",branches);
	fprintf(fout,"int m = %d;\n\t",m);
	fprintf(fout,"int dist = %d;\n\t",dist);
	fprintf(fout,"std::ostringstream filename;\n\t");
	fprintf(fout,"filename << \"timings\" << m << \"H\" <<dist<< \"B\" <<\".csv\";\n\t");
	fprintf(fout,"ofstream csv_file;\n\t");
	fprintf(fout,"csv_file.open(filename.str().c_str(),ios_base::app); // append instead of overwrite\n\t");
	fprintf(fout,"uint64_t time1, time2;\n");
	fprintf(fout,"\n\t");
	fprintf(fout,"__asm__ volatile (\"mfence\" ::: \"memory\");\n\t");
	fprintf(fout,"__asm__ volatile (\n\t\t");
	fprintf(fout,"\"CPUID\\n\\t\"\n\t\t");
	fprintf(fout,"\"RDTSC\\n\\t\"\n\t\t");
	fprintf(fout, ": \"=a\"(time1)\n\t\t");
	fprintf(fout,": : \"%%rbx\", \"%%rcx\", \"%%rdx\"\n\t");
	fprintf(fout,");\n\n\t");
	fprintf(fout,"int long unsigned i;\n\t");
	fprintf(fout,"int long unsigned liter = %d;\n\t", iterations);
	fprintf(fout,"for (i=0; i<liter; ++i){\n\t\t");
	fprintf(fout,"__asm__ volatile ( \n\t\t\t");

	/* generate sequence of asm instructions */
	/* assume short in-loop branches, distance < 128  */
	/* distance between loop condition branch and first in-loop branch is fixed to 16:
	   6 for jae + 5 mov + 3 cmp + 2* 1 clc */
	
	dist = dist - 2;
	num_mov = dist/5;
	num_cmp = (dist%5)/3;
	num_clc = (dist%5)%3;

	fprintf(fout, "\"clc\\n\"\n\t\t\t");
	fprintf(fout, "\"clc\\n\"\n\t\t\t");
	
	for (j = 0; j < branches - 1; j++) { 
		if (j == 0) {	/*  set condition code for the first branch */
			fprintf(fout, "\"mov $10, %%eax\\n\"\n\t\t\t");
			fprintf(fout, "\"cmp $15, %%eax\\n\"\n\t\t\t");
		}
		fprintf(fout, "\"jle l%d\\n\"\n\t\t\t", j);

		/* generate noops */
		/* short branch is two bytes */
		
		for (k = 1; k <= num_mov; k++) { 
			fprintf(fout, "\"mov $10, %%eax\\n\"\n\t\t\t");
		}
		for (k = 1; k <= num_cmp; k++) { 
			fprintf(fout, "\"cmp $15, %%eax\\n\"\n\t\t\t");
		}
        for (k = 1; k <= num_clc; k++) { 
			fprintf(fout, "\"clc\\n\"\n\t\t\t");
		}
		
		/* we can always generate a label (even for the last iteration - no harm) */
		
		fprintf(fout, "\"l%d:\\n\"\n\t\t\t", j); /* no new line here */
	}
	
	fprintf(fout, "\"clc\\n\"\n\t\t"); /* last branch targert */
	fprintf(fout, ");\n\t");	/* close _asm { */
	fprintf(fout, "}\n\n\t");	/* close for loop */
	fprintf(fout,"__asm__ volatile (\n\t\t");
	fprintf(fout,"\"CPUID\\n\\t\"\n\t\t");
	fprintf(fout,"\"RDTSC\\n\\t\"\n\t\t");
	fprintf(fout, ": \"=a\"(time2)\n\t\t");
	fprintf(fout,": : \"%%rbx\", \"%%rcx\", \"%%rdx\"\n\t");
	fprintf(fout,");\n\n\t");	
	fprintf(fout,"__asm__ volatile (\"mfence\" ::: \"memory\");\n\t");
	fprintf(fout, "cout<<\" TIme taken to execute this code is : \"<< (time2-time1)/(liter);\n\t");
	fprintf(fout,"csv_file << branches << \",\" << (time2 - time1)/(liter) << \"\\n\";\n\t");
	fprintf(fout,"cout<< \"Time1 is \" << time1 << \" and Time2 is \"<<time2;\n\t");
	fprintf(fout,"csv_file.close();\n\t");
	fprintf(fout,"return 0;\n");
	fprintf(fout, "}\n");	/* close main */
	fclose(fout);			/* close this file */
}