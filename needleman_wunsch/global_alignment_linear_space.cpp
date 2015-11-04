#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

#define GAP_PENALTY -5
#define MINUS_INFINITY -1000000

using namespace std;

//global variables, which can be put in a Class
//string S1 = "";
//string S2 = "";
map<char,int> dict;
vector<vector<int> > matrix;


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

vector<vector<int> > &read_matrix()
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

// we still need this unchanged for basic conditions.
vector<string> needleman_wunsch(string S1, string S2, int &bestScore)
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
    string R1 = "";
    string R2 = "";
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
    bestScore = score[N][M];

    vector<string> alignment;
    alignment.push_back(R1);
    alignment.push_back(R2);
    return alignment;
}

// directly use Paul's idea
vector<int> all_y_prefix_costs(string x, int index, string y)
{
    int N = y.length();
    int * firstColumn = new int [N+1];
    for (int i = 0; i <= N; i++)
    {
        firstColumn[i] = i*GAP_PENALTY;
    }
    int * secondColumn = new int [N+1];
    secondColumn[0] = GAP_PENALTY;
    //calculate column by column
    vector<int> y_prefix;
    //y_prefix.push_back(0);
    for (int i = 1; i <= index+1; i++)
    {
        //cout << x[i-1];
        for (int k = 1; k <= N; k++ )
        {
            char x_i = x[i-1];
            char y_k = y[k-1];

            int matchingCost = firstColumn[k-1] + matrix[dict[y_k]][dict[x_i]];
            int rightCost = firstColumn[k] + GAP_PENALTY;
            int downCost = secondColumn[k-1] + GAP_PENALTY;
            if (matchingCost >= rightCost && matchingCost >= downCost)
            {
                secondColumn[k] = matchingCost;
            }else if(rightCost >= downCost)
            {
                secondColumn[k] = rightCost;
            }else
            {
                secondColumn[k] = downCost;
            }
        }
        for (int h = 0; h <= N; h++)
        {
            firstColumn[h] = secondColumn[h];
            if( i == index+1)
            {
                y_prefix.push_back(secondColumn[h]);
            }
        }
        secondColumn[0] += GAP_PENALTY;
    }
    //cout << endl;
    return y_prefix;
}

vector<int> all_y_suffix_costs(string x, int index, string y)
{
    reverse(x.begin(), x.end());
    reverse(y.begin(), y.end());
    string x_prime = x;
    string y_prime = y;
    int index_prime = x.length() - index - 2;
    vector<int> temp_prefix = all_y_prefix_costs(x_prime, index_prime, y_prime);
    vector<int> y_suffix;
    for(int i = temp_prefix.size()-1; i >= 0; i--)
    {
        y_suffix.push_back(temp_prefix[i]);
    }
    return y_suffix;
}

// we return alignment result in a string vector and change bestScore inside function
vector<string> hirschberg(string x, string y, int &bestScore)
{
    //cout << "call hirschberg :\n" << x << "\n" << y << endl;
    string alignmentX = "";
    string alignmentY = "";

    vector<string> alignment;

    int m = x.length();
    int n = y.length();

    if (m == 0 || n == 0)
    {
        if (m == 0)
        {
            for(int i = 0; i < n; i++)
            {
                alignmentX = alignmentX + '-';
                alignmentY = alignmentY + y[i];
                bestScore += GAP_PENALTY;
            }
        }else if (n == 0)
        {
            for(int i = 0; i < m; i++)
            {
                alignmentX += x[i];
                alignmentY += '-';
                bestScore += GAP_PENALTY;
            }
        }
    }else if (m <= 2 or n <= 2)
    {
        // if first time, we will change bestScore here.
        return needleman_wunsch(x,y, bestScore);
    }else
    {
        int middle = (int)(m/2)-1; // start from 0
        //string index start from 1
        vector<int> yPrefix = all_y_prefix_costs(x, middle, y);
        vector<int> ySuffix = all_y_suffix_costs(x, middle, y);

        /*for(int i = 0; i < yPrefix.size(); i++)
        {
            cout << yPrefix[i] << " ";
        }
        cout << endl;

        for(int i = 0; i < ySuffix.size(); i++)
        {
            cout << ySuffix[i] << " ";
        }
        cout << endl;
        */
        // we can get best score here
        int best = MINUS_INFINITY;
        int bestq = 0;
        for (int q = 0; q <= n; q++)
        {
            int v1 = yPrefix[q];
            int v2 = ySuffix[q];
            //cout << q << " " << v1 << " " << v2 << " " << v1+v2 << " " << best << " " << endl;
            // prefix and suffix all start from 0 and end at n, but they already have 1 deviation.
            if (v1+v2 >= best)
            {
                bestq = q;
                best = v1+v2;
            }
        }
        bestScore = best;
        int tempScore = 0; // no meaning, just for parameter...
        //cout << "bestq " << bestq << endl;
        vector<string> leftAlignment = hirschberg(x.substr(0, middle+1), y.substr(0,bestq), tempScore);
        vector<string> rightAlignment = hirschberg(x.substr(middle+1, m-middle), y.substr(bestq, n-bestq), tempScore);

        alignmentX = leftAlignment[0] + rightAlignment[0];
        alignmentY = leftAlignment[1] + rightAlignment[1];
    }
    alignment.push_back(alignmentX);
    alignment.push_back(alignmentY);
    return alignment;
    // return alignment1 and alignment2 in a vector
}

int main()
{
    ofstream fout ("rosalind_5l.out");
    ifstream fin ("rosalind_5l.txt");

    read_matrix();

    int N = dict.size();

    //string S1 = "";
    //string S2 = "";
    /*string line;
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
    }*/
    string x = "";
    string y = "";
    fin >> x;
    fin >> y;
    int bestScore = 0;
    vector<string> alignment = hirschberg(x, y, bestScore);
    //cout << S1 << endl;
    //cout << S2 << endl;

    //needleman_wunsch(S1, S2);

    fout << bestScore << endl;
    fout << alignment[0] << endl;
    fout << alignment[1];
    return 0;
}