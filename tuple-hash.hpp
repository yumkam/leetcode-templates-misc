// https://stackoverflow.com/questions/7110301/generic-hash-for-tuples-in-unordered-map-unordered-set
// https://stackoverflow.com/a/7111460
size_t hash_combiner(size_t left, size_t right) //replacable
{ return left + 0x9e3779b9 + (right<<6) + (right>>2);}

template<int index, class...types>
struct hash_impl {
    size_t operator()(size_t a, const std::tuple<types...>& t) const {
        typedef typename std::tuple_element<index, std::tuple<types...>>::type nexttype;
        hash_impl<index-1, types...> next;
        size_t b = std::hash<nexttype>()(std::get<index>(t));
        return next(hash_combiner(a, b), t); 
    }
};
template<class...types>
struct hash_impl<0, types...> {
    size_t operator()(size_t a, const std::tuple<types...>& t) const {
        typedef typename std::tuple_element<0, std::tuple<types...>>::type nexttype;
        size_t b = std::hash<nexttype>()(std::get<0>(t));
        return hash_combiner(a, b); 
    }
};

template<class...types>
struct tuple_hash<std::tuple<types...>> {
    size_t operator()(const std::tuple<types...>& t) {
        const size_t begin = std::tuple_size<std::tuple<types...>>::value-1;
        return hash_impl<begin, types...>()(0, t);
    }
}
