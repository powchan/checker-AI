/**
 * @file player.h
 * @brief Black and White Game AI Player
 */

#include <iostream>
#include <vector>
#include <climits>
#include <cstring>
#include "../include/playerbase.h"

using namespace std;
vector<vector<char>> init_mat;
int general_score = 0;
constexpr int maxsize = 15;
int prior[maxsize][maxsize];

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

int getScoreForEv(int x, int y)
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

int Flip(Player* player, int startX, int startY, int dirX, int dirY, bool myself)
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

int doStep(Player* player, int stepX, int stepY, bool myself)
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

Player* copyPlayer(Player* player) {
    Player* new_player = new Player;
    new_player->row_cnt = player->row_cnt;
    new_player->col_cnt = player->col_cnt;

    new_player->mat = new char*[new_player->row_cnt];
    for (int i = 0; i < new_player->row_cnt; i++) {
        new_player->mat[i] = new char[new_player->col_cnt];
        memcpy(new_player->mat[i], player->mat[i], new_player->col_cnt * sizeof(char));
    }

    return new_player;
}

void freePlayer(Player* player) {
    for (int i = 0; i < player->row_cnt; i++) {
        delete[] player->mat[i];
    }
    delete[] player->mat;
    delete player;
}

bool isValid(Player* player, int posx, int posy, int maximizingPlayer) {
    if (posx < 0 || posx >= player->row_cnt || posy < 0 || posy >= player->col_cnt) {
        return false;
    }
    if (player->mat[posx][posy] == 'o' || player->mat[posx][posy] == 'O') {
        return false;
    }
    vector<pair<int, int>> step = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
    for (size_t i = 0; i < step.size(); ++i) {
        int dx = step[i].first;
        int dy = step[i].second;
        int x = posx + dx;
        int y = posy + dy;
        if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt) {
            continue;
        }
        if ((maximizingPlayer == 1 && player->mat[x][y] != 'o') || (maximizingPlayer == 0 && player->mat[x][y] != 'O')) {
            continue;
        }
        while (true) {
            x += dx;
            y += dy;
            if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt || (player->mat[x][y] >= '1' && player->mat[x][y] <= '9')) {
                break;
            }
            if ((maximizingPlayer == 1 && player->mat[x][y] == 'O') || (maximizingPlayer == 0 && player->mat[x][y] == 'o')) {
                return true;
            }
        }
    }
    return false;
}

int getStable(vector<vector<int>>& board, int current) {
    int n = board.size();
    int corner = 0; 
    int steady = 0;

    int row = n - 1;
    vector<vector<int>> corner_map = {
        {0, 0, 1, 1},
        {0, row, 1, -1},
        {row, 0, -1, 1},
        {row, row, -1, -1}
    };

    for (auto& corner_data : corner_map) {
        int corner_i = corner_data[0];
        int corner_j = corner_data[1];
        int dy = corner_data[2];
        int dx = corner_data[3];

        if (board[corner_i][corner_j] == 0) {
            corner += board[corner_i][corner_j + dx] * current * -3;
            corner += board[corner_i + dy][corner_j] * current * -3;
            corner += board[corner_i + dy][corner_j + dx] * current * -6;

            corner += board[corner_i][corner_j + 2 * dx] * current * 4;
            corner += board[corner_i + 2 * dy][corner_j] * current * 4;
            corner += board[corner_i + dy][corner_j + 2 * dx] * current * 2;
            corner += board[corner_i + 2 * dy][corner_j + dx] * current * 2;
        } else {
            int i = corner_i;
            int j = corner_j;
            int current_color = board[corner_i][corner_j];

            corner += board[corner_i][corner_j] * current * 15;

            while (i >= 0 && i <= row && board[i][corner_j] == current_color) {
                steady += current * current_color;
                i += dy;
            }
            while (j >= 0 && j <= row && board[corner_i][j] == current_color) {
                steady += current * current_color;
                j += dx;
            }
        }
    }
    if (n == 8)
        return 6 * corner + 12 * steady;
    else if (n == 10)
        return 10 * corner + 12 * steady;
    else if (n == 12)
        return 8 * corner + 12 * steady;
    else
        return 8 * corner + 12 * steady;
}

