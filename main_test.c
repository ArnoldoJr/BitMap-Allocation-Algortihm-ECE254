/**
  * ECE254 Linux Dynamic Memory Management Lab
  * @file: main_test.c
  * @brief: The main file to write tests of memory allocation algorithms
  */ 

/* includes */
/* system provided header files. You may add more */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* non-system provided header files. 
   Do not include more user-defined header files here
 */
#include "mem.h"

#define EQUAL_STRINGS 0

#define BYTES_PARAM 1
#define ALGO_PARAM 2
#define TEST_PARAM 3

#define BEST_FIT 0
#define WORST_FIT 1

#define UNKNOWN_TEST -1
#define DEFAULT_TEST 000
#define BAD_ALLOC_TEST 010
#define MIXED_BLOCKS_TEST 100
#define BIG_BLOCKS_TEST 110
#define SMALL_BLOCKS_TEST 120
#define ALLOC_DEALLOC_TEST_A 200
#define MERGE_SPACES_TEST 300
#define MERGE_SPACES_TEST_BIG 310
#define BIG_SMALL_BLOCKS_TEST 400
#define REAL_ACTIVITY_A 900
#define REAL_ACTIVITY_B 910
#define REAL_ACTIVITY_C 920
#define REAL_ACTIVITY_D 930

int memory_init(int algo, int pool) {
    if (algo == BEST_FIT) {
        return best_fit_memory_init(pool);
    }
    else if (algo == WORST_FIT) {
        return worst_fit_memory_init(pool);
    }
    return -1;
}

void *alloc(int algo, int size) {
    if (algo == BEST_FIT) {
        return best_fit_alloc(size);
    }
    else if (algo == WORST_FIT) {
        return worst_fit_alloc(size);
    }
    return NULL;
}

void dealloc(int algo, void* pointer) {
    if (algo == BEST_FIT) {
        best_fit_dealloc(pointer);
    }
    else if (algo == WORST_FIT) {
        worst_fit_dealloc(pointer);
    }
}

int count_extfrag(int algo, int size) {
    if (algo == BEST_FIT) {
        return best_fit_count_extfrag(size);
    }
    else if (algo == WORST_FIT) {
        return worst_fit_count_extfrag(size);
    }
    return -1;
}

