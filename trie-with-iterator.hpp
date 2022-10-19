class Trie {
    struct TrieNode {
        vector<TrieNode *> i2c;
        TrieNode *parent;
        unsigned cnt;
        array<uint8_t, 26> c2i;
        char c;
        TrieNode(TrieNode *parent, char c):cnt(0), i2c(), parent(parent), c(c) { c2i.fill(c2i.size()); }
        TrieNode():TrieNode(nullptr, '\0') {}
        ~TrieNode() { for(auto c: i2c) delete c; }
    };
    struct TrieIterator {
        typedef input_iterator_tag iterator_tag;
        typedef char reference, value_type;
        TrieNode *p;
        TrieIterator(TrieNode *p):p{p} {}
        TrieIterator():TrieIterator{nullptr} {}
        reference operator*() { return p->c; }
        operator TrieNode *() { return p; }
        auto &operator++() { p = p->parent; return *this; }
        auto operator++(bool) { auto temp = *this; ++*this; return temp; }
        bool operator != (const TrieIterator other) const {
            return p != other.p;
        }
    };
    TrieNode *root;
    vector<TrieNode *> v;
public:
    Trie():root(nullptr) {}
    ~Trie() { delete root; }
    TrieIterator begin() { return TrieIterator(root); }
    TrieIterator end() { return nullptr; }
    void insert(const string &s, unsigned pos = 0) {
        auto pnode = &root;
        auto node = root;
        auto parent = nullptr;
        for (;;) {
            if (node == nullptr) {
                node = new TrieNode(parent);
                *pnode = node;
            }
            if (pos >= s.length()) {
                if (1 == ++node->cnt)
                    v.push_back(node);
                return;
            }
            auto &idx = node->c2i[s[pos] - 'a'];
            if (idx >= node->i2c.size()) {
                idx = node->i2c.size();
                node->i2c.emplace_back();
            }
            parent = node;
            pnode = &(node->i2c[idx]);
            node = *pnode;
            ++pos;
        }
    }
};
