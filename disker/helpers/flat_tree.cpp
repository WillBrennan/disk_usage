#include "disker/helpers/flat_tree.h"

namespace disker {
std::ostream& operator<<(std::ostream& stream, const TreeNode& node) {
    stream << "{";
    stream << " parent: " << node.parent;
    stream << " start:" << node.children_start;
    stream << " end: " << node.children_end;
    stream << " }";
    return stream;
}
}  // namespace disker