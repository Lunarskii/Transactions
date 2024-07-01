#ifndef TRANSACTIONS_INCLUDE_BPT_B_PLUS_TREE_TPP_
#define TRANSACTIONS_INCLUDE_BPT_B_PLUS_TREE_TPP_

#include <cassert>

namespace s21
{

template<class Key, class Tp>
BPlusTree<Key, Tp>::BPlusTree()
    : root_(new Node(true))
{}

template<class Key, class Tp>
BPlusTree<Key, Tp>::BPlusTree(size_type order)
    : order_(order)
    , min_count_keys_(std::ceil(order / 2.0) - 1)
    , max_count_keys_(order - 1)
{
    if (order_ <= 2)
    {
        throw std::invalid_argument("The order of the tree cannot be less than or equal to 2.");
    }
    root_ = new Node(true);
}

template<class Key, class Tp>
BPlusTree<Key, Tp>::~BPlusTree()
{
    Clear_(root_);
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::Insert(const key_type& key, const mapped_type& value)
{
    if (auto node = FindLeaf_(key); node->Insert(key, value))
    {
        while (node->Size() == order_)
        {
            auto [new_node, middle_key] = Split_(node);

            if (!node->IsLeaf())
            {
                node->EraseKeys(0, 1);
            }

            if (node == root_)
            {
                root_ = new Node(false);
                root_->Insert(middle_key, { new_node, node });
                new_node->SetParent(root_);
                node->SetParent(root_);
            }
            else
            {
                auto parent = node->GetParent();
                parent->Insert(middle_key, { new_node });
                node = parent;
            }
        }

        assert(Exists_(key));
        return true;
    }

    return false;
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::mapped_type& BPlusTree<Key, Tp>::GetValue(const key_type& key)
{
    auto node = FindLeaf_(key);
    auto index = node->GetKeyIndex(key);

    if (node->Exists(key, index))
    {
        return node->Values().at(index);
    }

    throw std::runtime_error("The value was not found.");
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::Erase(const key_type& key)
{
    auto [leaf_node, internal_node] = FindLeafNodeWithInternalNode_(key);

    if (leaf_node->Exists(key))
    {
        auto leaf_node_size = leaf_node->Size();

        if (leaf_node_size > min_count_keys_)
        {
            leaf_node->Erase(key);
            UpdateInternalNodeIndices(internal_node, key);
        }
        else if (leaf_node_size == min_count_keys_)
        {
            if (PossibleToBorrow_(leaf_node))
            {
                leaf_node->Erase(key);
                Borrow_(leaf_node);
                UpdateInternalNodeIndices(internal_node, key);
            }
            else
            {
                auto parent = leaf_node->GetParent();

                UnlinkFromParentNode_(leaf_node);
                UnlinkFromNeighbors_(leaf_node);
                leaf_node->Erase(key);
                if (leaf_node != root_)
                {
                    delete leaf_node;
                }

                while (parent && (parent->Children().size() != parent->Keys().size() + 1 || parent->Size() == 0))
                {
                    if (parent == root_)
                    {
                        auto new_root = parent->Children().front();
                        delete root_;
                        root_ = new_root;
                        root_->SetParent(nullptr);
                        break;
                    }

                    if (PossibleToBorrow_(parent))
                    {
                        Borrow_(parent);
                        break;
                    }
                    else
                    {
                        Merge_(parent);
                        parent = parent->GetParent();
                    }
                }

                internal_node = FindLeafNodeWithInternalNode_(key).second;
                UpdateInternalNodeIndices(internal_node, key);
            }
        }

        assert(!Exists_(key));
        return true;
    }

    return false;
}

template<class Key, class Tp>
std::vector<std::pair<typename BPlusTree<Key, Tp>::key_type, typename BPlusTree<Key, Tp>::mapped_type>>
BPlusTree<Key, Tp>::ShowAll()
{
    std::vector<std::pair<key_type, mapped_type>> entries;
    auto node = root_;

    while (!node->IsLeaf())
    {
        node = node->Children().front();
    }

    for (; node != nullptr; node = node->GetRight())
    {
        auto keys = node->Keys();
        auto values = node->Values();

        for (size_type i = 0; i < node->Size(); ++i)
        {
            entries.push_back({ keys[i], values[i] });
        }
    }

    return entries;
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::AreRelatives_(Node* first_node, Node* second_node) const noexcept
{
    return first_node && second_node && first_node->GetParent() == second_node->GetParent();
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::PossibleToBorrow_(Node* borrowing_node) const noexcept
{
    return
        PossibleToBorrow_(borrowing_node, borrowing_node->GetLeft()) ||
        PossibleToBorrow_(borrowing_node, borrowing_node->GetRight());
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::PossibleToBorrow_(Node* borrowing_node, Node* borrowed_node) const noexcept
{
    return AreRelatives_(borrowing_node, borrowed_node) && borrowed_node->Size() > min_count_keys_;
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::PossibleToMerge_(Node* first_node, Node* second_node) const noexcept
{
    return AreRelatives_(first_node, second_node) && (first_node->Size() + second_node->Size() <= max_count_keys_);
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::UpdateInternalNodeIndices(Node* internal_node, const key_type& key)
{
    if (internal_node && internal_node->Exists(key))
    {
        auto index = internal_node->GetKeyIndex(key);
        auto min_key = GetMinKey_(internal_node->Children().at(index + 1));

        internal_node->Update(key, min_key);
    }
}

template<class Key, class Tp>
std::pair<typename BPlusTree<Key, Tp>::Node*, typename BPlusTree<Key, Tp>::key_type>
BPlusTree<Key, Tp>::Split_(Node* node)
{
    static constexpr auto GetMiddleKey = [](Node* node) -> key_type
    {
        auto keys = node->Keys();
        return keys.at(keys.size() / 2);
    };
    static constexpr auto InsertNewNode = [](Node* new_node, Node* node) -> void
    {
        auto node_parent = node->GetParent();
        auto node_left = node->GetLeft();

        if (node_left != nullptr) node_left->SetRight(new_node);
        new_node->SetLeft(node_left);
        new_node->SetRight(node);
        node->SetLeft(new_node);
        new_node->SetParent(node_parent);
    };

    auto new_node = new Node(node->IsLeaf());
    auto middle_key = GetMiddleKey(node);

    InsertNewNode(new_node, node);
    node->MoveCells(new_node, middle_key);

    return { new_node, middle_key };
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::Merge_(Node* node)
{
    static constexpr auto UnlinkFromParentNode = [](Node* node, const key_type& parent_key) -> void
    {
        if (auto parent = node->GetParent(); parent)
        {
            auto [key_index, child_index] = parent->GetKeyChildIndex(parent_key, node);
            parent->EraseKeyByIndex(key_index);
            parent->EraseChildByIndex(child_index);
        }
    };

    if (auto second_node = FindNodeToMerge_(node); second_node)
    {
        auto key_connecting_2_nodes = FindKeyConnecting2Nodes_(node, second_node);
        node->InsertKeys({ key_connecting_2_nodes });
        second_node->MoveAllCells(node);
        UnlinkFromParentNode(second_node, key_connecting_2_nodes);
        UnlinkFromNeighbors_(second_node);
        delete second_node;
    }
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::Node* BPlusTree<Key, Tp>::FindNodeToMerge_(Node* node)
{
    if (auto left_node = node->GetLeft(); PossibleToMerge_(node, left_node))
    {
        return left_node;
    }
    else if (auto right_node = node->GetRight(); PossibleToMerge_(node, right_node))
    {
        return right_node;
    }
    return nullptr;
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::key_type
BPlusTree<Key, Tp>::FindKeyConnecting2Nodes_(Node* first_node, Node* second_node)
{
    auto parent = first_node->GetParent();
    auto min_key = first_node->Keys().front();
    auto index = parent->GetKeyIndex(min_key);

    if (first_node->GetLeft() == second_node && !parent->Exists(min_key, index))
    {
        --index;
    }
    else if (first_node->GetRight() == second_node && parent->Exists(min_key, index))
    {
        ++index;
    }

    return parent->Keys().at(index);
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::UnlinkFromParentNode_(Node* node)
{
    if (auto parent = node->GetParent(); parent)
    {
        auto parent_key = node->Keys().front();
        auto [key_index, child_index] = parent->GetKeyChildIndex(parent_key, node);

        if (!parent->Exists(parent_key, key_index) && key_index != 0)
        {
            --key_index;
        }

        parent->EraseKeyByIndex(key_index);
        parent->EraseChildByIndex(child_index);
    }
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::UnlinkFromNeighbors_(Node* node)
{
    auto left_node = node->GetLeft();
    auto right_node = node->GetRight();

    if (left_node)
    {
        left_node->SetRight(right_node);
    }
    if (right_node)
    {
        right_node->SetLeft(left_node);
    }
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::Exists_(const key_type& key)
{
    auto [leaf_node, internal_node] = FindLeafNodeWithInternalNode_(key);
    return (leaf_node && leaf_node->Exists(key)) || (internal_node && internal_node->Exists(key));
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::Borrow_(Node* borrowing_node)
{
    if (Node* borrowed_node = FindNodeToBorrow_(borrowing_node); borrowed_node)
    {
        auto key_connecting_2_nodes = FindKeyConnecting2Nodes_(borrowing_node, borrowed_node);
        auto borrowed_key = borrowed_node->Move1Cell(borrowing_node);
        auto parent = borrowing_node->GetParent();

        if (borrowing_node->IsLeaf())
        {
            auto new_key_connecting_2_nodes = (borrowing_node->GetRight() == borrowed_node) ? borrowed_node->Keys().front() : borrowed_key;
            parent->Update(key_connecting_2_nodes, new_key_connecting_2_nodes);
        }
        else
        {
            parent->Update(key_connecting_2_nodes, borrowed_key);
            borrowing_node->Update(borrowed_key, key_connecting_2_nodes);
        }
    }
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::Node* BPlusTree<Key, Tp>::FindNodeToBorrow_(Node* borrowing_node)
{
    if (auto left_node = borrowing_node->GetLeft(); PossibleToBorrow_(borrowing_node, left_node))
    {
        return left_node;
    }
    else if (auto right_node = borrowing_node->GetRight(); PossibleToBorrow_(borrowing_node, right_node))
    {
        return right_node;
    }
    return nullptr;
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::Node* BPlusTree<Key, Tp>::FindLeaf_(const key_type& key)
{
    auto node = root_;

    while (!node->IsLeaf())
    {
        auto children = node->Children();
        auto index = node->GetKeyIndex(key);

        if (node->Exists(key, index))
        {
            ++index;
        }

        node = children.at(index);
    }

    return node;
}

template<class Key, class Tp>
std::pair<typename BPlusTree<Key, Tp>::Node*, typename BPlusTree<Key, Tp>::Node*>
BPlusTree<Key, Tp>::FindLeafNodeWithInternalNode_(const key_type& key)
{
    auto node = root_;
    Node* internal_node = nullptr;

    if (!root_->IsLeaf() && node->Exists(key))
    {
        internal_node = node;
    }

    while (!node->IsLeaf())
    {
        auto children = node->Children();
        auto index = node->GetKeyIndex(key);

        if (node->Exists(key, index))
        {
            ++index;
            internal_node = node;
        }

        node = children.at(index);
    }

    return { node, internal_node };
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::key_type BPlusTree<Key, Tp>::GetMinKey_(Node* node)
{
    while (!node->IsLeaf())
    {
        node = node->Children().front();
    }

    return node->Keys().front();
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::Clear_(Node* node)
{
    auto& children = node->Children();
    std::for_each(children.begin(), children.end(), [&](auto& child){ Clear_(child); });
    delete node;
}

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_BPT_B_PLUS_TREE_TPP_
