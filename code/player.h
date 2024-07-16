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


bool inMat(int x, int y, int row_cnt, int col_cnt);
int Flip(Player *player, int startX, int startY, int dirX, int dirY, bool myself);
int doStep(Player *player, int stepX, int stepY, bool myself);
Player *copyPlayer(Player *player);
void freePlayer(Player *player);

/**
 * 判断落子点是否有效
 * @param[in] player 当前棋局的状态信息
 * @param[in] posX 落子点的横坐标
 * @param[in] posY 落子点的纵坐标
 * @param[in] nowPlayer 当前是否轮到玩家
 */
bool isValid(Player *player, int posX, int posY, bool nowPlayer);
int getStable(vector<vector<int>> &board, int w_weight);
bool isFrontier(vector<vector<int>> &board, int i, int j);
int getFrontier(vector<vector<int>> &board, int w_weight);
int evaluate(Player *player);

/**
 * 进行alpha-Beta剪枝操作
 * @param[in] player 当前棋局的状态信息
 * @param[in] depth 当前搜索深度
 * @param[in] alpha alpha值，用于剪枝操作
 * @param[in] beta beta值，用于剪枝操作
 * @param[in] nowPlayer 当前是否轮到玩家
 */
int alphaBeta(Player *player, int depth, int alpha, int beta, bool nowPlayer);

/**
 * 初始化棋局状态信息并统计总分数
 * @param[in] player 当前棋局的状态信息
 */
void init(Player *player);

/**
 * 获取最优落子点的坐标
 * @param[in] player 当前棋局的状态信息
 */
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
    char myPiece = myself ? 'O' : 'o';
    char opponentPiece = myself ? 'o' : 'O';
    int x = startX + dirX;
    int y = startY + dirY;
    int cnt = 0;
    int cnt_score = 0;
    bool valid = false;

    while (inMat(x, y, player->row_cnt, player->col_cnt) && player->mat[x][y] == opponentPiece)
    {
        cnt++;
        cnt_score += getScoreOfPoint(x, y);
        x += dirX;
        y += dirY;
    }

    if (inMat(x, y, player->row_cnt, player->col_cnt) && player->mat[x][y] == myPiece)
    {
        valid = true;
    }

    if (valid && cnt > 0)
    {
        x = startX + dirX;
        y = startY + dirY;
        for (int i = 0; i < cnt; i++)
        {
            player->mat[x][y] = myPiece;
            x += dirX;
            y += dirY;
        }
        return cnt_score;
    }

    return 0;
}

int doStep(Player *player, int stepX, int stepY, bool myself)
{
    char myPiece = myself ? 'O' : 'o';
    player->mat[stepX][stepY] = myPiece;

    int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    int score = 0;
    for (int i = 0; i < 8; i++)
    {
        score += Flip(player, stepX, stepY, directions[i][0], directions[i][1], myself);
    }

    int point_score = getScoreOfPoint(stepX, stepY);
    if (myself)
    {
        player->your_score += score + point_score;
        player->opponent_score -= score;
    }
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
