/**
 * @file computer.h
 * @author CrazyThursday@jisuanke.com
 * @copyright jisuanke.com
 * @date 2024/7/16
 */

#include <iostream>
#include <vector>
#include <climits>
#include <cstring>
#include "../include/playerbase.h"

#define MAX_DEPTH 2 // 最大遍历深度

using namespace std;

vector<vector<char>> init_mat; // 地图初始分数
int general_score = 0;         // 地图总分数

/**
 * 获取当前所在点的分数
 * @param[in] x 点的横坐标
 * @param[in] y 点的纵坐标
 * @return 点的分数
 */
int getScoreOfPoint(int x, int y);

/**
 * 获取当前点的估值分数
 * @param[in] x 点的横坐标
 * @param[in] y 点的纵坐标
 * @return 点的分数
 */
int getScoreForEvaluate(int x, int y);

/**
 * 判断当前点是否在棋盘内
 * @param[in] x 点的横坐标
 * @param[in] y 点的纵坐标
 * @param[in] row_cnt 地图的最大行数（下标从0开始）
 * @param[in] col_cnt 棋盘的最大列数（下标从0开始）
 * @return true在棋盘内 false不在棋盘内
 */
bool inMat(int x, int y, int row_cnt, int col_cnt);

/**
 * 进行翻棋子操作，同时计算这一步下棋方的得分（不包含落子位置的得分）
 * @param[in] player 玩家信息，包括棋盘现态，双方各自的分数
 * @param[in] startX 落子的位置的横坐标
 * @param[in] startY 落子的位置的纵坐标
 * @param[in] dirX   -1表示向落子位置上方探索，0表示行不变，1表示向落子位置下方探索
 * @param[in] dirX   -1表示向落子位置左方探索，0表示行不变，1表示向落子位置右方探索
 * @param[in] myself true为我方下棋，false为对方下棋
 * @return 返回这一步得到的分数（不包含落子位置的得分）
 */
int Flip(Player *player, int startX, int startY, int dirX, int dirY, bool myself);

/**
 * 进行落子操作并探索是否有符合的棋子可以被翻转，修改双方分数
 * @param[in] player 玩家信息，包括棋盘现态，双方各自的分数
 * @param[in] stepX 落子的位置的横坐标
 * @param[in] stepY 落子的位置的纵坐标
 * @param[in] myself true为我方下棋，false为对方下棋
 * @return 返回这一步下棋方得到的分数（包含落子位置的得分）
 */
int doStep(Player *player, int stepX, int stepY, bool myself);
Player *copyPlayer(Player *player);
void freePlayer(Player *player);
bool isValid(Player *player, int posX, int posY, bool nowPlayer);
int getStable(vector<vector<int>> &board, int w_weight);
bool isFrontier(vector<vector<int>> &board, int i, int j);
int getFrontier(vector<vector<int>> &board, int w_weight);
int evaluate(Player *player);
int alphaBeta(Player *player, int depth, int alpha, int beta, bool nowPlayer);
void init(Player *player);
Point place(Player *player);

int getScoreOfPoint(int x, int y)
{
    char c = init_mat[x][y];
    if (c == 'o' || c == 'O')
    {
        return 0;
    }
    else
    {
        return c - '0';
    }
}

int getScoreForEvaluate(int x, int y)
{
    char c = init_mat[x][y];
    if (c == 'o' || c == 'O')
    {
        return 1;
    }
    else
    {
        return c - '0';
    }
}

bool inMat(int x, int y, int row_cnt, int col_cnt)
{
    return (x >= 0 && x < row_cnt && y >= 0 && y < col_cnt);
}

int Flip(Player *player, int startX, int startY, int dirX, int dirY, bool myself)
{
    char myPiece = myself ? 'O' : 'o';          //判断这一步下的是哪种棋子
    char opponentPiece = myself ? 'o' : 'O';
    int x = startX + dirX;                      //向dir决定的方向进行一次探索
    int y = startY + dirY;
    int cnt = 0;                                //记录有多少个棋子被翻转
    int cnt_score = 0;                          //记录翻转这些棋子的得分
    bool valid = false;
    
    //如果探索到的在棋盘内部且是对手的棋子，记录得分并探索下一个棋子
    while (inMat(x, y, player->row_cnt, player->col_cnt) && player->mat[x][y] == opponentPiece)
    {
        cnt++;                                  //被翻转的棋子加一
        cnt_score += getScoreOfPoint(x, y);
        x += dirX;
        y += dirY;
    }

    //确保落子点仍然是我方棋子
    if (inMat(x, y, player->row_cnt, player->col_cnt) && player->mat[x][y] == myPiece)
    {
        valid = true;
    }

    //开始对记录在案的棋子进行翻转
    if (valid && cnt > 0)
    {
        x = startX + dirX;                       //探索该方向上的下一个棋子
        y = startY + dirY;
        for (int i = 0; i < cnt; i++)            //翻转操作
        {
            player->mat[x][y] = myPiece;
            x += dirX;
            y += dirY;
        }
        return cnt_score;                        //返回除了落子点以外的得分
    }

    return 0;
}

