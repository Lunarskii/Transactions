#ifndef TRANSACTIONS_INCLUDE_BPT_B_PLUS_TREE_H_
#define TRANSACTIONS_INCLUDE_BPT_B_PLUS_TREE_H_

#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

#include "common/storage_interface.h"
#include "common/timer.h"

namespace s21
{

template<class Key, class Tp = Value>
class BPlusTree : public KeyValueStorageInterface<Key, Tp>
{
public:
    using key_type = typename KeyValueStorageInterface<Key, Tp>::key_type;
    using mapped_type = typename KeyValueStorageInterface<Key, Tp>::mapped_type;
    using size_type = typename KeyValueStorageInterface<Key, Tp>::size_type;

private:
    class BPlusTreeNode
    {
    public:
        using key_type = BPlusTree::key_type;
        using mapped_type = BPlusTree::mapped_type;
        using size_type = BPlusTree::size_type;

    public:
        explicit BPlusTreeNode(bool leaf);

        [[nodiscard]] bool IsLeaf() const noexcept;
        [[nodiscard]] std::vector<key_type>& Keys();
        [[nodiscard]] std::vector<mapped_type>& Values();
        [[nodiscard]] std::vector<BPlusTreeNode*>& Children();
        [[nodiscard]] size_type Size() const noexcept;
        [[nodiscard]] size_type GetKeyIndex(const key_type& key) const noexcept;
        [[nodiscard]] size_type GetChildIndex(const BPlusTreeNode* child) const noexcept;
        [[nodiscard]] std::pair<size_type, size_type> GetKeyChildIndex(const key_type& key, const BPlusTreeNode* child) const noexcept;
        [[nodiscard]] bool Exists(const key_type& key) const noexcept;
        [[nodiscard]] bool Exists(const key_type& key, const size_type& index) const noexcept;
        [[nodiscard]] BPlusTreeNode* GetParent() const noexcept;
        [[nodiscard]] BPlusTreeNode* GetLeft() const noexcept;
        [[nodiscard]] BPlusTreeNode* GetRight() const noexcept;
        [[nodiscard]] mapped_type& GetValue(const key_type& key);

        void SetParent(BPlusTreeNode* node) noexcept;
        void SetLeft(BPlusTreeNode* node) noexcept;
        void SetRight(BPlusTreeNode* node) noexcept;
        void MoveCells(BPlusTreeNode* dest, const key_type& limit);
        void MoveAllCells(BPlusTreeNode* dest);
        key_type Move1Cell(BPlusTreeNode* dest);
        void Update(const key_type& old_key, const key_type& new_key);
        void Update(const key_type& new_key);
        bool Insert(key_type key, mapped_type value);
        bool Insert(const key_type& key, const std::vector<BPlusTreeNode*>& children);
        void InsertKeys(const std::vector<key_type>& keys);
        bool Erase(key_type key);
        void Erase(const size_type& start, const size_type& end);
        void EraseKeyByIndex(size_type index);
        void EraseKeys(const size_type& start, const size_type& end);
        void EraseValues(const size_type& start, const size_type& end);
        void EraseChildren(const size_type& start, const size_type& end);
        void EraseChild(const key_type& key);
        void EraseChildByIndex(size_type index);

    private:
        bool leaf_{ false };
        std::vector<key_type> keys_;
        std::vector<mapped_type> values_;
        std::vector<BPlusTreeNode*> children_;
        BPlusTreeNode* parent_{ nullptr };
        BPlusTreeNode* left_{ nullptr };
        BPlusTreeNode* right_{ nullptr };
    };
    using Node = BPlusTreeNode;

public:
    BPlusTree();
    explicit BPlusTree(size_type order);
    ~BPlusTree();

    bool Insert(const key_type& key, const mapped_type& value) override;
    mapped_type& GetValue(const key_type& key) override;
    bool Erase(const key_type& key) override;
    std::vector<std::pair<key_type, mapped_type>> ShowAll() override;

private:
    [[nodiscard]] bool AreRelatives_(Node* first_node, Node* second_node) const noexcept;
    [[nodiscard]] bool PossibleToBorrow_(Node* borrowed_node) const noexcept;
    [[nodiscard]] bool PossibleToBorrow_(Node* borrowing_node, Node* borrowed_node) const noexcept;
    [[nodiscard]] bool PossibleToMerge_(Node* first_node, Node* second_node) const noexcept;

    void UpdateInternalNodeIndices(Node* internal_node, const key_type& key);
    std::pair<Node*, key_type> Split_(Node* node);
    void Merge_(Node* node);
    Node* FindNodeToMerge_(Node* node);
    key_type FindKeyConnecting2Nodes_(Node* first_node, Node* second_node);
    void UnlinkFromParentNode_(Node* node);
    void UnlinkFromNeighbors_(Node* node);
    bool Exists_(const key_type& key);
    void Borrow_(Node* borrowing_node);
    Node* FindNodeToBorrow_(Node* borrowing_node);
    Node* FindLeaf_(const key_type& key);
    std::pair<Node*, Node*> FindLeafNodeWithInternalNode_(const key_type& key);
    key_type GetMinKey_(Node* node);
    void Clear_(Node* node);

private:
    Node* root_{ nullptr };
    size_type order_{ 3 };
    size_type min_count_keys_{ 1 };
    size_type max_count_keys_{ 2 };
};

} // namespace s21

#include "b_plus_tree.tpp"
#include "b_plus_tree_node.tpp"

#endif // TRANSACTIONS_INCLUDE_BPT_B_PLUS_TREE_H_
