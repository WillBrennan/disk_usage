#pragma once

#include <iostream>
#include <ostream>
#include <vector>

#include "disker/logging.h"

namespace disker {
struct TreeNode {
    using Index = int;
    Index parent;
    Index children_start;
    Index children_end;
};

template <class ItemT>
class FlatTree {
  public:
    using Item = ItemT;
    using Node = TreeNode;
    using Index = Node::Index;
    static const Index NoParent = -1;

    Index emplace(Index parent, Item&& item);

    std::size_t size() const {
        logger::check_eq(nodes_.size(), items_.size(), "failed to populate correctly!");
        return nodes_.size();
    }

    const Item& at(const Index idx) const { return items_.at(idx); }
    const Item& operator[](const Index idx) const { return items_[idx]; }

    Item& at(const Index idx) { return items_.at(idx); }
    Item& operator[](const Index idx) { return items_[idx]; }

    Index parent(const Index idx) const { return nodes_.at(idx).parent; }

    const Node& node(const Index idx) const { return nodes_.at(idx); }

    bool hasChildren(const Index idx) const {
        const auto& node = nodes_.at(idx);
        return node.children_start != node.children_end;
    }

  private:
    std::vector<Node> nodes_;
    std::vector<Item> items_;
};

std::ostream& operator<<(std::ostream& stream, const TreeNode& node);

template <class T>
typename FlatTree<T>::Index FlatTree<T>::emplace(Index parent, Item&& item) {
    auto idx = Index(nodes_.size());

    // note(will.brennan) - children index behave like past the end iterators
    nodes_.emplace_back(Node{parent, idx + 1, idx + 1});
    items_.emplace_back(item);

    if (parent != NoParent) {
        auto& parent_node = nodes_.at(parent);

        if (hasChildren(parent)) {
            logger::check_eq(parent_node.children_end, idx, "children must be contiguous");
        } else {
            parent_node.children_start = idx;
        }

        parent_node.children_end = idx + 1;
    }

    return idx;
}
}  // namespace disker