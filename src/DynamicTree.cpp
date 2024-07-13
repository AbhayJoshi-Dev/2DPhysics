//https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf

#include"DynamicTree.h"
#include"DebugDraw.h"

#define aabbExtension 5

DynamicTree::DynamicTree()
{
    m_rootIndex = nullNode;

    m_nodeCapacity = 16;
    m_nodeCount = 0;
    //allocate the memory for nodes
    m_nodes = new TreeNode[m_nodeCapacity * sizeof(TreeNode)];
    //initialize the nodes memory with 0
    memset(m_nodes, 0, m_nodeCapacity * sizeof(TreeNode));


    // Build a linked list for the free list.
    for (int i = 0; i < m_nodeCapacity - 1; ++i)
    {
        m_nodes[i].nextIndex = i + 1;
    }
    m_nodes[m_nodeCapacity - 1].nextIndex = nullNode;
    m_freeList = 0;
}

DynamicTree::~DynamicTree()
{
    delete [] m_nodes;
}

int DynamicTree::AllocateNode()
{
    //expand the pool if necessary
    if (m_freeList == nullNode)
    {
        TreeNode* oldNodes = m_nodes;

        //double the pool size
        m_nodeCapacity *= 2;

        //reallocate the memory with double size
        m_nodes = new TreeNode[m_nodeCapacity * sizeof(TreeNode)];

        //copy the old memory
        memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(TreeNode));

        //delete the old memory
        delete [] oldNodes;

        // Build a linked list for the free list. The parent
        // pointer becomes the "next" pointer.
        for (int i = m_nodeCount; i < m_nodeCapacity - 1; ++i)
        {
            m_nodes[i].nextIndex = i + 1;
        }
        m_nodes[m_nodeCapacity - 1].nextIndex = nullNode;
        m_freeList = m_nodeCount;
    }

    int nodeId = m_freeList;
    m_freeList = m_nodes[nodeId].nextIndex;
    m_nodes[nodeId].child1 = nullNode;
    m_nodes[nodeId].child2 = nullNode;
    m_nodes[nodeId].parentIndex = nullNode;
    m_nodes[nodeId].userData = nullptr;
    m_nodes[nodeId].moved = false;
    ++m_nodeCount;

    return nodeId;
}

int DynamicTree::CreateNewLeaf(const AABB& aabb, void* userData)
{
    int nodeId = AllocateNode();

    //fatten aabb
    Vector2 r(aabbExtension, aabbExtension);
    m_nodes[nodeId].aabb.lowerBound = aabb.lowerBound - r;
    m_nodes[nodeId].aabb.upperBound = aabb.upperBound + r;
    m_nodes[nodeId].userData = userData;
    m_nodes[nodeId].moved = true;

    InsertLeaf(nodeId);

    return nodeId;
}

bool DynamicTree::MoveProxy(int proxyId, const AABB& aabb, const Vector2& displacement)
{
    //Move or Extend the combined AABB
    AABB fatAABB;
    Vector2 r(5.f, 5.f);
    fatAABB.lowerBound = aabb.lowerBound - r;
    fatAABB.upperBound = aabb.upperBound + r;


    //Predict the AABB
    float displacementMultipler = 4.f;
    Vector2 d = displacementMultipler * displacement;
    if (d.x < 0.0f)
        fatAABB.lowerBound.x += d.x;
    else
        fatAABB.upperBound.x += d.x;

    if (d.y < 0.0f)
        fatAABB.lowerBound.y += d.y;
    else
        fatAABB.upperBound.y += d.y;

    const AABB& treeAABB = m_nodes[proxyId].aabb;
    //check if old enlarged tree aabb contains the (combinrd sweep aabb/original body aabb)
    if (treeAABB.Contains(aabb))
    {
        // The old enlarged/fataabb still contains the object, but it might be too large.
        // Perhaps the object was moving fast but has stopped.
        // The huge AABB is larger than the new fat AABB.
        AABB hugeAABB;
        hugeAABB.lowerBound = fatAABB.lowerBound - 4.0f * r;
        hugeAABB.upperBound = fatAABB.upperBound + 4.0f * r;

        //check if old enlarged/fataabb is too large to contain the object 
        if (hugeAABB.Contains(treeAABB))
        {
            //it still contains the object and not too large
            return false;
        }
    }

    //remove and re-insert the aabb
    RemoveLeaf(proxyId);

    m_nodes[proxyId].aabb = fatAABB;

    InsertLeaf(proxyId);

    m_nodes[proxyId].moved = true;
    return true;
}

