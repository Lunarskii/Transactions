#ifndef TRANSACTIONS_INCLUDE_RBTREE_RBTREE_H_
#define TRANSACTIONS_INCLUDE_RBTREE_RBTREE_H_

#include <cstddef>
#include <iterator>
#include <stack>
#include <vector>
#include <queue>
#include <cassert>

namespace s21::s21_utils
{

enum rbColor { kBlack = 0, kRed = 1 };

template <class T, typename Comparator = std::less<T>,
          typename Allocator = std::allocator<T>>
class rbTree {
  using size_type = std::size_t;

 public:
  using data_type = T;

  /*
  ***************************
  Constructor
  */

  rbTree() = default;

  /*
  ***************************
  Destructor
  */

  ~rbTree() { clearTree(); };

  rbTree(std::initializer_list<T>&& args) {
    for (auto &&elm: args) {
      addNode(elm);
    }
  }

  /*
  ***************************
  Copy constructor
  */

  rbTree(const rbTree &other) {
    for (auto const &elm : other) {
      addNode(elm);
    }
  };

  /*
  ***************************
  Move constructor

  */

  rbTree(rbTree &&other) noexcept
      : root(other.root),
        leftmost(other.leftmost),
        rightmost(other.rightmost),
        size_(other.size_) {
    other.root = nullptr;
    other.leftmost = nullptr;
    other.rightmost = nullptr;
    other.size_ = 0;
  };

  rbTree &operator=(const rbTree &other) {
    if (this != &other) {
      rbTree tmp = other;
      *this = std::move(tmp);
    }
    return *this;
  }

  rbTree &operator=(rbTree &&other) noexcept {
    std::swap(root, other.root);
    std::swap(leftmost, other.leftmost);
    std::swap(rightmost, other.rightmost);
    std::swap(size_, other.size_);
    return *this;
  }

  struct rbTreeNode {
    ~rbTreeNode() {
      std::allocator_traits<Allocator>::destroy(allocator, data_);
      std::allocator_traits<Allocator>::deallocate(allocator, data_, 1);
    }
    rbTreeNode(const rbTreeNode &) = default;
    rbTreeNode(rbTreeNode &&) = default;
    void swap(rbTreeNode &other) noexcept
    {
      if (this == &other) return;
      std::swap(data_, other.data_);
    }

    Allocator allocator;
    rbTreeNode *left{};
    rbTreeNode *right{};
    rbTreeNode *parent{};
    bool rbColor = kRed;
    data_type *data_{};

    template <typename KK>
    explicit rbTreeNode(KK &&data, const Allocator &alloc = Allocator{})
        : allocator(alloc) {
      data_ = std::allocator_traits<Allocator>::allocate(allocator, 1);
      try {
        std::allocator_traits<Allocator>::construct(allocator, data_, std::forward<KK>(data));
      } catch (...) {
        std::allocator_traits<Allocator>::deallocate(allocator, data_, 1);
        throw;
      }
    }
  };

  /*
  Header is a parent node for the root, which helps
  to avoid checks if root->parent is nullptr. Also root is
  always header->left.

  Leftmost and righmost nodes point to the nodes with
  smallest and largest values in the tree. They provide
  constant complexity for begin() and end() iterators.

  nodesNum holds the number of nodes in the tree.

  */



 public:
  /*
 ***************************
 Forward Iterator allowing to read values without changing them


 */

  struct iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = data_type;
    using pointer = data_type *;
    using reference = data_type &;
    using self = iterator;

   public:
    iterator() noexcept : ptr_(nullptr), tree_ptr_(nullptr){};

    iterator(const rbTree *Tree, const rbTreeNode *node) noexcept {
      ptr_ = const_cast<rbTreeNode *>(node);
      tree_ptr_ = const_cast<rbTree *>(Tree);
    };

    iterator(const iterator &other) noexcept {
      ptr_ = other.ptr_;
      tree_ptr_ = other.tree_ptr_;
    }

    iterator(iterator &&other) noexcept {
      ptr_ = other.ptr_;
      tree_ptr_ = other.tree_ptr_;
    }

    self &operator=(const iterator &other) noexcept {
      if (this != &other) {
        ptr_ = other.ptr_;
        tree_ptr_ = other.tree_ptr_;
      }
      return *this;
    }

