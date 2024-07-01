#ifndef TRANSACTIONS_INCLUDE_BPT_B_PLUS_TREE_NODE_TPP_
#define TRANSACTIONS_INCLUDE_BPT_B_PLUS_TREE_NODE_TPP_

namespace s21
{

template<class Key, class Tp>
BPlusTree<Key, Tp>::BPlusTreeNode::BPlusTreeNode(bool leaf)
    : leaf_(leaf)
{}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::BPlusTreeNode::IsLeaf() const noexcept
{
    return leaf_;
}

template<class Key, class Tp>
std::vector<typename BPlusTree<Key, Tp>::BPlusTreeNode::key_type>& BPlusTree<Key, Tp>::BPlusTreeNode::Keys()
{
    return keys_;
}

template<class Key, class Tp>
std::vector<typename BPlusTree<Key, Tp>::BPlusTreeNode::mapped_type>& BPlusTree<Key, Tp>::BPlusTreeNode::Values()
{
    return values_;
}

template<class Key, class Tp>
std::vector<typename BPlusTree<Key, Tp>::BPlusTreeNode*>& BPlusTree<Key, Tp>::BPlusTreeNode::Children()
{
    return children_;
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::BPlusTreeNode::size_type BPlusTree<Key, Tp>::BPlusTreeNode::Size() const noexcept
{
    return keys_.size();
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::BPlusTreeNode::size_type
BPlusTree<Key, Tp>::BPlusTreeNode::GetKeyIndex(const key_type& key) const noexcept
{
    return std::distance(keys_.begin(), std::lower_bound(keys_.begin(), keys_.end(), key));
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::BPlusTreeNode::size_type
BPlusTree<Key, Tp>::BPlusTreeNode::GetChildIndex(const BPlusTreeNode* child) const noexcept
{
    return std::distance(children_.begin(), std::lower_bound(children_.begin(), children_.end(), child));
}

template<class Key, class Tp>
std::pair<typename BPlusTree<Key, Tp>::BPlusTreeNode::size_type, typename BPlusTree<Key, Tp>::BPlusTreeNode::size_type>
BPlusTree<Key, Tp>::BPlusTreeNode::GetKeyChildIndex(const key_type& key, const BPlusTreeNode* child) const noexcept
{
    auto key_index = GetKeyIndex(key);
    return { key_index, children_.at(key_index) == child ? key_index : key_index + 1 };
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::BPlusTreeNode::Exists(const key_type& key) const noexcept
{
    return Exists(key, GetKeyIndex(key));
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::BPlusTreeNode::Exists(const key_type& key, const size_type& index) const noexcept
{
    return index < Size() && keys_.at(index) == key;
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::BPlusTreeNode* BPlusTree<Key, Tp>::BPlusTreeNode::GetParent() const noexcept
{
    return parent_;
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::BPlusTreeNode* BPlusTree<Key, Tp>::BPlusTreeNode::GetLeft() const noexcept
{
    return left_;
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::BPlusTreeNode* BPlusTree<Key, Tp>::BPlusTreeNode::GetRight() const noexcept
{
    return right_;
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::BPlusTreeNode::mapped_type& BPlusTree<Key, Tp>::BPlusTreeNode::GetValue(const key_type& key)
{
    auto index = GetKeyIndex(key);

    if (Exists(key, index))
    {
        return values_.at(index);
    }

    throw std::runtime_error("The value was not found.");
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::SetParent(BPlusTreeNode* node) noexcept
{
    parent_ = node;
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::SetLeft(BPlusTreeNode* node) noexcept
{
    left_ = node;
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::SetRight(BPlusTreeNode* node) noexcept
{
    right_ = node;
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::MoveCells(BPlusTreeNode* dest, const key_type& limit)
{
    auto index = GetKeyIndex(limit);

    if (this->left_ == dest)
    {
        dest->keys_.insert(dest->keys_.end(), keys_.begin(), keys_.begin() + index);
        if (IsLeaf())
        {
            dest->values_.insert(dest->values_.end(), values_.begin(), values_.begin() + index);
            Erase(0, index);
        }
        else
        {
            dest->children_.insert(dest->children_.end(), children_.begin(), children_.begin() + index + 1);
            EraseChildren(0, index + 1);
            EraseKeys(0, index);

            for (auto& child : dest->Children())
            {
                child->SetParent(dest);
            }
        }
    }
    else
    {
        dest->keys_.insert(dest->keys_.begin(), keys_.begin() + index, keys_.end());
        if (IsLeaf())
        {
            dest->values_.insert(dest->values_.begin(), values_.begin() + index, values_.end());
            Erase(index, Size());
        }
        else
        {
            dest->children_.insert(dest->children_.begin(), children_.begin() + index, children_.end());
            EraseChildren(index, Size() + 1);
            EraseKeys(index, Size());

            for (auto& child : dest->Children())
            {
                child->SetParent(dest);
            }
        }
    }
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::MoveAllCells(BPlusTreeNode* dest)
{
    assert(this->left_ != nullptr || this->right_ != nullptr);

    bool is_left = (this->left_ == dest);

    auto key_insert_pos = is_left ? dest->keys_.end() : dest->keys_.begin();
    dest->keys_.insert(key_insert_pos, keys_.begin(), keys_.end());
    if (IsLeaf())
    {
        auto value_insert_pos = is_left ? dest->values_.end() : dest->values_.begin();
        dest->values_.insert(value_insert_pos, values_.begin(), values_.end());
    }
    else
    {
        auto children_insert_pos = is_left ? dest->children_.end() : dest->children_.begin();
        dest->children_.insert(children_insert_pos, children_.begin(), children_.end());

        for (auto& child : children_)
        {
            child->SetParent(dest);
        }
    }

    keys_.clear();
    values_.clear();
    children_.clear();
}

template<class Key, class Tp>
typename BPlusTree<Key, Tp>::BPlusTreeNode::key_type BPlusTree<Key, Tp>::BPlusTreeNode::Move1Cell(BPlusTreeNode* dest)
{
    bool is_left = (this->left_ == dest);

    if (is_left)
    {
        dest->keys_.insert(dest->keys_.end(), keys_.begin(), keys_.begin() + 1);
        keys_.erase(keys_.begin());

        if (IsLeaf())
        {
            dest->values_.insert(dest->values_.end(), values_.begin(), values_.begin() + 1);
            values_.erase(values_.begin());
        }
        else
        {
            dest->children_.insert(dest->children_.end(), children_.begin(), children_.begin() + 1);
            children_.erase(children_.begin());
            dest->children_.back()->SetParent(dest);
        }

        return dest->keys_.back();
    }
    else
    {
        dest->keys_.insert(dest->keys_.begin(), keys_.end() - 1, keys_.end());
        keys_.pop_back();

        if (IsLeaf())
        {
            dest->values_.insert(dest->values_.begin(), values_.end() - 1, values_.end());
            values_.pop_back();
        }
        else
        {
            dest->children_.insert(dest->children_.begin(), children_.end() - 1, children_.end());
            children_.pop_back();
            dest->children_.front()->SetParent(dest);
        }

        return dest->keys_.front();
    }
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::Update(const key_type& old_key, const key_type& new_key)
{
    auto index = GetKeyIndex(old_key);

    if (Exists(old_key, index))
    {
        keys_.at(index) = new_key;
    }

    assert(Exists(new_key, index));
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::Update(const key_type& new_key)
{
    auto index = GetKeyIndex(new_key);

    if (index == Size())
    {
        --index;
    }
    keys_.at(index) = new_key;

    assert(Exists(new_key, index));
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::BPlusTreeNode::Insert(key_type key, mapped_type value)
{
    auto index = GetKeyIndex(key);

    if (Exists(key, index))
    {
        return false;
    }

    keys_.insert(keys_.begin() + index, key);
    values_.insert(values_.begin() + index, value);

    return true;
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::BPlusTreeNode::Insert(const key_type& key, const std::vector<BPlusTreeNode*>& children)
{
    auto index = GetKeyIndex(key);

    if (Exists(key, index))
    {
        return false;
    }

    keys_.insert(keys_.begin() + index, key);
    children_.insert(children_.begin() + index, children.begin(), children.end());

    return true;
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::InsertKeys(const std::vector<key_type>& keys)
{
    auto index = GetKeyIndex(keys.front());

    keys_.insert(keys_.begin() + index, keys.begin(), keys.end());
}

template<class Key, class Tp>
bool BPlusTree<Key, Tp>::BPlusTreeNode::Erase(key_type key)
{
    auto index = GetKeyIndex(key);

    if (!Exists(key, index))
    {
        return false;
    }

    keys_.erase(keys_.begin() + index);
    if (IsLeaf())
    {
        values_.erase(values_.begin() + index);
    }
    else
    {
        children_.erase(children_.begin() + index);
    }

    return true;
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::Erase(const size_type& start, const size_type& end)
{
    EraseKeys(start, end);
    EraseValues(start, end);
    EraseChildren(start, end);
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::EraseKeyByIndex(size_type index)
{
    assert(index < keys_.size());
    assert(index >= 0);
    keys_.erase(keys_.begin() + index);
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::EraseKeys(const size_type& start, const size_type& end)
{
    keys_.erase(keys_.begin() + start, keys_.begin() + end);
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::EraseValues(const size_type& start, const size_type& end)
{
    if (IsLeaf())
    {
        values_.erase(values_.begin() + start, values_.begin() + end);
    }
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::EraseChildren(const size_type& start, const size_type& end)
{
    if (!IsLeaf())
    {
        children_.erase(children_.begin() + start, children_.begin() + end);
    }
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::EraseChild(const key_type& key)
{
    auto index = GetKeyIndex(key);

    if (Exists(key, index))
    {
        ++index;
    }

    children_.erase(children_.begin() + index);
}

template<class Key, class Tp>
void BPlusTree<Key, Tp>::BPlusTreeNode::EraseChildByIndex(size_type index)
{
    assert(index < children_.size());
    assert(index >= 0);
    children_.erase(children_.begin() + index);
}

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_BPT_B_PLUS_TREE_NODE_TPP_
