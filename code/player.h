#include <iostream>
#include <string.h>
#include "../include/playerbase.h"
#include <stdio.h>
#include <vector>
#include <limits.h>
#include <memory>

#define MAX_DEPTH 10
#define maxsize 12

using namespace std;

vector<vector<char>> init_mat;
int general_score = 0;

int getScoreOfPoint(int x, int y);
bool isValid(struct Player *player, int posx, int posy);
bool isOpponentValid(struct Player *player, int posx, int posy);

int getValidPoints(struct Player *player, vector<Point> &valid_points);
int getOppoValidPoints(struct Player *player, vector<Point> &valid_points);
int doStep(struct Player *player, int stepX, int stepY, bool myself);
int alphaBeta(struct Player *player, int depth, int alpha, int beta, bool maximizingPlayer);
int evaluate(struct Player *player);
int get_stable(vector<vector<int>> board);
int get_frontier(vector<vector<int>> board);
bool is_frontier(vector<vector<int>> board, int i, int j);
bool inMat(int x, int y, int row_cnt, int col_cnt);
int Flip(struct Player *player, int startX, int startY, int dirX, int dirY, bool myself);

void init(struct Player *player)
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

struct Point place(struct Player *player)
{
	Point p;
	struct Point point = initPoint(-1, -1);
	vector<Point> valid_points;
	int valid_num = getValidPoints(player, valid_points);
	if (valid_num > 0)
	{
		int max_score = INT_MIN;
		int score = 0;
		for (int i = 0; i < valid_num; i++)
		{
			struct Player temp_player = *player;
			doStep(&temp_player, valid_points[i].X, valid_points[i].Y, 1);
			score = alphaBeta(&temp_player, 0, INT_MIN, INT_MAX, 0);
			if (score > max_score)
			{
				max_score = score;
				point = valid_points[i];
			}

		}
	}
	return point;
}

bool isValid(struct Player *player, int posx, int posy)
{
	if (posx < 0 || posx >= player->row_cnt || posy < 0 || posy >= player->col_cnt)
	{
		return false;
	}
	if (player->mat[posx][posy] == 'o' || player->mat[posx][posy] == 'O')
	{
		return false;
	}
	int step[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
	for (int dir = 0; dir < 8; dir++)
	{
		int x = posx + step[dir][0];
		int y = posy + step[dir][1];
		if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt)
		{
			continue;
		}
		if (player->mat[x][y] != 'o')
		{
			continue;
		}
		while (true)
		{
			x += step[dir][0];
			y += step[dir][1];
			if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt || (player->mat[x][y] >= '1' && player->mat[x][y] <= '9'))
			{
				break;
			}
			if (player->mat[x][y] == 'O')
			{
				return true;
			}
		}
	}
	return false;
}

bool isOpponentValid(struct Player *player, int posx, int posy)
{
	if (posx < 0 || posx >= player->row_cnt || posy < 0 || posy >= player->col_cnt)
	{
		return false;
	}
	if (player->mat[posx][posy] == 'o' || player->mat[posx][posy] == 'O')
	{
		return false;
	}
	int step[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
	for (int dir = 0; dir < 8; dir++)
	{
		int x = posx + step[dir][0];
		int y = posy + step[dir][1];
		if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt)
		{
			continue;
		}
		if (player->mat[x][y] != 'O')
		{
			continue;
		}
		while (true)
		{
			x += step[dir][0];
			y += step[dir][1];
			if (x < 0 || x >= player->row_cnt || y < 0 || y >= player->col_cnt || (player->mat[x][y] >= '1' && player->mat[x][y] <= '9'))
			{
				break;
			}
			if (player->mat[x][y] == 'o')
			{
				return true;
			}
		}
	}
	return false;
}

int getValidPoints(struct Player *player, vector<Point> &valid_points)
{
	for (int i = 0; i < player->row_cnt; i++)
	{
		for (int j = 0; j < player->col_cnt; j++)
		{
			if (isValid(player, i, j))
			{
				valid_points.push_back(initPoint(i, j));
			}
		}
	}
	return valid_points.size();
}

int getOppoValidPoints(struct Player *player, vector<Point> &valid_points)
{
	for (int i = 0; i < player->row_cnt; i++)
	{
		for (int j = 0; j < player->col_cnt; j++)
		{
			if (isOpponentValid(player, i, j))
			{
				valid_points.push_back(initPoint(i, j));
			}
		}
	}
	return valid_points.size();
}

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

