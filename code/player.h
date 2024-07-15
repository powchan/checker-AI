/**
 * @file player.h
 * @author yangboyang@jisuanke.com
 * @copyright jisuanke.com
 * @date 2021-07-01
 */

#include <iostream>

#include <string.h>
#include "../include/playerbase.h"
#include <stdio.h>
#include <vector>
#include <limits.h>
#include <memory>

#define MAX_DEPTH 20

using namespace std;

vector<vector<char>> init_mat;

int getScoreOfPoint(int x, int y);
bool isValid(struct Player *player, int posx, int posy);		 // 获取posx,posy处是否是合法落子点
bool isOpponentValid(struct Player *player, int posx, int posy); // 获取对对方来说posx,posy处是否是合法落子点

/**
 * 获取当前所有可落子的点
 * @param[out] valid_points 可落子的点的位置的集合
 * @return 可落子的点的数目
 */
int getValidPoints(struct Player *player, vector<Point> &valid_points);

/**
 * 获取对方当前所有可落子的点
 * @param[out] valid_points 可落子的点的位置的集合
 * @return 可落子的点的数目
 */
int getOppoValidPoints(struct Player *player, vector<Point> &valid_points);

/**
 * 下一步棋，计算下棋的结果
 * @param[in] stepX 落子点的x坐标
 * @param[in] stepY 落子点的y坐标
 * @param[in] myself true为自己落子，否则为对方落子
 * @param[out] player 玩家状态，包括地图、得分
 * @return 此步增加的得分
 */
int doStep(struct Player *player, int stepX, int stepY, bool myself);

/**
 * 进行限制深度的深度优先搜索
 * @param[in] depth 当前深度，初始为0
 * @param[in] depth_limit 深度限制
 * @return 当前最优落子点
 */
int dfs(struct Player *player, int depth, int depth_limit, Point &coor);

void init(struct Player *player)
{
	// This function will be executed at the begin of each game, only once.
	for (int i = 0; i < player->row_cnt; i++)
	{
		vector<char> temp;
		for (int j = 0; j < player->col_cnt; j++)
		{
			temp.push_back(player->mat[i][j]);
		}
		init_mat.push_back(temp);
	}
}

struct Point place(struct Player *player)
{
	Point p;
	dfs(player, 0, MAX_DEPTH, p);
	return p;
	// return initPoint(-1, -1); // give up
}

bool isStable(Player *player, int x, int y, char playerChar)
{
	if (player->mat[x][y] != playerChar)
		return false;

	// 检查是否在边界上
	if (x == 0 || x == player->row_cnt - 1 || y == 0 || y == player->col_cnt - 1)
	{
		return true;
	}

	// 检查是否被同色棋子包围
	if ((x > 0 && player->mat[x - 1][y] == playerChar) &&
		(x < player->row_cnt - 1 && player->mat[x + 1][y] == playerChar) &&
		(y > 0 && player->mat[x][y - 1] == playerChar) &&
		(y < player->col_cnt - 1 && player->mat[x][y + 1] == playerChar))
	{
		return true;
	}

	return false;
}

int calculateStablePieces(Player *player, char playerChar)
{
	int stable_pieces = 0;
	for (int i = 0; i < player->row_cnt; i++)
	{
		for (int j = 0; j < player->col_cnt; j++)
		{
			if (isStable(player, i, j, playerChar))
			{
				stable_pieces += getScoreOfPoint(i, j);
			}
		}
	}
	return stable_pieces;
}

int calculateEdgePieces(Player *player, char playerChar)
{
	int edge_pieces = 0;
	for (int i = 0; i < player->col_cnt -1; i++)
	{
		if (player->mat[0][i] == playerChar)
			edge_pieces += getScoreOfPoint(0, i);
		if (player->mat[player->row_cnt - 1][i] == playerChar)
			edge_pieces += getScoreOfPoint(player->row_cnt - 1, i);
	}
	for (int i = 0; i < player->row_cnt - 1; i++)
	{
		if (player->mat[i][0] == playerChar)
			edge_pieces += getScoreOfPoint(i, 0);
		if (player->mat[i][player->col_cnt - 1] == playerChar)
			edge_pieces += getScoreOfPoint(i, player->row_cnt - 1);
	}
	return edge_pieces;
}

int calculateCornerPieces(Player *player, char playerChar)
{
	int corner_pieces = 0;
	if (player->mat[0][0] == playerChar)
		corner_pieces += getScoreOfPoint(0, 0);
	if (player->mat[0][player->col_cnt - 1] == playerChar)
		corner_pieces += getScoreOfPoint(0, player->row_cnt - 1);
	if (player->mat[player->row_cnt - 1][0] == playerChar)
		corner_pieces += getScoreOfPoint(player->row_cnt - 1, 0);
	if (player->mat[player->row_cnt - 1][player->col_cnt - 1] == playerChar)
		corner_pieces += getScoreOfPoint(player->row_cnt - 1, player->row_cnt - 1);
	return corner_pieces;
}

