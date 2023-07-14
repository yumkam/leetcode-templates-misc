#ifndef MORRIS_TRAVERSAL_HPP
#define MORRIS_TRAVERSAL_HPP
// O(1) space (no stack), modifies tree, but restore it before return
// WARNING: while tree is processed, its structure is distorted; you must not recurse on nodes in doit hook
template <typename TreeNode, typename DoIt>
void morrisInOrder(TreeNode *root, DoIt doit) {
	while(root) {
		if (auto pre = root->left; pre != nullptr) {
			decltype(pre) right;

			while ((right = pre->right) != nullptr && right != root)
				pre = right;

			if (right != nullptr) {
				pre->right = nullptr;
				auto next = root->right;
				doit(root);
				root = next;
			} else {
				pre->right = root;
				root = root->left;
			}
		} else {
			auto next = root->right;
			doit(root);
			root = next;
		}
	}
}

template <typename TreeNode, typename DoIt>
void morrisPreOrder(TreeNode *root, DoIt doit) {
	// same as inorder, but swap pre->right {!= vs ==} nullptr
	while(root) {
		if (auto pre = root->left; pre != nullptr) {
			decltype(pre) right;

			while((right = pre->right) != nullptr && right != root)
				pre = right;

			if (right != nullptr) {
				pre->right = nullptr;
				root = root->right;
			} else {
				doit(root);
				pre->right = root;
				root = root->left;
			}
		} else {
			doit(root);
			root = root->right;
		}
	}
}

template <typename TreeNode, typename DoIt>
void morrisReversedPostOrder(TreeNode *root, DoIt doit) {
	// same as preorder, but swap left<->right
	while(root) {
		if (auto pre = root->right; pre != nullptr) {
			for(;;)
				if (auto left = pre->left; left != nullptr && left != root)
					pre = left;
				else
					break;
			if (pre->left != nullptr) {
				pre->left = nullptr;
				root = root->left;
			} else {
				pre->left = root;
				doit(root);
				root = root->right;
			}
		} else {
			doit(root);
			root = root->left;
		}
	}
}

template <typename TreeNode, typename DoIt>
void morrisLevelPreOrder(TreeNode *root, DoIt doit) {
	// same as preorder, but maintain current level
	// Note: doit() is not called in order of levels, but order of calls is maintained within level
	size_t level = 0;
	while(root) {
		if (auto pre = root->left; pre != nullptr) {
			size_t to_up = 1;
			for (;;)
				if (auto right = pre->right; right != nullptr && right != root) {
					pre = right;
					++to_up;
				} else {
					break;
				}
			if (pre->right != nullptr) {
				pre->right = nullptr;
				root = root->right;
				level -= to_up;
			} else {
				doit(root, level);
				pre->right = root;
				root = root->left;
				++level;
			}
		} else {
			doit(root, level);
			root = root->right;
			++level;
		}
	}
}

template <typename TreeNode, typename DoIt>
void morrisLevelInOrder(TreeNode *root, DoIt doit) {
	// same as inorder, but maintain current level
	size_t level = 0;
	while(root) {
		if (auto pre = root->left; pre != nullptr) {
			size_t to_up = 1;
			for(;; ++to_up)
				if (auto right = pre->right; right != nullptr && right != root)
					pre = right;
				else
					break;
			if (pre->right != nullptr) {
				pre->right = nullptr;
				level += to_up;
				doit(root, level - 1);
				root = root->right;
			} else {
				pre->right = root;
				root = root->left;
				++level;
			}
		} else {
			auto next = root->right;
			doit(root, level);
			root = next;
			++level;
		}
	}
}

#if 0 // FIXME
// traverse all nodes, calls doit hook on node when it is done with, and have correct left/right pointers
template <typename TreeNode, typename DoIt>
void morrisAltPostOrder(TreeNode *root, DoIt doit) {
	while(root) {
		if (root->left == nullptr) {
			root = root->right;
		} else {
			auto pre = root->left;
			for(;;)
				if (auto right = pre->right; right != nullptr && right != root)
					pre = right;
				else
					break;
			if (pre->right != nullptr) {
				pre->right = nullptr;
				auto node = root->left;
				while(auto next = node->right) {
				    doit(node);
				    node = next;
				}
				auto next = root->right;
				doit(root);
				root = next;
			} else {
				pre->right = root;
				root = root->left;
			}
		}
	}
}
#endif
#if 0 // FIXME
// traverse all nodes, calls doit hook on node when it is done with, and have correct left/right pointers
template <typename TreeNode, typename DoIt>
void morrisAltOrder(TreeNode *root, DoIt doit) {
	while(root) {
		if (root->left == nullptr) {
			if (root->right == nullptr)
				doit(root);
			root = root->right;
		} else {
			auto pre = root->left;
			for(;;)
				if (auto right = pre->right; right != nullptr && right != root)
					pre = right;
				else
					break;
			if (pre->right != nullptr) {
				pre->right = nullptr;
				doit(pre);
				doit(root);
				root = root->right; 
			} else {
				pre->right = root;
				root = root->left;
			}
		}
	}
}
#endif

template <typename TreeNode, typename DoIt>
void morrisLevelOrderLeafs(TreeNode *root, DoIt doit) {
	// same as preorder, but maintains current level
	// doit() is called only for leaf nodes
	size_t level = 0;
	while(root) {
		if (root->left == nullptr) {
			if (root->right == nullptr)
				doit(root, level);
			root = root->right;
			++level;
		} else {
			auto pre = root->left;
			size_t to_up = 1;
			for (;;)
				if (auto right = pre->right; right != nullptr && right != root) {
					pre = right;
					++to_up;
				} else {
					break;
				}
			if (pre->right != nullptr) {
				pre->right = nullptr;
				if (pre->left == nullptr)
					doit(pre, level - 1);
				root = root->right;
				level -= to_up;
			} else {
				//doit(root, level);
				pre->right = root;
				root = root->left;
				++level;
			}
		}
	}
}
#endif
