#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

#define GAP_OPEN -11
#define GAP_EXTENTION -1
#define MINUS_INFINITE -100000

// ATTENTION: Here we just output the substring, not aligning result.
// If you want aligning result, un-annotate MARK part

using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        if(!item.empty()){
            elems.push_back(item);
        }
    }
    return elems;
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

vector<vector<int> > &read_matrix(map<char,int> &dict, vector<vector<int> > &matrix)
{
    // read matrix info from a text for augmentability
    ifstream fmatrix ("matrix.txt");

    if(!fmatrix.good())
    {
        cout << "ERROR: illegal input file path: " << "matrix.txt" <<endl;
        cout <<
        "Input format:\n" <<
        "program_name  \n";
        exit(0);
    }
    string row;
    int rowNum = 0;
    while(fmatrix != NULL)
    {
        rowNum ++;
        getline(fmatrix, row, '\n');
        vector<string> columns = split(row, ' ');
        if(rowNum == 1)
        {
            for(int i = 0; i < columns.size(); i++)
            {
                const char c = columns[i][0];
                dict[c]= i;
            }
        }else
        {
            vector<int> temp;
            for(int i = 1; i < columns.size(); i++)
            {
                int numb;
                istringstream(columns[i]) >> numb;
                temp.push_back(numb);
            }
            matrix.push_back(temp);
        }
    }
    return matrix;
}

// Smith Waterman Initialization
void initialize_score_matrix(int **score, char **trackBack, int M, int N)
{
    for (int i = 0; i < M+1; i++)
    {
        score[0][i] = 0;
        trackBack[0][i] = '0';
    }

    for (int i = 0; i < N+1; i++)
    {
        score[i][0] = 0;
        trackBack[i][0] = '0';
    }

    trackBack[0][0] = '0';
}
int maximum(int a, int b, int c)
{
    int m = max(a,b);
    m = max(m,c);
    //m = max(m,0);
    return m;
}

