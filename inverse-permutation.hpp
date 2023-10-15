// https://stackoverflow.com/questions/56603153/how-to-invert-a-permutation-represented-by-an-array-in-place
// https://stackoverflow.com/a/66807322
// author: https://stackoverflow.com/users/225186/alfc
// date: 2021-03-25T20:41Z
// ref: https://arxiv.org/pdf/1901.01926.pdf
namespace details {
	template<class It, class Index>
	void reverse_cycle(It t, Index start){
	    auto cur = t[start];
	    auto prev = start;
	    while( cur != start ){
		auto next = t[cur];
		t[cur] = prev;
		prev = cur;
		cur = next;
	    }
	    t[start] = prev;
	}

	template<class It, class Index>
	auto cycle_leader_shortcut(It t, Index start){
	    auto cur = t[start];
	    while(cur != start){
		if(cur < start) return false;
		cur = t[cur];
	    }
	    return true;
	}
}

// avg O(n log n) in-place algorithm
template<class It, class Size>
void inverse_permutation_shortcut_n(It first, Size n){
    using namespace details;
    for(Size i = 0; i != n; ++i){
        if( cycle_leader_shortcut(first, i) ) reverse_cycle(first, i);
    }
}