int doStep(Player *player, int stepX, int stepY, bool myself)
{
    char myPiece = myself ? 'O' : 'o';           //判断这一步下的是哪种棋子
    player->mat[stepX][stepY] = myPiece;         //在落子位置放上我方棋子

    int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    int score = 0;

    //向落子的八个方向探索，并进行翻转
    for (int i = 0; i < 8; i++)
    {
        score += Flip(player, stepX, stepY, directions[i][0], directions[i][1], myself);
    }

    int point_score = getScoreOfPoint(stepX, stepY);//获取落子点的分数

    //我方落子引起的得分变化
    if (myself) 
    {
        player->your_score += score + point_score;
        player->opponent_score -= score;
    }
    //对方落子引起的得分变化
    else
    {
        player->opponent_score += score + point_score;
        player->your_score -= score;
    }
    return score + point_score;
}

Player *copyPlayer(Player *player)
{
    Player *new_player = new Player;
    new_player->row_cnt = player->row_cnt;
    new_player->col_cnt = player->col_cnt;

    new_player->mat = new char *[new_player->row_cnt];
    for (int i = 0; i < new_player->row_cnt; i++)
    {
        new_player->mat[i] = new char[new_player->col_cnt];
        memcpy(new_player->mat[i], player->mat[i], new_player->col_cnt * sizeof(char));
    }

    return new_player;
}

void freePlayer(Player *player)
{
    for (int i = 0; i < player->row_cnt; i++)
    {
        delete[] player->mat[i];
    }
    delete[] player->mat;
    delete player;
}

bool isValid(Player *player, int posX, int posY, bool nowPlayer)
{
    if (posX < 0 || posX >= player->row_cnt || posY < 0 || posY >= player->col_cnt)
    {
        return false;
    }
    if (player->mat[posX][posY] == 'o' || player->mat[posX][posY] == 'O')
    {
        return false;
    }
    vector<pair<int, int>> step = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
    for (size_t i = 0; i < step.size(); ++i)
    {
        int dx = step[i].first;
        int dy = step[i].second;
        int x = posX + dx;
        int y = posY + dy;
        if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt)
        {
            continue;
        }
        if ((nowPlayer && player->mat[x][y] != 'o') || (!nowPlayer && player->mat[x][y] != 'O'))
        {
            continue;
        }
        while (true)
        {
            x += dx;
            y += dy;
            if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt || (player->mat[x][y] >= '1' && player->mat[x][y] <= '9'))
            {
                break;
            }
            if ((nowPlayer && player->mat[x][y] == 'O') || (!nowPlayer && player->mat[x][y] == 'o'))
            {
                return true;
            }
        }
    }
    return false;
}

int getStable(vector<vector<int>> &board, int w_weight)
{
    int n = board.size();
    int corner_weight = 0;
    int steady_weight = 0;

    int row = n - 1;
    vector<vector<int>> corner_map = {
        {0, 0, 1, 1},
        {0, row, 1, -1},
        {row, 0, -1, 1},
        {row, row, -1, -1}};

    for (auto &corner_data : corner_map)
    {
        int corner_x = corner_data[0];
        int corner_y = corner_data[1];
        int dy = corner_data[2];
        int dx = corner_data[3];

        if (board[corner_x][corner_y] == 0)
        {
            corner_weight += board[corner_x][corner_y + dx] * w_weight * -3;
            corner_weight += board[corner_x + dy][corner_y] * w_weight * -3;
            corner_weight += board[corner_x + dy][corner_y + dx] * w_weight * -6;

            corner_weight += board[corner_x][corner_y + 2 * dx] * w_weight * 4;
            corner_weight += board[corner_x + 2 * dy][corner_y] * w_weight * 4;
            corner_weight += board[corner_x + dy][corner_y + 2 * dx] * w_weight * 2;
            corner_weight += board[corner_x + 2 * dy][corner_y + dx] * w_weight * 2;
        }
        else
        {
            int i = corner_x;
            int j = corner_y;
            int current_score = board[corner_x][corner_y];

            corner_weight += board[corner_x][corner_y] * w_weight * 15;

            while (i >= 0 && i <= row && board[i][corner_y] == current_score)
            {
                steady_weight += w_weight * current_score;
                i += dy;
            }
            while (j >= 0 && j <= row && board[corner_x][j] == current_score)
            {
                steady_weight += w_weight * current_score;
                j += dx;
            }
        }
    }
    if (n == 8)
        return 6 * corner_weight + 12 * steady_weight;
    else if (n == 10)
        return 10 * corner_weight + 12 * steady_weight;
    else if (n == 12)
        return 8 * corner_weight + 12 * steady_weight;
    else
        return 8 * corner_weight + 12 * steady_weight;
}