    self &operator=(iterator &&other) noexcept {
      ptr_ = other.ptr_;
      tree_ptr_ = other.tree_ptr_;
      return *this;
    }

    self &operator++() {
      ptr_ = tree_ptr_->getNextNode(ptr_);
      return *this;
    };

    self &operator--() {
      if (ptr_ == nullptr) {
        ptr_ = tree_ptr_->rightmost;
      } else
        ptr_ = tree_ptr_->getPrevNode(ptr_);
      return *this;
    };

    self operator++(int) const {
      self tmp = *this;
      ptr_ = tree_ptr_->getNextNode(ptr_);
      return tmp;
    };

    self operator--(int) {
      self tmp = *this;
      if (ptr_ == nullptr) {
        ptr_ = tree_ptr_->rightmost;
      } else
        ptr_ = tree_ptr_->getPrevNode(ptr_);
      return tmp;
    };

    reference operator*() { return *ptr_->data_; }

    pointer operator->() { return ptr_->data_; }

    friend bool operator!=(const self &a, const self &b) noexcept {
      return a.ptr_ != b.ptr_;
    };

    friend bool operator==(const self &a, const self &b) noexcept {
      return a.ptr_ == b.ptr_;
    };

    rbTreeNode *ptr_;
    rbTree *tree_ptr_;
  };

  /*
 ***************************
 Forward constant iterator allowing to read values without changing them

 */

  struct const_iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    typedef const data_type *pointer;
    typedef const data_type &reference;
    using self = const_iterator;
    typedef rbTreeNode *base_ptr;

    const_iterator() noexcept : ptr_(nullptr), tree_ptr_(nullptr){};

    explicit const_iterator(const iterator &other) noexcept
        : ptr_(other.ptr_), tree_ptr_(other.tree_ptr_){};

    const_iterator(const rbTree *Tree, const rbTreeNode *node) noexcept {
      ptr_ = const_cast<rbTreeNode *>(node);
      tree_ptr_ = const_cast<rbTree *>(Tree);
    };

    reference operator*() const noexcept { return *ptr_->data_; }

    pointer operator->() const noexcept { return ptr_->data_; }

    self &operator++() {
      ptr_ = tree_ptr_->getNextNode(const_cast<base_ptr>(ptr_));
      return *this;
    }

    self operator++(int) {
      self tmp = *this;
      ptr_ = tree_ptr_->getNextNode(ptr_);
      return tmp;
    }

    self &operator--() {
      if (ptr_ == nullptr) {
        ptr_ = tree_ptr_->rightmost;
      } else
        ptr_ = tree_ptr_->getPrevNode(const_cast<base_ptr>(ptr_));
      return *this;
    }

    self operator--(int) {
      self tmp = *this;
      if (ptr_ == nullptr) {
        ptr_ = tree_ptr_->rightmost;
      } else
        ptr_ = tree_ptr_->getPrevNode(const_cast<base_ptr>(ptr_));
      return tmp;
    }

    friend bool operator!=(const self &a, const self &b) noexcept {
      return a.ptr_ != b.ptr_;
    };

    friend bool operator==(const self &a, const self &b) noexcept {
      return a.ptr_ == b.ptr_;
    };

