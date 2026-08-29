#include <utility>
#include <vector>
#include <algorithm>

template <class TDerived, class ResultType, class IndexType = unsigned>
struct MoAlgorithm {
    void remove(IndexType idx);  // TODO: remove value at idx from data structure
    void add(IndexType idx);     // TODO: add value at idx from data structure
    ResultType get_answer();  // TODO: extract the current answer of the data structure

    struct Query {
        IndexType l, r, idx;
    };

    auto operator() (const auto& queries_, auto&& conv) {
        std::vector<Query> queries;
        queries.reserve(queries_.size());
        IndexType i = 0;
        for (auto& q: queries_) {
            auto [l, r] = conv(q);
            queries.emplace_back(l, r, i++);
        }
        IndexType block_size = int_ceil_sqrt(i);
        std::sort(queries.begin(), queries.end(), [block_size=block_size](auto& a, auto& b) {
            return std::make_pair(a.l/block_size, a.r) < std::make_pair(b.l/block_size, b.r);
        });
        std::vector<ResultType> answers(queries.size());
        IndexType cur_l = 0;
        IndexType cur_r = 0; // half-closed, empty range
        // invariant: data structure will always reflect the range [cur_l, cur_r)
        for (Query q : queries) {
            while (cur_l > q.l)
                static_cast<TDerived *>(this)->add(--cur_l);
            while (cur_r < q.r)
                static_cast<TDerived *>(this)->add(cur_r++);
            while (cur_l < q.l)
                static_cast<TDerived *>(this)->remove(cur_l++);
            while (cur_r > q.r)
                static_cast<TDerived *>(this)->remove(--cur_r);
            answers[q.idx] = static_cast<TDerived *>(this)->get_answer();
        }
        return answers;
    }
};

