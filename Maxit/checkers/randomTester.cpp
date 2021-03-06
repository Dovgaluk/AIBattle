#include <sstream>
#include <iostream>
#include <ctime>
#include "execution.h"

const int size = 6;

int field[size][size];
int scores[2];
int row;
int col;

bool checkFirst(int r)
{
    for (int i = 0 ; i < size ; ++i)
        if (field[r - 1][i])
            return true;

    return false;
}

bool checkSecond(int c)
{
    for (int i = 0 ; i < size ; ++i)
        if (field[i][c - 1])
            return true;

    return false;
}

void saveField(int player)
{
    std::ostringstream outs;
    for (int i = 0 ; i < size ; ++i)
    {
        for (int j = 0 ; j < size ; ++j)
        {
            outs << field[i][j] << " ";
        }
        outs << "\n";
    }
	outs << player << " " << row << " " << col << "\n"; 
	outs << scores[0] << " " << scores[1] << "\n"; 

    printField(outs.str());
}

int main(int argc, char **argv)
{

	srand(time(NULL));

    if (argc != 3)
    {
        std::cout << "Usage: Maxit_tester <program1> <program2>\n";
        return 1;
    }
    const char *program1 = argv[1];
    const char *program2 = argv[2];

    // init field
    for (int i = 0 ; i < size ; ++i)
    {
        for (int j = 0 ; j < size ; ++j)
        {
            field[i][j] = rand() % size + 1;
        }
    }
    col = 1;
    row = 1;

    // save field and score before the first move
    saveField(1);

    bool first = true;
    ExecutionResult result = ER_OK;
    for (int move = 0 ; move < size * size ; ++move)
    {
        std::ostringstream outs;
        for (int i = 0 ; i < size ; ++i)
        {
            for (int j = 0 ; j < size ; ++j)
            {
                outs << field[i][j] << " ";
            }
            outs << "\n";
        }
		outs << !first + 1 << "\n" << (first ? row : col) << "\n"; 
        std::string output;
        result = runProcess(first ? program1 : program2, 
            outs.str(), output, 1000, 64000);
        if (result == ER_OK)
        {
            std::istringstream ins(output);
            int rowcol;
            ins >> rowcol;
            if (rowcol > 0 && rowcol <= size
                && ((first && field[row-1][rowcol-1])
                    || (!first && field[rowcol-1][col-1])))
            {

                if (first)
                    col = rowcol;
                else
                    row = rowcol;

                printLog(first, result, output);

                scores[!first] += field[row-1][col-1];
                
                field[row-1][col-1] = -field[row-1][col-1];
                // save field and score after the correct move
                saveField(!first + 1);
                field[row-1][col-1] = 0;

                // get next player
                bool canFirst = checkFirst(row);
                bool canSecond = checkSecond(col);
                if ((first && canSecond)
                    || (!first && canFirst))
                    first = !first;
                else if (canFirst)
                    first = true;
                else if (canSecond)
                    first = false;
                else 
                    break;
            }
            else
            {
                result = ER_IM;
                printLog(first, result, output);
                break;
            }
        }
        else
        {
            printLog(first, result, "");
            break;
        }
    }

    if (result == ER_OK)
    {
        if (scores[0] > scores[1])
            printLog(true, ER_WIN, "");
        else if (scores[0] < scores[1])
            printLog(false, ER_WIN, "");
        else
            printLog(false, ER_TIE, "");
    }

    return 0;
}