bool isFrontier(vector<vector<int>>& board, int i, int j) {
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (!(dy == 0 && dx == 0) && board[i + dy][j + dx] != 0) {
                return true;
            }
        }
    }
    return false;
}

int getFrontier(vector<vector<int>>& board, int current) {
    int n = board[0].size();
    int frontier = 0;

    for (int i = 1; i <= n - 2; i++) {
        for (int j = 1; j <= n - 2; j++) {
            if (board[i][j] != 0 && isFrontier(board, i, j)) {
                frontier -= board[i][j] * current;
            }
        }
    }
    return frontier;
}

int evaluate(Player* player, int ok_cnt) {
    vector<vector<int>> board(player->row_cnt, vector<int>(player->col_cnt, 0));
    for (int i = 0; i < player->row_cnt; i++) {
        for (int j = 0; j < player->col_cnt; j++) {
            if (player->mat[i][j] == 'O') {
                board[i][j] = getScoreForEv(i, j);
            } else if (player->mat[i][j] == 'o') {
                board[i][j] = -getScoreForEv(i, j);
            }
        }
    }

    int player_score = getStable(board, 1) + 4 * getFrontier(board, 1);

    return player_score;
}

int alphaBeta(Player* player, int depth, int alpha, int beta, int maximizingPlayer) {
    vector<Point> ok_points;
    for (int i = 0; i < player->row_cnt; i++) {
        for (int j = 0; j < player->col_cnt; j++) {
            if (isValid(player, i, j, maximizingPlayer)) {
                ok_points.push_back(initPoint(i, j));
            }
        }
    }
    if (depth == 0) {
        return evaluate(player, ok_points.size());
    }

    if (ok_points.empty()) {
        return evaluate(player, 0);
    }

    if (maximizingPlayer) {
        for (size_t i = 0; i < ok_points.size(); ++i) {
            Player* next_player = copyPlayer(player);
            doStep(next_player, ok_points[i].X, ok_points[i].Y, 1);
            int score = alphaBeta(next_player, depth - 1, alpha, beta, 0);

            alpha = max(alpha, score);

            freePlayer(next_player);

            if (beta <= alpha) {
                return beta;
            }
        }
        return alpha;
    } else {
        for (size_t i = 0; i < ok_points.size(); ++i) {
            Player* next_player = copyPlayer(player);
            doStep(next_player, ok_points[i].X, ok_points[i].Y, 0);

            int score = alphaBeta(next_player, depth - 1, alpha, beta, 1);

            beta = min(beta, score);

            freePlayer(next_player);

            if (beta <= alpha) {
                return alpha;
            }
        }
        return beta;
    }
}

void init(Player* player) {
    for (int i = 0; i < player->row_cnt; i++) {
        vector<char> temp;
        for (int j = 0; j < player->col_cnt; j++) {
            temp.push_back(player->mat[i][j]);
            if (player->mat[i][j] == 'o' || player->mat[i][j] == 'O') {
                general_score += 0;
            } else {
                general_score += (player->mat[i][j] - '0');
            }
        }
        init_mat.push_back(temp);
    }
}

Point place(Player* player) {
    vector<Point> ok_points;
    for (int i = 0; i < player->row_cnt; i++) {
        for (int j = 0; j < player->col_cnt; j++) {
            if (isValid(player, i, j, 1)) {
                ok_points.push_back(initPoint(i, j));
            }
        }
    }

    Point point = initPoint(-1, -1);
    if (!ok_points.empty()) {
        int max_score = INT_MIN;
        int score = 0;
        for (size_t i = 0; i < ok_points.size(); ++i) {
            Player* next_player = copyPlayer(player);
            doStep(next_player, ok_points[i].X, ok_points[i].Y, 1);
            score = alphaBeta(next_player, 2, INT_MIN, INT_MAX, 0);

            if (score > max_score) {
                max_score = score;
                point = ok_points[i];
            }

            freePlayer(next_player);
        }
    }

    return point;
}
