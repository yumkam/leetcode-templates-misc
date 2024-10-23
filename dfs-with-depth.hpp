#ifndef DFS_WITH_DEPTH
#define DFS_WITH_DEPTH
#include <vector>
#include <utility>
#include <tuple>
template <class TreeNode, class SizeType = unsigned>
class dfs_with_depth {
    std::vector<std::pair<TreeNode *, SizeType>> stack;
    public:
    void operator()(TreeNode *node, auto hook) {
        SizeType depth = 0;
        for (;;) {
            while(node) {
                hook(node, depth);
                ++depth;
                if (!node->left)
                    node = node->right;
                else {
                    if (node->right)
                        stack.emplace_back(node->right, depth);
                    node = node->left;
                }
            }
            if (stack.empty())
                break;
            std::tie(node, depth) = stack.back();
            stack.pop_back();
        }
    }
};
#endif
