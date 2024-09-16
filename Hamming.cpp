#include <stdio.h>
#include <cmath>
#include <stdlib.h>
//void 
void printH(int **H, int r, int n);
void printA(int **A, int r, int k);
void printG(int **G, int k, int n);
void printerr(int **err, int m, int n);
void printu(int **u, int m, int k);

int main()
{
    int r, n, m;
    int k;
    char dump[50];  

    // read file(sim.txt)
    FILE *fp;
    fp = fopen("sim.txt", "r"); // Change here
    
    //read r row and n column
    fscanf(fp, "%d\n", &n); 
    fgets(dump,50,fp);
    fscanf(fp, "%d\n", &r);
    fgets(dump,50,fp);
    
    k = n - r; 
    
	// malloc
    // int H[r][n] (size r*n)
    int **H = (int **)malloc(r * sizeof(int *));
    for (int i = 0; i < r; i++)
    {
        H[i] = (int *)malloc(n * sizeof(int));
    }
    //read matrix H
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fscanf(fp, "%d", &H[i][j]);
        }
        fgets(dump,50,fp);
    }
    //read m
    fscanf(fp, "%d\n", &m);
    fgets(dump,50,fp);

    // int err[m][n]
    int **err = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++)
    {
        err[i] = (int *)malloc(n * sizeof(int));
    }
    //read error pattern([m][n])
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fscanf(fp, "%d", &err[i][j]);
        }
        fgets(dump,50,fp);
    }
    //print r , n , k , m , H
    printf("r = %d\n", r);
    printf("n = %d\n", n);
    printf("k = %d\n", k);
    printf("m = %d\n\n", m);
    printf("error pattern:\n");
    printerr(err, m, n);
    printf("H:\n");
    printH(H, r, n);

    // int A[r][k] (G[I|A], H[A|I])
    int **A = (int **)malloc(r * sizeof(int *));
    for (int i = 0; i < r; i++)
    {
        A[i] = (int *)malloc(k * sizeof(int));
    }

    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < k; j++)
        {
            A[i][j] = H[i][j];
        }
    }
    //printf("A:\n");
    //printA(A, r, k);

    // int G[k][n];
    int **G = (int **)malloc(k * sizeof(int *));
    for (int i = 0; i < k; i++)
    {
        G[i] = (int *)malloc(n * sizeof(int));
    }
    //Caculate G form H
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j < k)
                G[i][j] = (i == j) ? 1 : 0;
            else
                G[i][j] = A[j - k][i];
        }
    }

    printf("G:\n");
    printG(G, k, n);

    //--------------------------------------------------Encoder------------------------------------------------------------//

    //u sequence
    int x[63] = {0};
    x[0] = 1;
    for (int i = 1; i <= 5; i++)
    {
        x[i] = 0;
    }
    for (int i = 6; i < 63; i++)
    {
        x[i] = x[i - 5] ^ x[i - 6];
    }
    printf("x :\n");
    for (int i = 0; i < 63; i++)
    {
        printf("%d ", x[i]);
    }
    printf("\n\n");
    
    // int u[m][k]
    int **u = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++)
    {
        u[i] = (int *)malloc(k * sizeof(int));
    }
    //declare m k-bit information messages
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k; j++)
        {
            u[i][j] = x[((k * i) + j) % 63];	//%63 (mod63)
        }
    }
    printf("Information message u:\n");
    printu(u, m, k);
    
    int codeword[m][n];
    int syndrome[m][r]; 
    int received_signal[m][n]; 
    int correct_signal[m][n] = {{0},{0}}; 
    int ans[m]; 
    int if_wrong[r]; 
    int col[m];
    
    //
    for (int count = 0 ; count < m ; count++)
    {
        
        //codeword = u * G
        for (int j = 0 ; j < n ; j ++)
        {
            codeword[count][j] = 0;
            for (int i = 0; i < k; i++)
            {
                codeword[count][j] += u[count][i] * G[i][j];
            }
            codeword[count][j] = codeword[count][j] % 2;
        }
        
        for (int i = 0 ; i < n; i++)
        
        //received signal = codeword + error
        for (int i = 0; i < n; i++)
        {
            received_signal[count][i] = codeword[count][i] ^ err[count][i]; // ^ :XOR 
        }
                      
        //----------------------------------------------------Decoder-----------------------------------------------------------------//
        for (int i = 0; i < n; i++)
        {
            syndrome[count][i] = 0;
        }
        //syndrome = H * received signal
        for (int i = 0 ; i < r ; i ++)
        {
            for (int j = 0 ; j < n ; j ++)
            {
                syndrome[count][i] += H[i][j] * received_signal[count][j];
                //printf("%d", syndrome[count][i]);
            }
            syndrome[count][i] = syndrome[count][i] % 2;
            //printf("  ");
            //printf("%d", syndrome[count][i]);
            //printf("\n");
        }

        printf("Syndrome s:\n");
        for (int i = 0; i < r; i++)
        {
            printf("%d ", syndrome[count][i]);
        }
        printf("\n\n");

        int count_same = 0; // if count_same = r, jth column is where the wrong bit at 
        int if_syn_zero = 0; // to count if there is no error
        
        //To compare which column of H is same as the syndrome
        //if syndrome = 0, no error
        for (int j = 0 ; j < n ; j ++)
        {
            count_same = 0;
            for (int i = 0 ; i < r; i ++)
            {
                if_syn_zero += syndrome[count][i];
                if (H[i][j] == syndrome[count][i])
                    count_same++;
            }
            if (count_same == r)
            {
                ans[count] = j;
                if_wrong[count] = 1;
				break;
            }
            else if(if_syn_zero == 0)
            {
                if_wrong[count] = -1;
            }
        }
        
        //print wrong bit or correct
        if(if_wrong[count] == 1)
        {
            printf("Wrong bit:\n");
            printf("%d", ans[count]);
            printf("\n\n");
            col[count] = ans[count];
        }
        else if(if_wrong[count] == -1)
        {
            printf("Correct!!!");
            printf("\n\n"); 
        }
        
    }
    
    printf("received signal:\n");
	for(int i = 0 ; i < m ; i ++)
	{
		for(int j = 0 ; j < n ; j ++)
		{
			printf("%d " , received_signal[i][j]);		
		}
		printf("\n");
	} 
	printf("\n");
	
	printf("ans:\n");
	for(int i = 0 ; i < m ; i ++)
	{
		printf("%d \n" , col[i]);
	}
	printf("\n");
    
	//Correct the receceived signal to correct signal
    for(int a = 0 ; a < m ; a ++)
    {
        if (if_wrong[a] == 1)
        {
        	for(int j = 0 ; j < n ; j ++)
            {
                if (j == col[a])
                {
                    correct_signal[a][j] = received_signal[a][j] ^ 1;
                }
                else
                    correct_signal[a][j] = received_signal[a][j];
            }
        }
        else if(if_wrong[a] == -1)
        {          
                for(int j = 0 ; j < n ; j ++)
                {
                    correct_signal[a][j] = received_signal[a][j];
                }
        }
    }
    //print correct siganl
    printf("correct_signal:\n");
    for(int i = 0 ; i < m ; i ++)
    {
        for(int j = 0 ; j < n ; j ++)
        {
            printf("%d " , correct_signal[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    int decode_result[m][k];

    for(int i = 0 ; i < m ; i ++)
    {
        for(int j = 0 ; j < k ; j ++) //¥u¯d«ek¦æ 
        {
            decode_result[i][j] = correct_signal[i][j];
        }
    }


    //print n-bit decode result
    //should same as the information message
    for (int i = 0 ; i < m; i ++)
    {
        for (int j = 0 ; j < k ; j ++)
        {
            printf("%d ", decode_result[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    //store file
    FILE *out;
    out = fopen("u.txt" , "w");
    for (int i = 0 ; i < m ; i ++)
    {
        for (int j = 0 ; j < k ; j ++)
        {
            fprintf(out , "%d " , decode_result[i][j]);
        }
        fprintf(out , "\n");
    }

    return 0;
}


void printH(int **H, int r, int n)
{
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", H[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printA(int **A, int r, int k)
{
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < k; j++)
        {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printG(int **G, int k, int n)
{
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", G[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printerr(int **err, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", err[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printu(int **u, int m, int k)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k; j++)
        {
            printf("%d ", u[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
