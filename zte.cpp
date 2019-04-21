#include <iostream>
#include<cstdio>
#include<cstring>
#include<vector>
#include <stdlib.h>
#define INF 1000000000
using namespace std;
//存放原始拓扑信息
vector<int> gridTopo[956];
//存放所有业务需求信息
vector<int> request[4001];
vector<int> route[1000];
int g[500][500];//没有改动的邻接图
int gf[500][500];//每次迭代更新的邻接图
int gf1[500][500];//floyd更新后的邻接图
int path[500][500];
int cap[500][500];//道路容量
int cap_cur[500][500];//当前容量
void clearVec()
{
	for (int i = 0; i<956; i++) gridTopo[i].clear();
	for (int i = 0; i<4001; i++) request[i].clear();
}
void readTxt()
{
	char readLine[1000];
	const char *delim = " ";
	char *p;
	freopen("gridtopoAndRequest.txt", "r", stdin);
	freopen("result.txt", "w", stdout);
	for (int i = 0; i<956; i++)
	{
		cin.getline(readLine, 1000);
		p = strtok(readLine, delim);
		while (p)
		{
			gridTopo[i].push_back(atoi(p));
			p = strtok(NULL, delim);
		}
	}
	for (int i = 0; i<4001; i++)
	{
		cin.getline(readLine, 1000);
		p = strtok(readLine, delim);
		while (p)
		{
			request[i].push_back(atoi(p));
			p = strtok(NULL, delim);
		}
	}
}
void initGraph()
{
	for (int i = 0; i<500; i++)
	{
		for (int j = 0; j<500; j++)
		{
			g[i][j] = INF;
			cap[i][j] = 0;
			cap_cur[i][j] = 0;
			path[i][j] = j;
		}
	}
	for (int i = 1; i<956; i++)
	{
		g[gridTopo[i][0]][gridTopo[i][1]] = gridTopo[i][3];
		g[gridTopo[i][1]][gridTopo[i][0]] = gridTopo[i][3];

		cap[gridTopo[i][0]][gridTopo[i][1]] = gridTopo[i][2];
		cap[gridTopo[i][1]][gridTopo[i][0]] = gridTopo[i][2];
	}
	for (int i = 0; i < 500; i++)
	{
		for (int j = 0; j < 500; j++)
		{
			gf1[i][j] = g[i][j];
			gf[i][j] = g[i][j];
		}
	}
}
void floyd()
{

	
	for (int k = 0; k<500; k++)
	{
		for (int i = 0; i<500; i++)
		{
			for (int j = 0; j<500; j++)
			{
				if (gf1[i][j]>gf1[i][k] + gf1[k][j])
				{
					gf1[i][j] = gf1[i][k] + gf1[k][j];
					path[i][j] = k;
				}
			}
		}
	}
}
void getVisitPath(vector<int> &visitPath, int startNode, int endNode)
{
	
	int k = path[startNode][endNode];
	if (k == endNode)
	{
		return;
	}
	else
	{
		getVisitPath(visitPath, startNode, k);
		visitPath.push_back(k);
		getVisitPath(visitPath, k, endNode);
	}
}
int computeRouteCost(vector<int>r)
{
	//计算每条路的单位权重
	int sum = 0;
	for (int i = 1; i<r.size(); i++)
	{
		sum += g[r[i - 1]][r[i]];
	}
	return sum;
}
int computeCost()
{
	//计算所有路的权重和
	int cost = 0;
	for (int i = 0; i < 1000; i++)
	{
		cost += computeRouteCost(route[i])*request[i*4+1][1];
	}
	return cost;
}
bool isValid(vector<int>r, int k)
{
	//判断当前道路是否超限
	for (int i = 1; i<r.size(); i++)
	{
		if (cap_cur[r[i - 1]][r[i]] + request[k][1]>cap[r[i - 1]][r[i]] * 0.8)
			return false;
	}
	return true;
}
void updateCap(vector<int>r, int k)
{
	//更新当前道路实际容量
	for (int i = 1; i<r.size(); i++)
	{
		cap_cur[r[i - 1]][r[i]] += request[k][1];
	}
}
void showResult()
{
	//输出最后结果
	cout << computeCost() << endl;
	for (int i = 1; i<4001; i += 4)
	{
		cout << request[i][0] << " " << request[i][1] << endl;
		for (int j = 0; j<route[i / 4].size(); j++)
		{
			cout << route[i / 4][j] << " ";
		}
		cout << endl;
	}
}
void findRoad()
{
	//找到所有路径
	int startNode, endNode;
	vector<int>vPath;
	for (int i = 1; i < 4001; i += 4)
	{
		startNode = request[i + 1][0];
		endNode = request[i + 1][request[i + 1].size() - 1];
		vPath.clear();
		vPath.push_back(startNode);
		getVisitPath(vPath, startNode, endNode);
		vPath.push_back(endNode);
		route[i / 4] = vPath;
		updateCap(route[i / 4], i);
	}
}
bool checkCap()
{
	//每次寻路完成后，检查是否超限
	for (int i = 0; i < 500; i++)
	{
		for (int j = 0; j < 500; j++)
		{
			if (cap_cur[i][j] > cap[i][j]*0.8)
			{
				return false;
			}
		}
	}
	return true;
}
void updateGraph()
{
	//将超限的道路更新权重   如何跟新？策略？利用超限来反馈？
	for (int i = 0; i < 500; i++)
	{
		for (int j = 0; j < 500; j++)
		{
			if (rand()%2==1&&cap_cur[i][j] > cap[i][j] * 0.8)//理论上应该超限越多罚得越多。。
			{
				gf[i][j]++;
			}
			cap_cur[i][j] = 0;
			gf1[i][j] = gf[i][j];
			path[i][j] = j;
		}
	}
}
int main()
{
	clearVec();
	//1.输入
	readTxt();

	//2.write your code
	initGraph();
	floyd();
	findRoad();
	//第一次输出的成本是最低的？只是不满足超限？
	//迭代 直到所有道路都不超限
	while (!checkCap())
	{
		//cout << computeCost() << endl;
		updateGraph();
		floyd();
		findRoad();
		
	}
	showResult();

	return 0;
}