int main(int argc, char *argv[])
{
	int algo = BEST_FIT; // default algorithm to test is best fit
	int test = DEFAULT_TEST;
	int pool = 1024;

	if (argc != 4) {
		printf("Usage: %s <byes> <algo> <test>. Bytes is memory pool size in bytes. Algorithm is 'best' or 'wrost' fit. Tests are: 'a', 'b', 'c'.\n", argv[0]);
		exit (-1);
	}

    char* bytesParam = argv[BYTES_PARAM];
    char* algoParam = argv[ALGO_PARAM];
    char* testParam = argv[TEST_PARAM];

	if (strcmp(algoParam, "best")  != EQUAL_STRINGS && strcmp(algoParam, "worst") != EQUAL_STRINGS) {
		printf("Invalid algorithm type, should be 'best' or 'worst'.\n");
		exit(-1);
	}
    else if (atoi(bytesParam) < 0) {
        printf("Invalid size of memory pool, should be >0 bytes.\n");
        exit(-1);
    }
	else {

	    pool = atoi(bytesParam);

		if (strcmp(algoParam, "best") == EQUAL_STRINGS) { algo = BEST_FIT; }
		else if (strcmp(algoParam, "worst") == EQUAL_STRINGS) { algo = WORST_FIT; }
		else { algo = BEST_FIT; }

		if (strcmp(argv[TEST_PARAM], "default") == EQUAL_STRINGS) {
		    printf("DEFAULT_TEST\n");
			test = DEFAULT_TEST;
		}
        else if (strcmp(testParam, "bad") == EQUAL_STRINGS) {
            printf("BAD_ALLOC_TEST\n");
            test = BAD_ALLOC_TEST;
        }
        else if (strcmp(testParam, "mixed") == EQUAL_STRINGS) {
            printf("MIXED_BLOCKS_TEST\n");
            test = MIXED_BLOCKS_TEST;
        }
        else if (strcmp(testParam, "big") == EQUAL_STRINGS) {
            printf("BIG_BLOCKS_TEST\n");
            test = BIG_BLOCKS_TEST;
        }
        else if (strcmp(testParam, "small") == EQUAL_STRINGS) {
            printf("SMALL_BLOCKS_TEST\n");
            test = SMALL_BLOCKS_TEST;
        }
        else if (strcmp(testParam, "alloc") == EQUAL_STRINGS) {
            printf("ALLOC_DEALLOC_TEST_A\n");
		    test = ALLOC_DEALLOC_TEST_A;
		}
        else if (strcmp(testParam, "merge") == EQUAL_STRINGS) {
            printf("MERGE_SPACES_TEST\n");
            test = MERGE_SPACES_TEST;
        }
        else if (strcmp(testParam, "bigsmall") == EQUAL_STRINGS) {
            printf("BIG_SMALL_BLOCKS_TEST\n");
            test = BIG_SMALL_BLOCKS_TEST;
        }
        else if (strcmp(testParam, "mergebig") == EQUAL_STRINGS) {
            printf("MERGE_SPACES_TEST_BIG\n");
            test = MERGE_SPACES_TEST_BIG;
        }
        else if (strcmp(testParam, "reala") == EQUAL_STRINGS) {
            printf("REAL_ACTIVITY_A\n");
            test = REAL_ACTIVITY_A;
        }
        else if (strcmp(testParam, "realb") == EQUAL_STRINGS) {
            printf("REAL_ACTIVITY_B\n");
            test = REAL_ACTIVITY_B;
        }
        else if (strcmp(testParam, "realc") == EQUAL_STRINGS) {
            printf("REAL_ACTIVITY_C\n");
            test = REAL_ACTIVITY_C;
        }
        else if (strcmp(testParam, "reald") == EQUAL_STRINGS) {
            printf("REAL_ACTIVITY_D\n");
            test = REAL_ACTIVITY_D;
        }
		else {
			test = UNKNOWN_TEST;
		}
	}

    printf("Initializing %d bytes.\n", pool);
    if (memory_init(algo, pool) == -1) {
        printf("Could not initialize memory pool.\n");
        exit(-1);
    }

    switch(test) {
        case DEFAULT_TEST: {
            printf("Starting default test.\n");
            void *p;
            int num = -1;

            p = alloc(algo, 8);
            printf("p=%p\n", p);
            if (p != NULL) {
                dealloc(algo, p);
            }
            num = count_extfrag(algo, 4);
            printf("num = %d\n", num);

            break;
        }
        case BAD_ALLOC_TEST: {
            printf("Starting bad alloc test.\n");
            void *p;

            p = alloc(algo, pool * 2);
            printf("alloc of %dB: p=%p\n", pool * 2, p);

            p = alloc(algo, pool - 8);
            printf("alloc of %dB: p=%p\n", pool - 8, p);

            p = alloc(algo, 0);
            printf("alloc of 0B: p=%p\n", p);

            p = alloc(algo, -1);
            printf("alloc of -1B: p=%p\n", p);

            dealloc(algo, NULL);

            p = alloc(algo, 8);
            printf("alloc of 64B: p=%p\n", p);
            dealloc(algo, p + 9);
            printf("dealloc of (p + 9B)\n");

            break;
        }
        case MIXED_BLOCKS_TEST: {
            printf("Starting allocation of blocks of various sizes test.\n");
            void *p;
            int num = -1;
            int S;

            for (S = 8; S <= 4096; S *= 2) {
                p = alloc(algo, S);

                if (p != NULL) dealloc(algo, p);
                num = count_extfrag(algo, 4);
                printf("extfrag of size < 4B (should be zero): %d\n", num);
            }

            break;
        }
        case BIG_BLOCKS_TEST: {
            printf("Starting big block allocation test. Worst fit should perform worse.\n");
            void *p[4];
            void *q;
            int num;
            int i;

            for (i = 0; i < 4; i++) {
                p[i] = alloc(algo, 64);
                printf("64B alloc: p[%d]=%p\n", i, p[i]);
            }

            dealloc(algo, p[1]);
            printf("dealloc p[1]\n");
            dealloc(algo, p[3]);
            printf("dealloc p[3]\n");

            q = alloc(algo, 64);
            printf("64B alloc: q=%p\n", q);

            num = count_extfrag(algo, pool); // pool is bigger than any possible free space, so this just counts the number of empty blocks
            printf("number of free spaces: %d\n", num);

            break;
        }
        case SMALL_BLOCKS_TEST: {
            printf("Starting small block allocation test.\n");

            void *p[9];
            void *q[4];
            int num = 1;
            int i;

            printf("9x 24B allocs:\n");
            for (i = 0; i < 9; i++) {
                p[i] = alloc(algo, 24);
                printf("p[%d]=%p\n", i, p[i]);
            }

            for (i = 1; i < 9; i += 2) {
                dealloc(algo, p[i]);
                printf("deallocated p[%d]\n", i);
            }

            printf("4x 16B allocs:\n");
            for (i = 0; i < 4; i++) {
                q[i] = alloc(algo, 16);
                printf("q[%d]=%p\n", i, q[i]);
            }

            num = count_extfrag(algo, 16);
            printf("exfrags tof <16B: %d\n", num);

            break;
        }
        case BIG_SMALL_BLOCKS_TEST: {
            printf("Starting big then small the big block allocation test.\n");

            void *p[9];
            int num = 1;
            int i;

            printf("9x 64B allocs:\n");
            for (i = 0; i < 9; i++) {
                p[i] = alloc(algo, 64);
                printf("p[%d]=%p\n", i, p[i]);
            }

            for (i = 1; i < 9; i += 2) {
                dealloc(algo, p[i]);
                printf("deallocated p[%d]\n", i);
            }

            printf("allocated 6x 8B blocks\n");
            for (i = 0; i < 6; i++) {
                alloc(algo, 8);
            }

            num = count_extfrag(algo, 32);
            printf("exfrags of <32B: %d\n", num);

            break;
        }
        case ALLOC_DEALLOC_TEST_A: {
            printf("Starting allocation-deallocation test.\n");
            void *p[10];
            int num = -1;
            int i;

            for (i = 0; i < 10; i++) {
                p[i] = alloc(algo, 16);
                printf("alloc #%d of 16B: p=%p\n", i, p);
            }
            printf("deallocating pointers 1, 3 and 5.\n");
            if (p[1] != NULL) dealloc(algo, p[1]);
            if (p[3] != NULL) dealloc(algo, p[3]);
            if (p[5] != NULL) dealloc(algo, p[5]);
            num = count_extfrag(algo, 24);
            printf("extfrag of size < 24B: %d\n", num);
            break;
        }
        case MERGE_SPACES_TEST: {
            printf("Starting merge of spaces test test.\n");
            void *p[4];
            void *q;
            int num = -1;
            int i;

            for (i = 0; i < 4; i++) {
                p[i] = alloc(algo, 16);
                printf("alloc #%d of 16B: p=%p\n", i, p[i]);
            }

            printf("dealloc of p[1] and p[2], free space should be merged\n");
            dealloc(algo, p[1]);
            dealloc(algo, p[2]);

            num = count_extfrag(algo, 24);
            printf("extfrag of size < 24B: %d\n", num);

            num = count_extfrag(algo, 64);
            printf("extfrag of size < 64B: %d\n", num);

            q = alloc(algo, 24); // Address of best fit should be inside the freed block. Worst fit should be outside.
            printf("alloc #%d of 24B: q=%p\n", i, q);

            dealloc(algo, p[0]);
            dealloc(algo, p[3]);
            dealloc(algo, q);

            break;

        }
        case MERGE_SPACES_TEST_BIG: {
            printf("Starting merge of spaces test test.\n");
            void *p[16];
            void *q;
            void *r;
            int i;

            for (i = 0; i < 16; i++) {
                p[i] = alloc(algo, 16);
                printf("alloc #%d of 16B: p=%p\n", i, p[i]);
            }

            printf("dealloc of p[1,2,3,5,7,8], free space should be merged\n");
            // free size 16*3 = 48
            dealloc(algo, p[1]);
            dealloc(algo, p[2]);
            dealloc(algo, p[3]);

            // free size 16
            dealloc(algo, p[5]); // <-- best fit should put address of q* here

            // free size 16*2 = 32
            dealloc(algo, p[7]);
            dealloc(algo, p[8]); // <-- best fit should put address of r* here

            // worst fit should put addresses of p and q after p[15]

            q = alloc(algo, 8);
            printf("alloc #%d of 8B: q=%p\n", i, q);

            r = alloc(algo, 24);
            printf("alloc #%d of 24B: r=%p\n", i, r);

            for (i = 0; i < 16; i++) {
                if (i != 1 && i != 2 && i != 3 && i != 5 && i != 7 && i != 8) {
                    dealloc(algo, p[i]);
                }
            }
            dealloc(algo, q);
            dealloc(algo, r);

            break;

        }
        case REAL_ACTIVITY_A: {
            printf("Starting real activity test A.\n");
            void *a;
            void *b;
            void *c;
            void *d;
            void *e;
            int num = -1;

            a = alloc(algo, 45);
            printf("alloc of 45B: a=%p\n", a);

            b = alloc(algo, 92);
            printf("alloc of 92B: b=%p\n", b);

            c = alloc(algo, 12);
            printf("alloc of 12B: c=%p\n", c);

            dealloc(algo, b);
            printf("dealloc of b\n");

            d = alloc(algo, 35);
            printf("alloc of 35B: d=%p\n", d);

            dealloc(algo, c);
            printf("dealloc of c\n");

            dealloc(algo, a);
            printf("dealloc of a\n");

            e = alloc(algo, 100);
            printf("alloc of 100B: e=%p\n", e);

            dealloc(algo, d);
            printf("dealloc of d\n");

            dealloc(algo, e);
            printf("dealloc of e\n");

            num = count_extfrag(algo, 8);
            printf("extfrag of size < 8B: %d\n", num);
            num = count_extfrag(algo, 16);
            printf("extfrag of size < 16: %d\n", num);
            num = count_extfrag(algo, 32);
            printf("extfrag of size < 32: %d\n", num);
            num = count_extfrag(algo, 64);
            printf("extfrag of size < 64: %d\n", num);
            num = count_extfrag(algo, 128);
            printf("extfrag of size < 128: %d\n", num);

            break;
        }
        case REAL_ACTIVITY_B: {
            printf("Starting real activity test B.\n");
            void *a, *b, *c, *d, *e, *f, *g, *h;
            int num = -1;

            a = alloc(algo, 8);
            printf("alloc of 8B: a=%p\n", a);

            b = alloc(algo, 8);
            printf("alloc of 8B: b=%p\n", b);

            c = alloc(algo, 8);
            printf("alloc of 8B: c=%p\n", c);

            d = alloc(algo, 8);
            printf("alloc of 8B: d=%p\n", d);

            dealloc(algo, b);
            printf("dealloc of b\n");

            e = alloc(algo, 4);
            printf("alloc of 4B: e=%p\n", e);

            f = alloc(algo, 2);
            printf("alloc of 4B: f=%p\n", f);

            g = alloc(algo, 32);
            printf("alloc of 32B: g=%p\n", g);

            dealloc(algo, d);
            printf("dealloc of d\n");

            h = alloc(algo, 6);
            printf("alloc of 6B: h=%p\n", h);

            dealloc(algo, a);
            dealloc(algo, c);
            dealloc(algo, e);
            dealloc(algo, f);
            dealloc(algo, g);
            dealloc(algo, h);

            num = count_extfrag(algo, 8);
            printf("extfrag of size < 8B: %d\n", num);
            num = count_extfrag(algo, 16);
            printf("extfrag of size < 16: %d\n", num);
            num = count_extfrag(algo, 32);
            printf("extfrag of size < 32: %d\n", num);
            num = count_extfrag(algo, 64);
            printf("extfrag of size < 64: %d\n", num);
            num = count_extfrag(algo, 128);
            printf("extfrag of size < 128: %d\n", num);

            break;
        }
        case REAL_ACTIVITY_C: { // like the one on "Program Workflow" slide
            printf("Starting real activity test C (\"Program Workflow\").\n");
            void *a, *b, *c, *d, *e, *f, *g, *h, *i, *j;
            int num = -1;

            a = alloc(algo, 64);
            printf("alloc of 64B: a=%p\n", a);

            b = alloc(algo, 64);
            printf("alloc of 64B: b=%p\n", b);

            c = alloc(algo, 128);
            printf("alloc of 128B: c=%p\n", c);

            d = alloc(algo, 64);
            printf("alloc of 64B: d=%p\n", d);

            e = alloc(algo, 32);
            printf("alloc of 64B: e=%p\n", e);

            f = alloc(algo, 128);
            printf("alloc of 64B: f=%p\n", f);

            g = alloc(algo, 32);
            printf("alloc of 128B: g=%p\n", g);

            h = alloc(algo, 64);
            printf("alloc of 64B: h=%p\n", h);

            i = alloc(algo, 32);
            printf("alloc of 128B: i=%p\n", i);

            j = alloc(algo, 32);
            printf("alloc of 128B: j=%p\n", j);

            dealloc(algo, c);
            dealloc(algo, e);
            dealloc(algo, g);
            dealloc(algo, i);
            printf("dealloc of c, e, g, i\n");

            num = count_extfrag(algo, 32);
            printf("extfrag of size < 32: %d\n", num);
            num = count_extfrag(algo, 64);
            printf("extfrag of size < 64: %d\n", num);
            num = count_extfrag(algo, 128);
            printf("extfrag of size < 128: %d\n", num);
            num = count_extfrag(algo, 256);
            printf("extfrag of size < 256: %d\n", num);

            break;
        }
        case REAL_ACTIVITY_D: { // INCOMPLETE
            printf("Starting real activity test D.\n");
            void *a[16];
            int i;
            int num = -1;

            for (i = 0; i < 16; i++) {
                a[i] = alloc(algo, 32);
                printf("alloc of 32B: a=%p\n", a[i]);
            }
            while (alloc(algo, 32) != NULL) {} // fill remaining space with 32B blocks

            for (i = 0; i < 16; i+=2) { // free every other a[] pointer
                dealloc(algo, a[i]);
                printf("dealloc of a[%d]\n", i);
            }

            /*
            for (i = 0; i < 16; i++) {
                b[i] = alloc(algo, 16);
                printf("alloc of 32B: a=%p\n", a[i]);
            }
            */

            num = count_extfrag(algo, 32);
            printf("extfrag of size < 32: %d\n", num);
            num = count_extfrag(algo, 64);
            printf("extfrag of size < 64: %d\n", num);
            num = count_extfrag(algo, 128);
            printf("extfrag of size < 128: %d\n", num);
            num = count_extfrag(algo, 256);
            printf("extfrag of size < 256: %d\n", num);

            break;
        }
        default: {
            printf("Unknown test.\n");
            break;
        }
    }

    printf("End of test.\n");
	return 0;
}