bool inMat(int x, int y, int row_cnt, int col_cnt)
{
	return (x >= 0 && x < row_cnt && y >= 0 && y < col_cnt);
}

int Flip(struct Player *player, int startX, int startY, int dirX, int dirY, bool myself)
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

int doStep(struct Player *player, int stepX, int stepY, bool myself)
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

int get_stable(vector<vector<int>> board)
{
	int n = board[0].size();
	int corner = 0;
	int steady = 0;

	int row = n - 1;
	int corner_map[4][4] =
		{
			{0, 0, 1, 1},
			{0, row, 1, -1},
			{row, 0, -1, 1},
			{row, row, -1, -1}};

	for (int c = 0; c < 4; c++)
	{
		int corner_i = corner_map[c][0];
		int corner_j = corner_map[c][1];
		int dy = corner_map[c][2];
		int dx = corner_map[c][3];

		if (board[corner_i][corner_j] == 0)
		{
			corner += board[corner_i][corner_j + dx] * -3;
			corner += board[corner_i + dy][corner_j]  * -3;
			corner += board[corner_i + dy][corner_j + dx]  * -6;
			corner += board[corner_i][corner_j + 2 * dx]  * 4;
			corner += board[corner_i + 2 * dy][corner_j]  * 4;
			corner += board[corner_i + dy][corner_j + 2 * dx]  * 2;
			corner += board[corner_i + 2 * dy][corner_j + dx] * 2;
		}
		else
		{
			int i = corner_i;
			int j = corner_j;
			int current_color = board[corner_i][corner_j];

			corner += board[corner_i][corner_j]  * 15;

			while (i >= 0 && i <= row && board[i][corner_j] == current_color)
			{
				steady +=  current_color;
				i += dy;
			}
			while (j >= 0 && j <= row && board[corner_i][j] == current_color)
			{
				steady +=  current_color;
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

bool is_frontier(vector<vector<int>> board, int i, int j)
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

int get_frontier(vector<vector<int>> board)
{
	int n = board[0].size();
	int frontier = 0;

	for (int i = 1; i <= n - 2; i++)
	{
		for (int j = 1; j <= n - 2; j++)
		{
			if (board[i][j] != 0 && is_frontier(board, i, j))
			{
				frontier -= board[i][j];
			}
		}
	}
	return frontier;
}

int evaluate(struct Player *player)
{
	vector<vector<int>> board(player->row_cnt, vector<int>(player->col_cnt, 0));
	for (int i = 0; i < player->row_cnt; i++)
	{
		for (int j = 0; j < player->col_cnt; j++)
		{
			if (player->mat[i][j] == 'O')
			{
				board[i][j] = getScoreOfPoint(i, j);
			}
			else if (player->mat[i][j] == 'o')
			{
				board[i][j] = -getScoreOfPoint(i, j);
			}
			else
			{
				board[i][j] = 0;
			}
		}
	}
	int player_score = get_stable(board) + 4 * get_frontier(board);

	return player_score;
}

int alphaBeta(struct Player *player, int depth, int alpha, int beta, bool maximizingPlayer)
{
	if (depth == MAX_DEPTH)
	{
		return evaluate(player);
	}

	vector<Point> valid_points;
	int step_num = maximizingPlayer ? getValidPoints(player, valid_points) : getOppoValidPoints(player, valid_points);
	if (step_num <= 0)
	{
		return evaluate(player);
	}

	if (maximizingPlayer)
	{
		int max_score = INT_MIN;
		for (int i = 0; i < step_num; i++)
		{
			auto temp_player = *player;
			Point temp_coor;
			doStep(&temp_player, valid_points[i].X, valid_points[i].Y, true);
			int temp_score = alphaBeta(&temp_player, depth + 1, alpha, beta, false);
			alpha = max(alpha, temp_score);
			if (beta <= alpha)
			{
				return beta;
			}
		}
		return alpha;
	}
	else
	{
		int min_score = INT_MAX;
		for (int i = 0; i < step_num; i++)
		{
			auto temp_player = *player;
			Point temp_coor;
			doStep(&temp_player, valid_points[i].X, valid_points[i].Y, false);
			int temp_score = alphaBeta(&temp_player, depth + 1, alpha, beta, true);
			beta = min(beta, temp_score);
			if (beta <= alpha)
			{
				return alpha;
			}
		}
		return beta;
	}
}
