---
layout: post
title: Cache Lab Part A
excerpt: "CSAPP实验中的一个。模拟缓存的hit/miss"
categories: [CS]
comments: true
---



```c
#include "cachelab.h"

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct line_t {
    // a line of the cache
    // since this is just a simulator, we do not need to fetch data from memory really,
    // so I omit the B part
    int valid;
    unsigned long tag;
} line;

typedef struct set_t {
    // a set contains E lines
    // actually, the time complexity of load and eviction of a LRU cache could be O(1)
    // but since there is no std library of hashmap in C, so I did not bother implement
    // it. The solution could be found in a Leetcode question with keyword LRU.
    line *lines;
} set;

set *sets;      // the simulated cache
int s = 0, E = 0, b = 0;            // parameters of the cache
int miss = 0, hit = 0, eviction = 0;            // evaluation
int op_count = 0;                       // count the num of ops. for LRU
int vflag = 0;                      // verbose flag

// fetch miss data from memory
void fetch(unsigned set_index, unsigned long tag);

void load(unsigned set_index, unsigned long tag) {
    int hit_flag = 0;
    for (int i = 0; i < E; ++i) {
        if (sets[set_index].lines[i].valid && (sets[set_index].lines[i].tag == tag)) {
            // hit
            hit_flag = 1;
            sets[set_index].lines[i].valid = op_count;
            break;
        }
    }
    if (hit_flag) {
        ++hit;
        if (vflag) printf("%s", " hit");
    } else {
        ++miss;
        if (vflag) printf("%s", " miss");
        // fetch from memory
        fetch(set_index, tag);
    }
}

void modify(unsigned set_index, unsigned long tag) {
    load(set_index, tag);
    ++hit;      // the store op must hit since the block has been loaded into cache
    if (vflag) printf("%s", " hit");
}

void fetch(unsigned set_index, unsigned long tag) {
    int find_flag = 0;          // whether find a unvalid line
    for (int i = 0; i < E; ++i) {
        if (!sets[set_index].lines[i].valid) {
            sets[set_index].lines[i].valid = op_count;
            sets[set_index].lines[i].tag = tag;
            find_flag = 1;
            break;
        }
    }
    if (find_flag) return;
    // eviction happens. select the LRU line
    ++eviction;
    if (vflag) printf("%s", " eviction");
    int least_time = 0x7fffffff, least = -1;
    for (int i = 0; i < E; ++i) {
        if (sets[set_index].lines[i].valid < least_time) {
            least_time = sets[set_index].lines[i].valid;
            least = i;
        }
    }

    // replace the LRU line with the new fetched line
    sets[set_index].lines[least].valid = op_count;
    sets[set_index].lines[least].tag = tag;
}

int main(int argc, char **argv) {
    // read args
    char *tracefile = NULL;
    char c;      // tmp value
    opterr = 0;

    char usage[] = "Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n\
Options:\n\
  -h         Print this help message.\n\
  -v         Optional verbose flag.\n\
  -s <num>   Number of set index bits.\n\
  -E <num>   Number of lines per set.\n\
  -b <num>   Number of block offset bits.\n\
  -t <file>  Trace file.\n\
\n\
Examples:\n\
  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n\
  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n";

    while ((c = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (c) {
            case 'h':
                printf(usage, argv[0], argv[0], argv[0]);
                return 1;
            case 'v':
                vflag = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                tracefile = optarg;
                break;
            case '?':
                if (optopt == 's' || optopt == 'E' || optopt == 'b' || optopt == 't')
                    fprintf(stderr, "Option -%c requires an argument.\n",
                            optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                abort();
        }
    }

    // check mandatory options
    if (s == 0 || E == 0 || b == 0 || tracefile == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printf(usage, argv[0], argv[0], argv[0]);
        return 1;
    }
    // validate the file path
    if (access(tracefile, F_OK) == -1) {
        printf("%s: No such file or directory\n", tracefile);
        return 1;
    }

    // construct lines
    int num_sets = 1 << s;
    sets = (set*) malloc(num_sets * sizeof(set));
    for (int i = 0; i < num_sets; ++i) {
        sets[i].lines = (line*) malloc(E * sizeof(line));
        for (int j = 0; j < E; ++j)
            sets[i].lines[j].valid = 0;
    }

    // open trace file and parse
    FILE *fp = fopen(tracefile, "r");
    char buff[255];
    char instruction;
    unsigned long address, tag;
    unsigned set_index;
    int size;       // the size is omitted by the assumption of part A
    while (fgets(buff, 255, (FILE *)fp)) {
        if (buff[0] == 'I') continue;
        sscanf(buff, " %c %lx,%d", &instruction, &address, &size);

        if (vflag) printf("%c %lx,%d", instruction, address, size);  // verbose

        // compute tag, set index and offset
        address >>= b;      // omit offset
        set_index = address & ((1 << s) - 1);
        tag = address >> s;

        // treat L and S the same
        ++op_count;
        if (instruction == 'M') modify(set_index, tag);
        else load(set_index, tag);

        if (vflag) printf("%s", " \n");
    }
    fclose(fp);

    printSummary(hit, miss, eviction);

    // free allocated memory
    for (int i = 0; i < num_sets; ++i) {
        free(sets[i].lines);
    }
    free(sets);

    return 0;
}
```