    rbTreeNode *ptr_;
    rbTree *tree_ptr_;
  };

  /*
  Many funtions depend on the iterator, so it has to be as high
  in the header as possible. But that forces to initialize it with
  tree pointer. Even though it's a tree subclass it doesn't know about
  methods that are declared under. As I understand.
  */

  iterator begin() const { return iterator(this, leftmost); }

  iterator end() const { return iterator(this, nullptr); }

  const_iterator cbegin() const { return const_iterator(this, leftmost); }

  const_iterator cend() const { return const_iterator(this, nullptr); }

  /*
  ***************************
  Public methods to work with the tree
  */

  [[nodiscard]] bool isEmpty() const noexcept { return size_ == 0; }

  [[nodiscard]] size_type Size() const { return static_cast<size_t>(size_); }

  [[nodiscard]] size_type maxSize() const {
    std::allocator<rbTreeNode> alloc1;
    return alloc1.max_size();
  }

  void clearTree() {
    std::stack<rbTreeNode*> stack;
    if (root != nullptr) {
      stack.push(root);
    }

    while (!stack.empty()) {
      rbTreeNode* current = stack.top();
      stack.pop();

      if (current->left != nullptr) {
        stack.push(current->left);
      }
      if (current->right != nullptr) {
        stack.push(current->right);
      }

      delete current;
    }
    root= nullptr;
    leftmost= nullptr;
    rightmost= nullptr;
    size_=0;
  }

  void addNode(std::initializer_list<data_type> &&args) {
    for (auto &&elm: args) {
      addNode(elm);
    }
  }

  /*
  This function setups the process of inserting the node.
  1. Finds the parent with findNode()
  2. Allocates memory with new
  3. Calls insertNode() to start the algorithm
  */
  std::pair<iterator, bool> addNode(data_type data) {
    auto [node, ok] = findNode(data);

    if (!ok) {  // insertion for set and map. No equal values allowed
      auto *newNode = new rbTreeNode{data};
      insertNode(newNode, node);
      assert(numBlack(root->left) == numBlack(root->right));
      return std::make_pair(iterator(this, newNode), true);
    }
    return std::make_pair(iterator(this, node), false);
  }

  /*
  Removes node on key. Returns number of removed nodes
  */
  size_type removeNode(data_type data) {
    auto [node, ok] = findNode(data);
    if (ok) {
      deleteNode(node);
      if (root)
      assert(numBlack(root->left) == numBlack(root->right));
      return 1;
    }
    return 0;
  }

  iterator removeNode(iterator iter) {  // removes nodes at itererator pos
    auto next_node = getNextNode(iter.ptr_);
    deleteNode(iter.ptr_);
    if (root)
    assert(numBlack(root->left) == numBlack(root->right));
    return iterator{this, next_node};
  };

  std::pair<iterator, bool> FindKey(const data_type &elemX) {
    auto [node, ok] = findNode(elemX);
    return std::make_pair(iterator(this, node), ok);
  }

 private:
  /*
  ***************************
  Using std::less to compare keys
  */

  int compareKeys(const data_type &lk, const data_type &rk) const {
    int res = 0;
    if (Comparator{}(lk, rk))
      res = -1;
    else if (Comparator{}(rk, lk))
      res = 1;
    return res;
  }

  /*
  Find the node by comparing keys. Return the result of comparement.
  */
  std::pair<rbTreeNode *, bool> findNode(const data_type &elemX) {
    rbTreeNode *currentNode = root;
    rbTreeNode *parentNode = nullptr;

    while (currentNode) {
      auto compare_result = compareKeys(elemX, *currentNode->data_);
      parentNode = currentNode;
      if (compare_result == 0) {  // we found the node
        return std::make_pair(currentNode, true);
      } else if (compare_result < 0) {  // search nodes on the left
        currentNode = currentNode->left;
      } else {  // search nodes on the right
        currentNode = currentNode->right;
      }
    }

    return std::make_pair(parentNode, false);
  };

  bool IsRed(const rbTreeNode *node) { return node && node->rbColor == kRed; }

  bool IsBlack(const rbTreeNode *node) { return !IsRed(node); }

  /*
  Link new node to the parent.
  */
  void insertNode(rbTreeNode *newNode, rbTreeNode *parentNode) {
    if (!parentNode) {
      newNode->rbColor = kBlack;
      root = newNode;
      leftmost = newNode;
      rightmost = newNode;
      ++size_;
      return;
    }

    newNode->parent = parentNode;
    auto cmp_result = compareKeys(*parentNode->data_, *newNode->data_);
    if (cmp_result > 0) {
      parentNode->left = newNode;
      if (parentNode ==
          leftmost) {
        leftmost = newNode;
      }
    } else {
      parentNode->right = newNode;
      if (parentNode == rightmost) {
        rightmost = newNode;
      }
    }

    ++size_;
    balanceNode(newNode);
  };

  /*
  Those are from the Introduction to Algorithms by
  Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest,
  and Clifford Stein.
  */

  void rotateLeft(rbTreeNode *pivotNode) {
    auto childNode = pivotNode->right;
    FixParents(pivotNode, childNode);
    pivotNode->right = childNode->left;
    if (childNode->left) childNode->left->parent = pivotNode;
    childNode->parent = pivotNode->parent;
    childNode->left = pivotNode;
    pivotNode->parent = childNode;
    childNode->rbColor = pivotNode->rbColor;
    pivotNode->rbColor = kRed;
  }
  void FixParents(const rbTreeNode *pivotNode, rbTreeNode *childNode) {
    auto parent = pivotNode->parent;
    if (!parent) {
      root = childNode;
      root->rbColor = kBlack;
    } else {
      if (parent->left == pivotNode) {
        parent->left = childNode;
      } else {
        parent->right = childNode;
      }
    }
  }

  void rotateRight(rbTreeNode *pivotNode) {
    auto childNode = pivotNode->left;
    FixParents(pivotNode, childNode);
    pivotNode->left = childNode->right;
    if (childNode->right) childNode->right->parent = pivotNode;
    childNode->parent = pivotNode->parent;
    childNode->right = pivotNode;
    pivotNode->parent = childNode;
    childNode->rbColor = pivotNode->rbColor;
    pivotNode->rbColor = kRed;
  }

  void FlipColor(rbTreeNode *pivotNode) {
    pivotNode->rbColor = kRed;
    pivotNode->left->rbColor = !pivotNode->left->rbColor;
    pivotNode->right->rbColor = !pivotNode->right->rbColor;
    root->rbColor = kBlack;
  }

  void balanceNode(rbTreeNode *pivotNode) {
    while (pivotNode != root) {
      auto parent = pivotNode->parent;
      if (pivotNode == root) break;  // Дерево сбалансировано

      // Если оба потомка красные, то меняем цвета, и дальше смотрим на
      // родителя, поскольку только он мог сломать баланс, продолжаем
      if (IsRed(parent->left) && IsRed(parent->right)) {
        FlipColor(parent);
        pivotNode = parent;
        continue;
      }

      // Если правый потомок красный, левый - черный, левый поворот, цвет
      // меняется у родителя, продолжаем
      if (IsRed(parent->right)) {
        rotateLeft(parent);
        pivotNode = parent;
        continue;
      }

      // Если нода красная и левый потомок, родитель красный, то поворачиваем
      // деда, текущая нода остается прежней для свапа, продолжаем
      if (IsRed(pivotNode) && IsRed(parent)) {
        rotateRight(parent->parent);
        continue;
      }
      // Если ни одно условие не выполнилось - дерево сбалансировано
      break;
    }

    root->rbColor = kBlack;  // Корень всегда черный
  }

  rbTreeNode *minNode(rbTreeNode *currentNode) {
    while (currentNode->left)
      currentNode = currentNode->left;
    return currentNode;
  }

  rbTreeNode *maxNode(rbTreeNode *currentNode) {
    while (currentNode->right)
      currentNode = currentNode->right;
    return currentNode;
  }

  rbTreeNode *getNextNode(rbTreeNode *currentNode) {
    if (currentNode == rightmost) return nullptr;
    if (currentNode->right) {
      return minNode(currentNode->right);
    } else if (currentNode == currentNode->parent->left) {
      return currentNode->parent;
    } else {
      while (currentNode == currentNode->parent->right) {
        currentNode = currentNode->parent;
      }
      return currentNode->parent;
    }
  }

  rbTreeNode *getPrevNode(rbTreeNode *currentNode) {
    if (currentNode == leftmost) return nullptr;
    if (currentNode->left) {
      return maxNode(currentNode->left);
    } else {
      while (currentNode == currentNode->parent->left) {
        currentNode = currentNode->parent;
      }
      return currentNode->parent;
    }
  }

  /*
  nodePtr is going to be deleted from the tree. We need to
  find another node for it's place (usually the next in order)
  and replace them. Then we balance tree as if this node
  'to be deleted' still exists.
  */
  void deleteNode(rbTreeNode *nodePtr) {
    auto node_to_delete = nodePtr;
    if (nodePtr->left) {
      // Если оба потомка - ищем минимум из правого поддерева, если только левый
      // потомок - значит потомок красный, а сама нода - черная - свапаем
      auto node_to_swap =
          nodePtr->right ? minNode(nodePtr->right) : nodePtr->left;
      nodePtr->swap(*node_to_swap);
      node_to_delete = node_to_swap;
    }
    // если удаляемая нода- черная, разбираем
    if (IsBlack(node_to_delete)) {
      auto deletion_result = node_to_delete;
      do {
        deletion_result = DeleteBlackNode(deletion_result);
      } while (deletion_result);
    }
    EraseNode(node_to_delete);
  }

  rbTreeNode *DeleteBlackNode(const rbTreeNode *nodePtr) {
    auto parent = nodePtr->parent;
    if (!parent) return nullptr;
    // Родитель красный
    if (IsRed(parent)) {
      DeleteWithRedParent(nodePtr);
      return nullptr;
    }
    // Родитель черный
    return DeleteWithBlackParent(nodePtr);
  }

  rbTreeNode *DeleteWithBlackParent(const rbTreeNode *node) {
    auto parent = node->parent;
    auto brother = parent->left == node ? parent->right : parent->left;
    auto brother_child = brother->left;
    // Если брат красный
    if (brother->rbColor == kRed) {
      Rotate(node);
      DeleteWithRedParent(node);
      return nullptr;
    }
    // Брат черный
    // Есть red ребенок у брата
    if (brother_child && brother_child->rbColor == kRed) {
      bool left_brother = brother == parent->left;
      Rotate(node);
      if (left_brother)
      {
        FlipColor(brother);
        brother->rbColor = kBlack;
      } else
      {
        rotateLeft(parent);
        rotateRight(brother);
        FlipColor(brother_child);
        brother_child->rbColor = kBlack;
      }
      return nullptr;
    }
    // Нет ребенка - меняем цвет brother и получаем дерево с bh-1
    brother->rbColor = kRed;
    balanceNode(brother);
    return brother->parent == parent ? parent: brother;
  }
  void Rotate(const rbTreeNode *node) {
    auto parent = node->parent;
    if (parent->right == node) {
      rotateRight(parent);
    } else {
      rotateLeft(parent);
    }
  }

  void DeleteWithRedParent(const rbTreeNode *node) {
    auto parent = node->parent;
    // Удаляем правого брата
    if (parent->right == node) {
      // у левого есть ребенок -> правый поворот
      if (parent->left->left && parent->left->left->rbColor == kRed) {
        rotateRight(parent);
        balanceNode(parent);
        return;
      }
      // у левого нет ребенка -> родитель становится черным, брат - красным
      SwapColors(parent, parent->left);
      balanceNode(parent->left);
      return;
    }
    // Удаляем левого брата
    // у правого есть ребенок -> левый поворот
    if (parent->right->left && parent->right->left->rbColor == kRed) {
      rotateLeft(parent);
      // Проверяем бывшего ребенка правого брата
      balanceNode(parent->right);
      return;
    }
    // у правого нет ребенка -> родитель становится черным, брат - красным
    SwapColors(parent, parent->right);
    balanceNode(parent->right);
  }

  void SwapColors(rbTreeNode *parent, rbTreeNode *child) {
    parent->rbColor=kBlack;
    child->rbColor=kRed;
  }

  void EraseNode(rbTreeNode *node) {
    auto parent = node->parent;
    if (!parent) {
      root = nullptr;
      leftmost = nullptr;
      rightmost = nullptr;
    } else {
      if (node == leftmost) leftmost = getNextNode(node);
      if (node == rightmost) rightmost = getPrevNode(node);
      if (parent->left == node) {
        parent->left = nullptr;
      } else {
        parent->right = nullptr;
      }
    }
    delete node;
    --size_;
  }

  int numBlack(rbTreeNode *subRoot) {
    if (!subRoot) return 0;
    int blackleft = numBlack(subRoot->left);
    int blackright = numBlack(subRoot->right);
    if (blackleft != blackright) throw std::runtime_error("internal");
    if (subRoot->rbColor == kBlack) {
      blackleft += 1;
    }
    return blackleft;
  };

private:
  rbTreeNode *root{};
  rbTreeNode *leftmost{};
  rbTreeNode *rightmost{};
  size_type size_ = 0;
};

} // namespace s21::s21_utils

#endif  // TRANSACTIONS_INCLUDE_RBTREE_RBTREE_H_
