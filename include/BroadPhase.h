//https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf

#pragma once

#include<vector>

#include"MathUtils.h"
#include"DynamicTree.h"

class Shape;
class Body;
struct AABB;

struct Pair
{
	int m_proxyId1;
	int m_proxyId2;
};

class BroadPhase
{
public:
	BroadPhase();
	~BroadPhase();
	template <typename T>
	void GeneratePairs(T* callback);

	//create an initial aabb 
	int CreateProxy(const AABB& aabb, void* userData);
	void DestroyProxy(int proxyId);
	void MoveProxy(int proxyId, const AABB& aabb, const Vector2& displacement);
	bool TestOverlap(int proxyIdA, int proxyIdB) const;
	void QueryCallback(int proxyId);

	int GetPairCount() const;
	DynamicTree& GetTree() { return m_tree; }

private:

	void BufferMove(int proxyId);
	void UnBufferMove(int proxyId);

private:
	DynamicTree m_tree;
	int m_proxyCount;

	int* m_moveBuffer;
	int m_moveCapacity;
	int m_moveCount;

	Pair* m_pairBuffer;
	int m_pairCapacity;
	int m_pairCount;

	int m_queryProxyId;
};

template <typename T>
void BroadPhase::GeneratePairs(T* callback)
{
	//reset pair buffer
	m_pairCount = 0;

	//peform tree wueries for all movinf proxies
	for (int i = 0; i < m_moveCount; i++)
	{
		m_queryProxyId = m_moveBuffer[i];

		if (m_queryProxyId == -1)
			continue;

		const AABB& fatAABB = m_tree.GetFatAABB(m_queryProxyId);

		m_tree.Query(this, fatAABB);
	}

	// Send pairs to caller
	for (int i = 0; i < m_pairCount; ++i)
	{
		Pair* pair = m_pairBuffer + i;
		void* userDataA = m_tree.GetUserData(pair->m_proxyId1);
		void* userDataB = m_tree.GetUserData(pair->m_proxyId2);

		callback->AddPair(userDataA, userDataB);
	}

	// Clear move flags
	for (int i = 0; i < m_moveCount; ++i)
	{
		int proxyId = m_moveBuffer[i];
		if (proxyId == -1)
			continue;

		m_tree.ClearMoved(proxyId);
	}

	// Reset move buffer
	m_moveCount = 0;

}