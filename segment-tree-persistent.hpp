#ifndef SEGMENT_TREE_PERSISTENT_HPP
#define SEGMENT_TREE_PERSISTENT_HPP
#include <iterator>
#include <limits>
#include <numeric>
#include <utility>
#include <queue>
#include <algorithm>
namespace SegmentTree {
    namespace PointUpdateRangeQuery {
        namespace Persistent {
            // combine(value_type &a, const value_type b) -> void
            template<typename ValueType, typename Combine, typename SizeType = size_t>
                requires requires (Combine combine, ValueType &a, const ValueType b) {
                    { combine(a, b) } -> std::same_as<void>;
                }
                class SegTree {
                    public:
                        typedef SizeType size_type;
                        typedef SizeType index_type;
                        typedef ValueType value_type;
                    private:
                        struct Node {
                            value_type val;
                            Node *l;
                            Node *r;
                        };
			mutable std::deque<Node> m_pool; // used as poor-man fixed chunk allocator
                        size_type m_size;
                        [[no_unique_address]]
                            Combine combine;
                    public:
                        SegTree(index_type size, Combine combine = Combine()) : m_size{size}, combine(combine) {
                        }
                    private:
                        template <typename Source>
                            Node *build(index_type l, index_type r, Source src) const {
                                if (l == r)
                                    return nullptr;
                                if (l + 1 == r)
                                    return &m_pool.emplace_back(src(), nullptr, nullptr);
                                auto mid = l + (r - l)/2; //std::midpoint(l, r);
                                auto newl = build(l, mid, src);
                                auto res = &m_pool.emplace_back(newl->val, newl, build(mid, r, src));
                                combine(res->val, res->r->val);
                                return res;
                            }
                        Node *update(Node *root, index_type i, const value_type &v, index_type l, index_type r) const {
                            if (l == r)
                                return nullptr;
                            if (l + 1 == r)
                                return &m_pool.emplace_back(v, nullptr, nullptr);
                            auto mid = l + (r - l)/2; //std::midpoint(l, r);
                            auto newl = root ? root->l : nullptr;
			    auto newr = root ? root->r : nullptr;
			    if (i < mid) newl = update(newl, i, v, l, mid);
			    else newr = update(newr, i, v, mid, r);
                            auto res = &m_pool.emplace_back((newl == nullptr ? newr->val : newl->val), newl, newr);
                            if (newl != nullptr && newr != nullptr)
                                combine(res->val, newr->val);
			    return res;
                        }
                        void query(Node *root, index_type l, index_type r, index_type lo, index_type hi, value_type &res) const {
                            if (root == nullptr)
                                return;
                            if (lo <= l && r <= hi) {
                                combine(res, root->val);
                                return;
                            }
                            auto mid = l + (r - l)/2; //std::midpoint(l, r);
                            if (lo < mid)
                                query(root->l, l, mid, lo, std::min(hi, mid), res);
                            if (mid < hi)
                                query(root->r, mid, r, std::max(mid, lo), hi, res);
                        }
                        template <typename Reduce>
                        void reduce(Node *root, index_type l, index_type r, index_type lo, index_type hi, Reduce op) const {
                            if (root == nullptr)
                                return;
                            if (lo <= l && r <= hi) {
                                op(root->val);
                                return;
                            }
                            auto mid = l + (r - l)/2; //std::midpoint(l, r);
                            if (lo < mid)
                                reduce(root->l, l, mid, lo, std::min(hi, mid), op);
                            if (mid < hi)
                                reduce(root->r, mid, r, std::max(mid, lo), hi, op);
                        }
                    public:
                        typedef Node * node_type;
                        // O(n) 
                        template <typename Source>
                            Node *build(Source src) {
                                return build(0, m_size, src);
                            }
                        // O(log(n)) recursion/stack depth, O(log n) time, O(log n) allocations
                        Node *update(Node *root, index_type i, const value_type &v) {
                            return update(root, i, v, (index_type)0, m_size);
                        }
                        // O(log(n)) recursion/stack depth, O(log n) time
                        void query_halfopen(Node *root, index_type lo, index_type hi, value_type &res) const {
                            query(root, (index_type)0, m_size, lo, hi, res);
                        }
                        // O(log(n)) recursion/stack depth, O(log n) time
                        // op should be callable as op(const value_type) -> void
                        template <typename Reduce>
                        void reduce_halfopen(Node *root, index_type lo, index_type hi, Reduce op) const {
                            reduce(root, (index_type)0, m_size, lo, hi, op);
                        }
                };
        }
    }
}
#endif // SEGMENT_TREE_PERSISTENT_HPP
