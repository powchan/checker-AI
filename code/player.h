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
 * @param[out] player 玩家状态，包括地图、得分
 * @return 此步增加的得分
 */
int doStep(struct Player *&player, int stepX, int stepY);

/**
 * 进行限制深度的深度优先搜索
 * @param[in] depth 当前深度，初始为0
 * @param[in] depth_limit 深度限制
 * @return 当前最优落子点
 */
Point dfs(struct Player *player, int depth, int depth_limit);

void init(struct Player *player)
{
	// This function will be executed at the begin of each game, only once.
}

struct Point place(struct Player *player)
{

	return initPoint(-1, -1); // give up
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
}

/**
 * 下一步棋，计算下棋的结果
 * @param[in] stepX 落子点的x坐标
 * @param[in] stepY 落子点的y坐标
 * @param[out] player 玩家状态，包括地图、得分
 * @return 此步增加的得分
 */
int doStep(struct Player *&player, int stepX, int stepY)
{
}

/**
 * 进行限制深度的深度优先搜索
 * @param[in] depth 当前深度，初始为0
 * @param[in] depth_limit 深度限制
 * @return 当前最优落子点
 */
Point dfs(struct Player *player, int depth, int depth_limit)
{
}