void DynamicTree::InsertLeaf(int leafIndex)
{
    //check is tree is empty
    if (m_nodeCount == 1)
    {
        m_rootIndex = leafIndex;
        m_nodes[m_rootIndex].parentIndex = nullNode;
        return;
    }

    //std::cout << leafIndex << std::endl;

   // if (leafIndex == 2)
    {
    //   __debugbreak();
    }


    // Stage 1: find the best sibling for the new leaf

    //Surface Area Heuristic 
    //Branch and Bound Algorithm
    AABB leafAABB = m_nodes[leafIndex].aabb;
    int index = m_rootIndex;

    //check if the index node is a leaf or not
    while (!m_nodes[index].IsLeaf())
    {
        //if index node is not a leaf

        //store childs
        int child1 = m_nodes[index].child1;
        int child2 = m_nodes[index].child2;

        //calculate parameter of index aabb
        float area = m_nodes[index].aabb.GetPerimeter();

        //combine both(index and leaf) aabb and calculate perimeter
        AABB combinedAABB;
        combinedAABB.Union(m_nodes[index].aabb, leafAABB);
        float combinedArea = combinedAABB.GetPerimeter();

        // Cost of creating a new parent for this node and the new leaf
        float cost = 2.0f * combinedArea;

        // Minimum cost of pushing the leaf further down the tree
        float inheritanceCost = 2.0f * (combinedArea - area);

        //cost of descending into child1
        float cost1;
        if (m_nodes[child1].IsLeaf())
        {
            AABB aabb;
            aabb.Union(leafAABB, m_nodes[child1].aabb);
            cost1 = aabb.GetPerimeter() + inheritanceCost;
        }
        else
        {
            AABB aabb;
            aabb.Union(leafAABB, m_nodes[child1].aabb);
            float oldArea = m_nodes[child1].aabb.GetPerimeter();
            float newArea = aabb.GetPerimeter();

            cost1 = (newArea - oldArea) + inheritanceCost;
        }

        //cost of descending into child1
        float cost2;
        if (m_nodes[child2].IsLeaf())
        {
            AABB aabb;
            aabb.Union(leafAABB, m_nodes[child2].aabb);
            cost2 = aabb.GetPerimeter() + inheritanceCost;
        }
        else
        {
            AABB aabb;
            aabb.Union(leafAABB, m_nodes[child2].aabb);
            float oldArea = m_nodes[child2].aabb.GetPerimeter();
            float newArea = aabb.GetPerimeter();
            cost2 = (newArea - oldArea) + inheritanceCost;
        }

        //Descend according to the minimum cost
        //break if cost of creating a new parent is smaller than descending into the childs
        if (cost < cost1 && cost < cost2)
            break;

        //Descend
        if (cost1 < cost2)
            index = child1;
        else
            index = child2;
    }

    int sibling = index;

    //Stage 2: Create a new node as a parent

    int oldParent = m_nodes[sibling].parentIndex;
    int newParent = AllocateNode();
    m_nodes[newParent].parentIndex = oldParent;
    m_nodes[newParent].userData = nullptr;
    m_nodes[newParent].aabb.Union(leafAABB, m_nodes[sibling].aabb);

    if (oldParent != nullNode)
    {
        //the sibling was not the root

        //add the new node into the child of old parent
        if (m_nodes[oldParent].child1 == sibling)
            m_nodes[oldParent].child1 = newParent;
        else
            m_nodes[oldParent].child2 = newParent;

        //set childs of new node
        m_nodes[newParent].child1 = sibling;
        m_nodes[newParent].child2 = leafIndex;

        //set the new node as a parent for sibling and inserting node
        m_nodes[sibling].parentIndex = newParent;
        m_nodes[leafIndex].parentIndex = newParent;
    }
    else
    {
        // The sibling was the root

        //set childs of new node
        m_nodes[newParent].child1 = sibling;
        m_nodes[newParent].child2 = leafIndex;

        //set the new node as a parent for sibling and inserting node
        m_nodes[sibling].parentIndex = newParent;
        m_nodes[leafIndex].parentIndex = newParent;

        m_rootIndex = newParent;
    }

    //Stage 3:: walk back up the tree refitting AABBs

    index = m_nodes[leafIndex].parentIndex;
    while (index != nullNode)
    {
        int child1 = m_nodes[index].child1;
        int child2 = m_nodes[index].child2;

        m_nodes[index].aabb.Union(m_nodes[child1].aabb, m_nodes[child2].aabb);
        index = m_nodes[index].parentIndex;
    }
}

void DynamicTree::RemoveLeaf(int leafId)
{
    if (leafId == m_rootIndex)
    {
        m_rootIndex = nullNode;
        return;
    }

    int parent = m_nodes[leafId].parentIndex;
    int grandParent = m_nodes[parent].parentIndex;
    int sibling = m_nodes[parent].child1 == leafId ? m_nodes[parent].child2 : m_nodes[parent].child1;

    if (grandParent != nullNode)
    {
        // Destroy parent and connect sibling to grandParent.
        if (m_nodes[grandParent].child1 == parent)
        {
            m_nodes[grandParent].child1 = sibling;
        }
        else
        {
            m_nodes[grandParent].child2 = sibling;
        }
        m_nodes[sibling].parentIndex = grandParent;
        FreeNode(parent);

        // Adjust ancestor bounds.
        int index = grandParent;
        while (index != nullNode)
        {
            //index = Balance(index);

            int child1 = m_nodes[index].child1;
            int child2 = m_nodes[index].child2;

            m_nodes[index].aabb.Union(m_nodes[child1].aabb, m_nodes[child2].aabb);
            //m_nodes[index].height = 1 + b2Max(m_nodes[child1].height, m_nodes[child2].height);

            index = m_nodes[index].parentIndex;
        }
    }
    else
    {
        m_rootIndex = sibling;
        m_nodes[sibling].parentIndex = nullNode;
        FreeNode(parent);
    }
}

void DynamicTree::FreeNode(int nodeId)
{
    m_nodes[nodeId].nextIndex = m_freeList;
    m_freeList = nodeId;
    --m_nodeCount;
}

const AABB& DynamicTree::GetFatAABB(int proxyId) const
{
    return m_nodes[proxyId].aabb;
}

bool DynamicTree::WasMoved(int proxyId) const
{
    return m_nodes[proxyId].moved;
}

void DynamicTree::ClearMoved(int proxyId)
{
    m_nodes[proxyId].moved = false;
}

void DynamicTree::DestroyProxy(int proxyId)
{

}

void* DynamicTree::GetUserData(int nodeId) const
{
    return m_nodes[nodeId].userData;
}