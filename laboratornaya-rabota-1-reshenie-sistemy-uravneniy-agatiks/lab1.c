#include<stdio.h>
#include <stdlib.h>
#include <math.h>

void swap(float *elems, long long i, long long j, long long n);

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Wrong number of arguments.");
		return 1;
	}
	FILE* in = fopen(argv[1], "r");
	if (in == NULL)
	{
		printf("Can't find or open file named \"%s\" for reading.", argv[1]);
		return 1;
	}

	//read matrix
	long long n = 0;
	fscanf(in, "%lli", &n);
	float* elems = (float*)malloc(n * (n + 1) * sizeof(float));
	if (elems == NULL)
	{
	    printf("Can't allocate memory.");
	    fclose(in);
        return 2;
	}
	for (long long i = 0; i < n; i++)
	{
		for (long long j = 0; j < n + 1; j++)
		{
			fscanf(in, "%f", &elems[i * (n + 1) + j]);
		}
	}
	fclose(in);

	//triangular matrix
    float temp;
    long long ans = 1; // 1 - has only one solution,  2 - has many solutions, 0 - has no solution
    for(long long i = 0; i < n; i++)
    {
        long long helpRow = i;
        for (long long j = i; j < n; j++)
        {
            if(elems[j * (n + 1) + i] == 0)
            {
                continue;
            }
            else
            {
                if (fabsf(elems[j * (n + 1) + i]) > fabsf(elems[helpRow * (n + 1) + i]))
                {
                    helpRow = j;
                }
            }
        }
        if (elems[helpRow * (n + 1) + i] != 0) 
	{
            swap(elems, i, helpRow, n);
            helpRow = i;
            for (long long j = i + 1; j < n; j++) 
            {
                long long cnt = 0;
                temp = elems[j * (n + 1) + i] / elems[i * (n + 1) + helpRow];
                for (long long k = i; k < n + 1; k++) 
		{
                    elems[j * (n + 1) + k] = elems[j * (n + 1) + k] - elems[i * (n + 1) + k] * temp;
                    if (elems[j * (n + 1) + k] == 0 && k < n) {
                        cnt++;
                    }
                }
                if (cnt == n - i) // Проверяем, что в этот момент не образовалась простая строка,
                    // которая говорит о том, что решение точно не единственное
                {
                    if (elems[j * (n + 1) + n] == 0) 
                    {
                        ans = 2;
                    } 
                    else 
                    {
                        ans = 0;
                        break;
                    }
                }
            }
        }
        else
        {
            continue;
        }
    }

    //answer
    FILE* out = fopen(argv[2], "w");
    if (out == NULL)
    {
        printf("Can't find or open file named \"%s\" for writing.", argv[2]);
        //следить нужно за массивами elems и solution и файлами in и out
        // in уже закрыли точно, out пытаемся открыть, solution ещё не создан
        free(elems);
        return 1;
    }
    if (ans == 1)
    {
        float* solution = (float*)malloc(n * sizeof(float));
        if (solution == NULL)
        {
            printf("Can't allocate memory.");
            //in закрыли, solution создаём
            fclose(out);
            free(elems);
            return 2;
        }
        for (long long i = n - 1; i >= 0; i--)
        {
            solution[i] = elems[i * (n + 1) + n];
            for (long long j = i + 1; j < n; j++)
            {
                if (j != i)
                {
                    solution[i] = solution[i] - elems[i * (n + 1) + j] * solution[j];
                }
            }
            if (elems[i * (n + 2)] != 0)
            {
                solution[i]= solution[i] / elems[i * (n + 2)];
            }
            else
            {
                if (solution[i] == 0) {
                    ans = 2;
                }
                else {
                    ans = 0;
                    break;
                }

            }
        }
        if(ans == 1)
        {
            for (long long i = 0; i < n; i++)
            {
                fprintf(out, "%g\n", solution[i]);
            }
        }

        free(solution);
    }
    if(ans == 0)
    {
        fprintf(out, "no solutions");
    }
    else if (ans == 2)
    {
        fprintf(out, "many solutions");
    }

    free(elems);
    fclose(out);
    return 0;
}

void swap(float *elems, long long i, long long j, long long n)
{
    float temp;//не инициализирую, потому что перед использованием проинизализируется
    for(long long k = i; k < n + 1; k++)
    {
        temp = elems[j * (n + 1) + k];
        elems[j * (n + 1) + k] = elems[i * (n + 1) + k];
        elems[i * (n + 1) + k] = temp;
    }
}