int smith_waterman(string S1, string S2, string &R1, string &R2, map<char,int> &dict, vector<vector<int> > &matrix)
{
    int M = S1.length();
    int N = S2.length();
    /*
    0MMMMMMMMMMMMMMMM
    N
    N
    N
    N
    N
    N
    so the matrix is N*M
    */
    int **score = new int *[N+1];
    for (int i = 0; i <= N; i++)
    {
        score[i] = new int [M+1];
    }

    char **trackBack = new char *[N+1];
    // * for match, - for ->, | for moving downward
    for (int i = 0; i <= N; i++)
    {
        trackBack[i] = new char [M+1];
        for (int k = 0; k <= M; k++)
        {
            trackBack[i][k] = '0';
        }
    }

    char **WtrackBack = new char *[N+1];
    // * for match, - for ->, | for moving downward
    for (int i = 0; i <= N; i++)
    {
        WtrackBack[i] = new char [M+1];
    }

    char **XtrackBack = new char *[N+1];
    // * for match, - for ->, | for moving downward
    for (int i = 0; i <= N; i++)
    {
        XtrackBack[i] = new char [M+1];
    }

    char **YtrackBack = new char *[N+1];
    // * for match, - for ->, | for moving downward
    for (int i = 0; i <= N; i++)
    {
        YtrackBack[i] = new char [M+1];
    }
    /*
    Flag:
    from M 1
    from X 2
    from Y 4
    this M 8
    this X 16
    this Y 32
    */



    R1 = "";
    R2 = "";
    initialize_score_matrix(score, trackBack, M, N);
    int maxScore = 0;
    int maxN = 0;
    int maxM = 0;

    //affine gap penalty
    //since we have already use M, we now use W,X,Y matrix
    //initialization:
    int **W = new int *[N+1];
    for (int i = 0; i <= N; i++)
    {
        W[i] = new int [M+1];
        for (int k = 0; k <= M; k++)
        {
            W[i][k] = 0;
        }
    }

    int **X = new int *[N+1];
    for (int i = 0; i <= N; i++)
    {
        X[i] = new int [M+1];
        for (int k = 0; k <= M; k++)
        {
            X[i][k] = MINUS_INFINITE;
        }
    }

    int **Y = new int *[N+1];
    for (int i = 0; i <= N; i++)
    {
        Y[i] = new int [M+1];
        for (int k = 0; k <= M; k++)
        {
            Y[i][k] = MINUS_INFINITE;
        }
    }


    for (int i = 1; i <=N; i++)
    {
        for (int k = 1; k <= M; k++)
        {
            char S1_k = S1[k-1];
            char S2_i = S2[i-1];
            //calculate W score
            //W[i][k] = W[i-1][k-1], X[i-1][k-1], Y[i-1][k-1];
            if(W[i-1][k-1] >= X[i-1][k-1] && W[i-1][k-1] >= Y[i-1][k-1])
            {
                W[i][k] = W[i-1][k-1];
                WtrackBack[i][k] = 'W';
            }else if (X[i-1][k-1] >= Y[i-1][k-1])
            {
                W[i][k] = X[i-1][k-1];
                WtrackBack[i][k] = 'X';
            }else
            {
                W[i][k] = Y[i-1][k-1];
                WtrackBack[i][k] = 'Y';
            }

            W[i][k] += matrix[dict[S1_k]][dict[S2_i]];
            if (W[i][k] < 0)
            {
                //W[i][k] = 0;
                //WtrackBack[i][k] = '0';
            }
            int matchingCost = W[i][k];

            //gap in S2, calculate X score
            //X[i][k] = maximum(GAP_OPEN + W[i][k-1], GAP_EXTENTION + X[i][k-1]);
            if(GAP_OPEN+W[i][k-1] >= GAP_EXTENTION+X[i][k-1])
            {
                X[i][k] = GAP_OPEN + W[i][k-1];
                XtrackBack[i][k] = 'W';
            }else
            {
                X[i][k] = GAP_EXTENTION + X[i][k-1];
                XtrackBack[i][k] = 'X';  // -->
            }
            int rightCost = X[i][k];

            //Y[i][k] = maximum(GAP_OPEN + W[i-1][k], GAP_EXTENTION + Y[i-1][k]);
            if(GAP_OPEN + W[i-1][k] >= GAP_EXTENTION + Y[i-1][k])
            {
                Y[i][k] = GAP_OPEN + W[i-1][k];
                YtrackBack[i][k] = 'W';
            }else
            {
                Y[i][k] = GAP_EXTENTION + Y[i-1][k];
                YtrackBack[i][k] = 'Y';
            }
            int downCost = Y[i][k];

            /*if (i == 3 && k == 6)
            {
                cout << "$$$$$$$$$$$$" << endl;
                cout << W[i-1][k-1] << "\t" << W[i][k] << endl;
                cout << matrix[dict[S1_k]][dict[S2_i]] << endl;
            }*/

            if (matchingCost >= rightCost && matchingCost >= downCost)
            {
                score[i][k] = matchingCost;
                //WtrackBack[i][k] = 'W';
            }else if(rightCost >= downCost)
            {
                score[i][k] = rightCost;
                //WtrackBack[i][k] = 'X';
            }else
            {
                score[i][k] = downCost;
                //WtrackBack[i][k] = 'Y';
            }
            if (score[i][k] < 0)
            {
                score[i][k] = 0;
                //W[i][k] = 0;
                //X[i][k] = 0;
                //Y[i][k] = 0;
                //trackBack[i][k] = '0';
            }
            if (maxScore < score[i][k])
            {
                maxScore = score[i][k];
                maxN = i;
                maxM = k;
            }
        }
    }

    //trackBack
    int n = maxN;
    int m = maxM;
    char matrixIndex = 'Y';
    if (W[n][m] >= X[n][m] && W[n][m] >= Y[n][m])
    {
        matrixIndex = 'W';
    }else if(X[n][m] >= Y[n][m])
    {
        matrixIndex = 'X';
    }
    //cout << XtrackBack[14][16] << endl;
    //bool gothought0 = false;
    while(score[n][m] > 0)
    {
        //trackBack[n][m] = matrixIndex;
        if(matrixIndex == 'W')
        {
            if (WtrackBack[n][m] == 'W')
            {
                matrixIndex = 'W';
            }else if(WtrackBack[n][m] == 'X')
            {

                matrixIndex = 'X';
            }else if(WtrackBack[n][m] == 'Y')
            {
                matrixIndex = 'Y';
            }else if(WtrackBack[n][m] == '0')
            {
                cout << "00000000000 " << score[n-1][m-1] << endl;
            }
            R1 += S1[m-1];
            R2 += S2[n-1];
            n--;
            m--;
        }else if(matrixIndex == 'X')
        {
            if (XtrackBack[n][m] == 'W')
            {
                matrixIndex = 'W';
            }else if (XtrackBack[n][m] == 'X')
            {
                matrixIndex = 'X';
            }
            R1 += S1[m-1];
            //R2 += '-'; //MARK, if want aligning result, un-annotate this
            m--;
        }else if(matrixIndex == 'Y')
        {

            if (YtrackBack[n][m] == 'W')
            {
                matrixIndex = 'W';
            }else if (YtrackBack[n][m] == 'Y')
            {
                matrixIndex = 'Y';
            }
            //R1 += '-';   //MARK , if want aligning result, un-annotate this
            R2 += S2[n-1];
            n--;
        }

    }
    reverse(R1.begin(), R1.end());
    reverse(R2.begin(), R2.end());

    /*cout << W[14][16] <<" " << X[14][16] << " " << X[14][17] << endl;
    //for Debug
    ofstream fout ("test.txt");
    //fout << matrix[dict['D']][dict['D']] << endl;

    for (int i = 0; i <= M; i++)
    {
        if(i > 0)
        {
            fout << S1[i-1] << "\t";
        }else
        {
            fout << " \t \t";
        }
    }
    fout << endl;
    for(int i = 0; i <= N; i++)
    {
        if(i > 0)
        {
            fout << S2[i-1] << "\t";
        }else
        {
            fout << ' ' << "\t";
        }
        for (int k = 0; k <= M; k++)
        {
            fout << score[i][k] << "\t";
        }
        fout << endl;
    }
    fout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;

    for (int i = 0; i <= M; i++)
    {
        if(i > 0)
        {
            fout << S1[i-1] << "\t";
        }else
        {
            fout << " \t \t";
        }
    }
    fout << endl;
    for(int i = 0; i <= N; i++)
    {
        if(i > 0)
        {
            fout << S2[i-1] << "\t";
        }else
        {
            fout << ' ' << "\t";
        }

        for (int k = 0; k <= M; k++)
        {
            fout << trackBack[i][k] << " \t";
        }
        fout << endl;
    }
    */
    return maxScore;

}

int main()
{
    ofstream fout ("rosalind_laff.out");
    ifstream fin ("rosalind_laff.txt");

    //cout << GAP_EXTENTION << GAP_OPEN << endl;

    map<char,int> dict;
    vector<vector<int> > matrix;
    read_matrix(dict, matrix);

    int N = dict.size();

    string S1 = "";
    string S2 = "";
    string R1,R2;
    string line;
    int fastaNum = 0;
    while(fin != NULL)
    {
        getline(fin, line, '\n');
        if (line[0] == '>')
        {
            fastaNum ++;
            continue;
        }
        if(fastaNum == 1)
        {
            S1 += line;
        }else if(fastaNum == 2)
        {
            S2 += line;
        }
    }
    //cout << S1 << endl;
    //cout << S2 << endl;
    int score = smith_waterman(S1, S2, R1, R2, dict, matrix);
    fout << score << endl;
    fout << R1 << endl;
    fout << R2;
    return 0;
}