//https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf

#include"BroadPhase.h"
#include"Shape.h"
#include"Collision.h"

BroadPhase::BroadPhase()
{
	m_moveCapacity = 16;
	m_moveCount = 0;
	//allocate the move buffer on the heap
	m_moveBuffer = new int[m_moveCapacity * sizeof(int)];

	m_pairCount = 0;
	m_pairCapacity = 16;
	m_pairBuffer = new Pair[m_pairCapacity * sizeof(Pair)];

}

BroadPhase::~BroadPhase()
{
	delete[] m_moveBuffer;
	delete[] m_pairBuffer;
}

int BroadPhase::CreateProxy(const AABB& aabb, void* userData)
{
	int proxyid = m_tree.CreateNewLeaf(aabb, userData);
	m_proxyCount++;
	BufferMove(proxyid);
	return proxyid;
}

void BroadPhase::BufferMove(int proxyId)
{
	//check if movebuffer is full of not
	if (m_moveCount == m_moveCapacity)
	{
		//save the address of the move buffer to free it later
		int* oldBuffer = m_moveBuffer;

		//double the capacity
		m_moveCapacity *= 2;

		//re-allocate the buffer
		m_moveBuffer = new int[m_moveCapacity * sizeof(int)];

		//copy the memory of oldbuffer to the newly allocated memory upto the movecount
		memcpy(m_moveBuffer, oldBuffer, m_moveCount * sizeof(int));
		 
		//free the old buffer memory
		delete [] oldBuffer;
	}
	//save the proxyid to the movebuffer
	m_moveBuffer[m_moveCount] = proxyId;
	m_moveCount++;
}

void BroadPhase::DestroyProxy(int proxyId)
{
	//make the proxy null
	UnBufferMove(proxyId);
	m_moveCount--;
	//destroy from tree
	m_tree.DestroyProxy(proxyId);
}

void BroadPhase::UnBufferMove(int proxyId)
{
	for (int i = 0; i < m_moveCount; i++)
	{
		if (m_moveBuffer[i] == proxyId)
		{
			//null proxy
			m_moveBuffer[i] = -1;
		}
	}
}

void BroadPhase::MoveProxy(int proxyId, const AABB& aabb, const Vector2& displacement)
{
	//extend the aabb and add to the proxies to the buffer if the aabb doesn't contain the object
	bool buffer = m_tree.MoveProxy(proxyId, aabb, displacement);
	if (buffer)
		BufferMove(proxyId);
}

void BroadPhase::QueryCallback(int proxyId)
{
	//proxy cannot pair with itself
	if (proxyId == m_queryProxyId)
		return;

	//check if both proxies are moving? avoid duplicate pairs
	if (proxyId > m_queryProxyId && m_tree.WasMoved(proxyId))
		return;

	if (m_pairCount == m_pairCapacity)
	{
		//save the address of pair buffer to free it later
		Pair* oldBuffer = m_pairBuffer;

		//double the capacity
		m_pairCapacity *= 2;

		//re-allocate the buffer
		m_pairBuffer = new Pair[m_pairCapacity * sizeof(Pair)];

		//copy the memory of oldbuffer to the newly allocated memory upto the movecount
		memcpy(m_pairBuffer, oldBuffer, m_pairCount * sizeof(Pair));

		//free the old buffer memory
		delete [] oldBuffer;
	}

	m_pairBuffer[m_pairCount].m_proxyId1 = std::min(proxyId, m_queryProxyId);
	m_pairBuffer[m_pairCount].m_proxyId2 = std::max(proxyId, m_queryProxyId);

	m_pairCount++;
}

int BroadPhase::GetPairCount() const
{
	return m_pairCount;
}

bool BroadPhase::TestOverlap(int proxyIdA, int proxyIdB) const
{
	const AABB& aabbA = m_tree.GetFatAABB(proxyIdA);
	const AABB& aabbB = m_tree.GetFatAABB(proxyIdB);
	return Collision::AABBToAABB(aabbA, aabbB);
}