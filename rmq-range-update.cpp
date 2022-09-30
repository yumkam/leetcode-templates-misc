#ifndef RMQ_RANGE_HPP
#define RMQ_RANGE_HPP
#include <vector>
#include <algorithm>
#include <numeric>
template<typename V, typename Size = unsigned>
class rmq {
	struct record {
		V value;
		V range_value;
		bool complete_range;
		record():value{numeric_limits<V>::min()}, range_value{numeric_limits<V>::min()}, complete_range{false} {}
	};
	std::vector<record> v;
	public:
	rmq(Size size):v{size} {}
	V query(Size left, Size right, Size begin, Size end, V cmax) {
		for (;;) {
			if (end < left || begin > right)
				break;
			Size mid = left + (right - left)/2;
			if (begin <= mid && mid <= end)
				cmax = std::max(cmax, v[mid].value);
			if (left == right)
				break;
			if (begin <= left && right <= end || v[mid].complete_range) {
				cmax = std::max(v[mid].range_value, cmax);
				break;
			}
			if (begin >= mid) {
				//if (mid == right) break;
				left = mid + 1;
				continue;
			}
			if (end <= mid) {
				if (left == mid) break;
				right = mid - 1;
				continue;
			}
			if (left < mid) // O(log n) recursion
				cmax = query(left, mid - 1, begin, end, cmax);
			left = mid + 1;
		}
		return cmax;
	}
	V query(Size begin, Size end, V cmax = numeric_limits<V>::min()) {
		return query(0, v.size() - 1, begin, end, cmax);
	}
	private:
	void update(Size left, Size right, Size begin, Size end, V value) {
		for (;;) {
			if (end < left || begin > right)
				return;
			Size mid = left + (right - left)/2;
			if (begin <= mid && mid <= end)
				v[mid].value = std::max(v[mid].value, value);
			if (left == right)
				return;
			auto range_value = v[mid].range_value;
			v[mid].range_value = std::max(range_value, value);
			if (begin <= left && right <= end) {
				v[mid].complete_range = true;
				return;
			}
			if (v[mid].complete_range) {
				if (left < mid) // O(1) recursion
					update(left, mid - 1, left, mid - 1, range_value);
				if (mid < right) // O(1) recursion
					update(mid + 1, right, mid + 1, right, range_value);
				v[mid].complete_range = false;
			}
			if (begin >= mid) {
				//if (mid < right) break;
				left = mid + 1;
				continue;
			}
			if (end <= mid) {
				if (left == mid) break;
				right = mid - 1;
				continue;
			}
			if (left < mid) // O(log n) recursion
				update(left, mid - 1, begin, end, value);
			left = mid + 1;
		}
	}
	public:
	void update(Size begin, Size end, V value) {
		update(0, v.size() - 1, begin, end, value);
	}
	private:
	void update(Size left, Size right, Size index, V value) {
		for (;;) {
			if (index < left || index > right)
				return;
			Size mid = left + (right - left)/2;
			if (index == mid)
				v[mid].value = std::max(v[mid].value, value);
			if (left == right)
				return;
			auto range_value = v[mid].range_value;
			v[mid].range_value = std::max(range_value, value);
			if (v[mid].complete_range) {
				if (left < mid) // O(1) recursion
					update(left, mid - 1, left, mid - 1, range_value);
				if (mid < right) // O(1) recursion
					update(mid + 1, right, mid + 1, right, range_value);
				v[mid].complete_range = false;
			}
			if (index >= mid) {
				// if (mid == right) break;
				left = mid + 1;
				continue;
			}
			if (index <= mid) {
				if (left == mid) break;
				right = mid - 1;
				continue;
			}
			left = mid + 1;
		}
	}
	public:
	void update(Size index, V value) {
		update(0, v.size() - 1, index, value);
	}
};
#endif
