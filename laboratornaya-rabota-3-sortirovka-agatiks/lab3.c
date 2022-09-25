#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char lastName[21];
    char name[21];
    char patronimic[21];
    long phoneNumber;
} String;

int isStr(char c);

char* takeStr(String *x, char c);

long takeNum(String *x, char c);

int cmp (String *a, String *b);

long partition(String** strings, long l, long r);

void sort(String** strings, long l, long r);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Wrong number of arguments.");
        return 1;
    }
    FILE *in = fopen(argv[1], "r");
    if (in == NULL) {
        printf("Can't find or open file named \"%s\" for reading.", argv[1]);
        return 1;
    }
    String* curr = (String*)malloc(sizeof(String));
    int read = fscanf(in, "%s %s %s %li\n", curr->lastName, curr->name, curr->patronimic, &curr->phoneNumber);
    long size = 0;
    while (read == 4)
    {
        size++;
        read = fscanf(in, "%s %s %s %li\n", curr->lastName, curr->name, curr->patronimic, &curr->phoneNumber);
    }
    fseek(in, 0, SEEK_SET);
    String** strings = (String**) malloc(size * sizeof(String*));
    if (strings == NULL) {
        printf("Memory allocation error");
        fclose(in);
        return 2;
    }
    read = fscanf(in, "%s %s %s %li\n", curr->lastName, curr->name, curr->patronimic, &curr->phoneNumber);
    long i = 0;
    while (read == 4 && i < size) {
        strings[i] = curr;
        i++;
        curr = (String*)malloc(sizeof(String));
        read = fscanf(in, "%s %s %s %li\n", curr->lastName, curr->name, curr->patronimic, &curr->phoneNumber);
    }
    free(curr);
    sort(strings, 0, size - 1);
    FILE *out = fopen(argv[2], "w");
    if (out == NULL) {
        printf("Can't find or open file named \"%s\" for writing.", argv[2]);
        free(strings);
        fclose(in);
        return 1;
    }
    for (i = 0; i < size; i++) {
        fprintf(out, "%s %s %s %li\n", strings[i]->lastName, strings[i]->name, strings[i]->patronimic, strings[i]->phoneNumber);
    }
    free(strings);
    fclose(in);
    fclose(out);
    return 0;
}

int isStr(char c)
{
    return (c == 'l' || c == 'n' || c == 'p');
}

char* takeStr(String *x, char c)
{
    switch(c){
    case 'l': return x->lastName;
    case 'n': return x->name;
    case 'p': return x->patronimic;
    default: return NULL;
    }
}

long takeNum(String *x, char c)
{
    switch(c){
    case 'c': return x->phoneNumber;
    default: return 0;
    }
}

int cmp (String *a, String *b)
{
    char * seq = "lnpc";// l - lastname, n - name, p - patronimic, c - contact number
    for (int i = 0; i < 4; i++) {
        if (isStr(seq[i])) {
            int ans = strcmp(takeStr(a, seq[i]), takeStr(b, seq[i]));
            if (ans != 0) {
                return ans;
            }
        } else {
            long x = takeNum(a, seq[i]), y = takeNum(b, seq[i]);
            if (x < y){
                return -1;
            }
            else if (x > y){
                return 1;
            }
        }
    }
    return 0;
}

long partition(String** strings, long l, long r)
{
    long i = l, j = r;
    String* med = strings[(l + r)>>1], *temp;
    while (i <= j) {
        while (cmp(strings[i], med) < 0) i++;
        while (cmp(med, strings[j]) < 0) j--;
        if (i >= j) {
            break;
        }
        temp = strings[i];
        strings[i] = strings[j];
        strings[j] = temp;
        i++;
        j--;
    }
    return j;
}

void sort(String** strings, long l, long r)
{
    while (l < r) {
        long p = partition(strings, l, r);
        if (p - l < r - l - 1) {
            sort(strings, l, p);
            l = p + 1;
        } else {
            sort(strings, p + 1, r);
            r = p;
        }
    }
}