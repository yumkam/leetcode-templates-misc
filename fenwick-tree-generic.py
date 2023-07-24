from typing import TypeVar, List, Tuple, Generic, Callable
from collections.abc import Iterable
T = TypeVar('T')
K = TypeVar('K')
class FenwickGeneric(Generic[T]):
    A = []
    op = None
    def __init__(self, A: List[T], op: Callable[[T,T],T]) -> None:
        self.A = A
        self.op = op
    # should be called unless A consists of only of neutral element
    # (0 for addition, 1 for multiplication, equal values for min/max)
    def init(self) -> None:
        # time = O(n)
        n = len(self.A)
        for i in range(1, n):
            j = i + (i & -i)
            if j < n:
                self.A[j] = self.op(self.A[j], self.A[i])
    # requires `op(current_value, v) = new_value`
    # for op == max v must be equal or greater than current value
    # for op == min v must be equal or less than current value
    # time = O(log n)
    def add(self, idx: int, v: T) -> None:
        if idx == 0:
            self.A[0] = self.op(self.A[0], v)
            return
        n = len(self.A)
        while idx < n:
            self.A[idx] = self.op(self.A[idx], v);
            idx += idx & -idx # LSB

    # return op(A[0], op(A[1], op(A[2], ... op(A[idx - 1], A[idx]))))
    # time = O(log idx)
    def prefix_sum(self, idx: int) -> T:
        v = self.A[0]
        while idx != 0:
            v = self.op(v, self.A[idx])
            idx &= idx - 1
        return v
    def size(self) -> int:
        return len(self.A)
    # ra, rb = range_sum(i, j); ra = op(rb, op(...op(A[i + 1], A[i + 2]), ... A[j])
    # (that is, if invert() defined, you can compute op(...op(A[i + 1], A[i + 2]), ... A[j]) as op(ra, invert(rb))
    # but a bit faster than op(ft.prefix_sum(j), invert(ft.prefix_sum(i)))
    def range_sum(self, i:int, j: int) -> Tuple[T, T]:
        ra, rb = self.A[0], self.A[0];
        # assert(i < len(self.A))
        # assert(j < len(self.A))
        # assert(i < j)
        while j > i:
            ra = self.op(ra, self.A[j])
            j &= j - 1
        while i > j:
            rb = self.op(rb, self.A[i])
            i &= i - 1
        return (ra, rb)
    def append(self, v: T) -> None:
        # time = O(log n)
        cidx = len(self.A)
        if cidx > 0:
            j = cidx - 1
            while j + (j & -j) == cidx:
                v = self.op(v, self.A[j])
                j &= j - 1
        self.A.append(v)
    def extend(self, vs: Iterable[T]) -> None:
        # time = O(n + m)
        osize = len(self.A)
        self.A.extend(vs)
        i = max(1, osize - 1)
        # round down i to power of two
        while True:
            npow = i & (i - 1)
            if npow == 0:
                break
            i = npow
        while i < len(self.A):
            j = i + (i & -i)
            if j >= osize and j < len(self.A):
                A[j] = self.op(A[j], A[i])
            i += 1
    def bisect_left(self, key: K, j: int, jpow2: int, comp: Callable[[T, K], bool] = lambda a, b: a < b):
        acc = self.A[0]
        if not comp(acc, key): return 0
        res = 0
        k = jpow2
        while k > 0:
            p = res + k
            if p < j:
                temp = self.op(acc, A[p])
                if comp(temp, key): acc, res = temp, p
            k >>= 1
        return res + 1

    def bisect_left(self, key: K, comp: Callable[[T, K], bool] = lambda a, b: a < b):
        n = self.size()
        return self.bisect_left(key, n, 1<<n.bit_length(), comp)
    def bisect_right(self, key: K, j: int, jpow2: int, comp: Callable[[K, T], bool] = lambda a, b: a < b):
        acc = self.A[0]
        if comp(key, acc): return 0
        if jpow2 is None: jpow2 = 1<<j.bit_length()
        res = 0
        while jpow2 > 0:
            p = res + jpow2
            if p < j:
                temp = self.op(acc, A[p])
                if not comp(key, temp): acc, res = temp, p
            jpow2 >>= 1
        return res + 1
    def bisect_right(self, key: K, comp: Callable[[T, K], bool] = lambda a, b: a < b):
        n = self.size()
        return self.bisect_right(key, n, 1<<n.bit_length(), comp)
    def clear(self) -> None:
        self.A = []
