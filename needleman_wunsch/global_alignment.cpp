#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

#define GAP_PENALTY -5

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

// Needleman Wunsch Initialization
void initialize_score_matrix(int **score, char **trackBack, int M, int N)
{
    for (int i = 0; i < M+1; i++)
    {
        score[0][i] = i * GAP_PENALTY;
        trackBack[0][i] = '-';
    }

    for (int i = 0; i < N+1; i++)
    {
        score[i][0] = i * GAP_PENALTY;
        trackBack[i][0] = '|';
    }

    trackBack[0][0] = '*';
}

int needleman_wunsch(string S1, string S2, string &R1, string &R2, map<char,int> &dict, vector<vector<int> > &matrix)
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
    }
    R1 = "";
    R2 = "";
    initialize_score_matrix(score, trackBack, M, N);

    for (int i = 1; i <=N; i++)
    {
        for (int k = 1; k <= M; k++)
        {
            char S1_k = S1[k-1];
            char S2_i = S2[i-1];
            int matchingCost = score[i-1][k-1] + matrix[dict[S1_k]][dict[S2_i]];
            int rightCost = score[i][k-1] + GAP_PENALTY;
            int downCost = score[i-1][k] + GAP_PENALTY;
            if (matchingCost >= rightCost && matchingCost >= downCost)
            {
                score[i][k] = matchingCost;
                trackBack[i][k] = '*';
            }else if(rightCost >= downCost)
            {
                score[i][k] = rightCost;
                trackBack[i][k] = '-';
            }else
            {
                score[i][k] = downCost;
                trackBack[i][k] = '|';
            }
        }
    }

    //trackBack
    int n = N;
    int m = M;
    while(n > 0 || m > 0)
    {
        if (trackBack[n][m] == '*')
        {
            R1 += S1[m-1];
            R2 += S2[n-1];
            n--;
            m--;
        }else if(trackBack[n][m] == '-')
        {
            R1 += S1[m-1];
            R2 += '-';
            m--;
        }else if(trackBack[n][m] == '|')
        {
            R1 += '-';
            R2 += S2[n-1];
            n--;
        }
    }
    reverse(R1.begin(), R1.end());
    reverse(R2.begin(), R2.end());
    return score[N][M];
}

int main()
{
    ofstream fout ("rosalind_glob.out");
    ifstream fin ("rosalind_glob.txt");

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
    int score = needleman_wunsch(S1, S2, R1, R2, dict, matrix);
    cout << score << endl;
    //fout << R1 << endl;
    //fout << R2 << endl;
    return 0;
}