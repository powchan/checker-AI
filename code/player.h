/**
 * @file player.h
 * @author yangboyang@jisuanke.com
 * @copyright jisuanke.com
 * @date 2021-07-01
 */

#include <string.h>
#include "../include/playerbase.h"
#include <stdio.h>
#include <vector>
#include <limits.h>

#define MAX_DEPTH 9

using namespace std;

bool isValid(struct Player *player, int posx, int posy); // 获取posx,posy处是否是合法落子点

/**
 * 获取当前所有可落子的点
 * @param[out] valid_points 可落子的点的位置的集合
 * @return 可落子的点的数目
 */
int getVaildPoints(struct Player *player, vector<Point> &valid_points);

/**
 * 下一步棋，计算下棋的结果
 * @param[in] stepX 落子点的x坐标
 * @param[in] stepY 落子点的y坐标
 * @param[in] myself true为自己落子，否则为对方落子
 * @param[out] player 玩家状态，包括地图、得分
 * @return 此步增加的得分
 */
int doStep(struct Player *&player, int stepX, int stepY, bool myself);

/**
 * 进行限制深度的深度优先搜索
 * @param[in] depth 当前深度，初始为0
 * @param[in] depth_limit 深度限制
 * @return 当前最优落子点
 */
int dfs(struct Player *player, int depth, int& depth_limit, Point &coor);

void init(struct Player *player)
{
	// This function will be executed at the begin of each game, only once.
}

struct Point place(struct Player *player)
{
	Point p;
	dfs(player, 0, MAX_DEPTH, p);
	return p;
	// return initPoint(-1, -1); // give up
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

/**
 * 获取当前所有可落子的点
 * @param[out] valid_points 可落子的点的位置的集合
 * @return 可落子的点的数目
 */
int getVaildPoints(struct Player *player, vector<Point> &valid_points)
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
 * 下一步棋，计算下棋的结果
 * @param[in] stepX 落子点的x坐标
 * @param[in] stepY 落子点的y坐标
 * @param[in] myself true为自己落子，否则为对方落子
 * @param[out] player 玩家状态，包括地图、得分
 * @return 此步增加的得分
 */
int doStep(struct Player *&player, int stepX, int stepY, bool myself)
{
}

/**
 * 进行限制深度的深度优先搜索
 * @param[in] depth 当前深度，初始为0
 * @param[in] depth_limit 深度限制
 * @param[out] coor 当前最优落子点
 * @return 决策树最大得分
 */
int dfs(struct Player *player, int depth, int& depth_limit, Point &coor)
{
	if (depth == depth_limit)
	{
		return player->your_score - player->opponent_score;
	}

	vector<Point> try_places;
	int step_num = getVaildPoints(player, try_places);

	if (depth % 2 == 0)
	{
		int max_score = INT_MIN;
		Point max_coor = initPoint(-1, -1);
		for (int i = 0; i < step_num; i++)
		{
			auto temp_player = new struct Player;
			memcpy(temp_player, player, sizeof(struct Player));
			Point temp_coor;
			doStep(temp_player, try_places[i].X, try_places[i].Y, true);
			int temp_score = dfs(temp_player, depth + 1, depth_limit, temp_coor);
			if (temp_score > max_score)
			{
				max_score = temp_score;
				max_coor = temp_coor;
			}
			delete temp_player;
		}
		coor = max_coor;
		return max_score;
	}
	else //对方行走，可以考虑修改为随机行走
	{
		int min_score = INT_MAX;
		Point min_coor = initPoint(-1, -1);
		for (int i = 0; i < step_num; i++)
		{
			auto temp_player = new struct Player;
			memcpy(temp_player, player, sizeof(struct Player));
			Point temp_coor;
			doStep(temp_player, try_places[i].X, try_places[i].Y, false);
			int temp_score = dfs(temp_player, depth + 1, depth_limit, temp_coor);
			if (temp_score < min_score)
			{
				min_score = temp_score;
				min_coor = temp_coor;
			}
			delete temp_player;
		}
		coor = min_coor;
		return min_score;
	}
}