int calculatePotentialMoves(Player *player, bool is_your_turn)
{
	vector<Point> try_places;
	int potential_moves = is_your_turn ? getValidPoints(player, try_places) : getOppoValidPoints(player, try_places);
	return potential_moves;
}

int evaluate(Player *player)
{
	int score = (player->your_score - player->opponent_score);

	// 计算稳定子、边缘子、角落子数量
	int stable_pieces = calculateStablePieces(player, 'O') - calculateStablePieces(player, 'o');
	int edge_pieces = calculateEdgePieces(player, 'O') - calculateEdgePieces(player, 'o');
	int corner_pieces = calculateCornerPieces(player, 'O') - calculateCornerPieces(player, 'o');

	// 计算我方和对方潜在行动数量
	int your_potential_moves = calculatePotentialMoves(player, true);
	int opponent_potential_moves = calculatePotentialMoves(player, false);

	// 综合所有因素计算最终得分
	const int mut = 5;
	score += stable_pieces * mut * 2;
	score += edge_pieces * mut;
	score += corner_pieces * mut * 4;
	score += your_potential_moves * 2;
	score -= opponent_potential_moves * 2;

	return score;
}

bool isValid(struct Player *player, int posx, int posy) // 获取posx,posy处是否是合法落子点
{
	if (posx < 0 || posx >= player->row_cnt || posy < 0 || posy >= player->col_cnt)
	{
		return false;
	}
	if (player->mat[posx][posy] == 'o' || player->mat[posx][posy] == 'O')
	{
		return false;
	}
	int step[8][2] = {0, 1, 0, -1, 1, 0, -1, 0, 1, 1, -1, -1, 1, -1, -1, 1};
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

bool isOpponentValid(struct Player *player, int posx, int posy) // 获取对对方来说posx,posy处是否是合法落子点
{
	if (posx < 0 || posx >= player->row_cnt || posy < 0 || posy >= player->col_cnt)
	{
		return false;
	}
	if (player->mat[posx][posy] == 'o' || player->mat[posx][posy] == 'O')
	{
		return false;
	}
	int step[8][2] = {0, 1, 0, -1, 1, 0, -1, 0, 1, 1, -1, -1, 1, -1, -1, 1};
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

/**
 * 获取当前所有可落子的点
 * @param[out] valid_points 可落子的点的位置的集合
 * @return 可落子的点的数目
 */
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

/**
 * 获取对方当前所有可落子的点
 * @param[out] valid_points 可落子的点的位置的集合
 * @return 可落子的点的数目
 */
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

/**
 * 下一步棋，计算下棋的结果
 * @param[in] stepX 落子点的x坐标
 * @param[in] stepY 落子点的y坐标
 * @param[out] player 玩家状态，包括地图、得分
 * @return 此步增加的得分
 */
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

/**
 * 进行限制深度的深度优先搜索
 * @param[in] depth 当前深度，初始为0
 * @param[in] depth_limit 深度限制
 * @param[out] coor 当前最优落子点
 * @return 决策树最大得分
 */
int dfs(struct Player *player, int depth, int depth_limit, Point &coor)
{
	if (depth == depth_limit)
	{
		return evaluate(player);
	}

	vector<Point> try_places;
	int step_num = (depth % 2 == 0) ? getValidPoints(player, try_places) : getOppoValidPoints(player, try_places);
	if (step_num <= 0)
	{
		if (coor.X == -1)
		{
			return evaluate(player);
		}
		else
		{
			coor = initPoint(-1, -1);
			return dfs(player, depth + 1, depth_limit, coor);
		}
	}

	if (depth % 2 == 0)
	{
		int max_score = INT_MIN;
		Point max_coor = initPoint(-1, -1);
		for (int i = 0; i < step_num; i++)
		{
			auto temp_player = *player;
			Point temp_coor;
			doStep(&temp_player, try_places[i].X, try_places[i].Y, true);
			int temp_score = dfs(&temp_player, depth + 1, depth_limit, temp_coor);
			if (temp_score > max_score)
			{
				max_score = temp_score;
				max_coor = try_places[i];
			}
		}
		coor = max_coor;
		return max_score;
	}
	else
	{
		int min_score = INT_MAX;
		Point min_coor = initPoint(-1, -1);
		for (int i = 0; i < step_num; i++)
		{
			auto temp_player = *player;
			Point temp_coor;
			doStep(&temp_player, try_places[i].X, try_places[i].Y, false);
			int temp_score = dfs(&temp_player, depth + 1, depth_limit, temp_coor);
			if (temp_score < min_score)
			{
				min_score = temp_score;
				min_coor = try_places[i];
			}
		}
		coor = min_coor;
		return min_score;
	}
}