bool isFrontier(vector<vector<int>> &board, int i, int j)
{
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (!(dy == 0 && dx == 0) && board[i + dy][j + dx] != 0)
            {
                return true;
            }
        }
    }
    return false;
}

int getFrontier(vector<vector<int>> &board, int w_weight)
{
    int n = board[0].size();
    int frontier_weight = 0;

    for (int i = 1; i <= n - 2; i++)
    {
        for (int j = 1; j <= n - 2; j++)
        {
            if (board[i][j] != 0 && isFrontier(board, i, j))
            {
                frontier_weight -= board[i][j] * w_weight;
            }
        }
    }
    return frontier_weight;
}

int evaluate(Player *player)
{
    vector<vector<int>> board(player->row_cnt, vector<int>(player->col_cnt, 0));
    for (int i = 0; i < player->row_cnt; i++)
    {
        for (int j = 0; j < player->col_cnt; j++)
        {
            if (player->mat[i][j] == 'O')
            {
                board[i][j] = getScoreForEvaluate(i, j);
            }
            else if (player->mat[i][j] == 'o')
            {
                board[i][j] = -getScoreForEvaluate(i, j);
            }
        }
    }

    int player_score = getStable(board, 1) + 4 * getFrontier(board, 1);

    return player_score;
}

int alphaBeta(Player *player, int depth, int alpha, int beta, bool nowPlayer)
{
    vector<Point> valid_points;
    for (int i = 0; i < player->row_cnt; i++)
    {
        for (int j = 0; j < player->col_cnt; j++)
        {
            if (isValid(player, i, j, nowPlayer))
            {
                valid_points.push_back(initPoint(i, j));
            }
        }
    }
    if (depth == 0)
    {
        return evaluate(player);
    }

    if (valid_points.empty())
    {
        return evaluate(player);
    }

    if (nowPlayer)
    {
        for (size_t i = 0; i < valid_points.size(); ++i)
        {
            Player *next_player = copyPlayer(player);
            doStep(next_player, valid_points[i].X, valid_points[i].Y, 1);
            int score = alphaBeta(next_player, depth - 1, alpha, beta, false);

            alpha = max(alpha, score);

            freePlayer(next_player);

            if (beta <= alpha)
            {
                return beta;
            }
        }
        return alpha;
    }
    else
    {
        for (size_t i = 0; i < valid_points.size(); ++i)
        {
            Player *next_player = copyPlayer(player);
            doStep(next_player, valid_points[i].X, valid_points[i].Y, 0);

            int score = alphaBeta(next_player, depth - 1, alpha, beta, true);

            beta = min(beta, score);

            freePlayer(next_player);

            if (beta <= alpha)
            {
                return alpha;
            }
        }
        return beta;
    }
}

void init(Player *player)
{
    for (int i = 0; i < player->row_cnt; i++)
    {
        vector<char> temp;
        for (int j = 0; j < player->col_cnt; j++)
        {
            temp.push_back(player->mat[i][j]);
            if (player->mat[i][j] == 'o' || player->mat[i][j] == 'O')
            {
                general_score += 0;
            }
            else
            {
                general_score += (player->mat[i][j] - '0');
            }
        }
        init_mat.push_back(temp);
    }
}

Point place(Player *player)
{
    vector<Point> valid_points;
    for (int i = 0; i < player->row_cnt; i++)
    {
        for (int j = 0; j < player->col_cnt; j++)
        {
            if (isValid(player, i, j, true))
            {
                valid_points.push_back(initPoint(i, j));
            }
        }
    }

    Point point = initPoint(-1, -1);
    if (!valid_points.empty())
    {
        int max_score = INT_MIN;
        int score = 0;
        for (size_t i = 0; i < valid_points.size(); ++i)
        {
            Player *next_player = copyPlayer(player);
            doStep(next_player, valid_points[i].X, valid_points[i].Y, 1);
            score = alphaBeta(next_player, MAX_DEPTH, INT_MIN, INT_MAX, false);

            if (score > max_score)
            {
                max_score = score;
                point = valid_points[i];
            }

            freePlayer(next_player);
        }
    }

    return point;
}
