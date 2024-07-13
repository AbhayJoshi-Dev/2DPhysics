//https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf

#pragma once

#include<stack>

#include"Shape.h"
#include"Collision.h"

#define nullNode -1

struct TreeNode
{
	AABB aabb;

	int child1;
	int child2;

	int objectIndex;
	int parentIndex;

	bool IsLeaf() const { return child1 == nullNode; }

	bool moved;

	int nextIndex;

	void* userData; //for storing body pointer
};

class DynamicTree
{
public:

	DynamicTree();
	~DynamicTree();
	int AllocateNode();
	int CreateNewLeaf(const AABB& aabb, void* userData);
	void InsertLeaf(int leafId);
	void RemoveLeaf(int leafId);
	void FreeNode(int leafId);

	template <typename T>
	void Query(T* callback, const AABB& aabb) const;

	bool WasMoved(int proxyId) const;
	bool MoveProxy(int proxyId, const AABB& aabb, const Vector2& displacement);
	void ClearMoved(int proxyId);
	void* GetUserData(int nodeId) const;

	void DestroyProxy(int proxyId);
	const AABB& GetFatAABB(int proxyId) const;
	inline TreeNode* GetNodes() const { return m_nodes; }
	inline int GetNodeCount() const { return m_nodeCount; }

private:

	TreeNode* m_nodes;
	int m_nodeCount;
	int m_nodeCapacity;

	int m_rootIndex;

	int m_freeList;

};

template <typename T>
inline void DynamicTree::Query(T* callback, const AABB& aabb) const
{
	std::stack<int> stack;
	stack.push(m_rootIndex);

	while (!stack.empty())
	{
		int nodeId = stack.top();
		stack.pop();

		if (nodeId == nullNode)
			return;

		TreeNode* node = m_nodes + nodeId;

		if (Collision::AABBToAABB(node->aabb, aabb))
		{
			if (node->IsLeaf())
			{
				callback->QueryCallback(nodeId);
			}
			else
			{
				stack.push(node->child1);
				stack.push(node->child2);
			}
		}
	}
}