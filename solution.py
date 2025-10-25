import sys
from collections import defaultdict
class ComponentManager:
    """Cache component states to avoid repeated tuple allocations."""
    __slots__ = ("tuple_to_id", "id_to_tuple", "cache")
    def __init__(self):
        self.tuple_to_id = {}
        self.id_to_tuple = []
        self.cache = {}
        self._register((0,) * 7)
    def _register(self, comp_tuple):
        idx = len(self.id_to_tuple)
        self.tuple_to_id[comp_tuple] = idx
        self.id_to_tuple.append(comp_tuple)
        return idx
    def get_start_id(self):
        return 0
    def union(self, comp_id, u, v):
        key = (comp_id, u, v)
        cached = self.cache.get(key)
        if cached is not None:
            return cached
        comp_list = list(self.id_to_tuple[comp_id])
        mask_u = comp_list[u]
        mask_v = comp_list[v]
        if mask_u == 0:
            mask_u = 1 << u
        if mask_v == 0:
            mask_v = 1 << v
        union_mask = mask_u | mask_v
        for i in range(7):
            if (union_mask >> i) & 1:
                comp_list[i] = union_mask
        comp_tuple = tuple(comp_list)
        comp_id_new = self.tuple_to_id.get(comp_tuple)
        if comp_id_new is None:
            comp_id_new = self._register(comp_tuple)
        self.cache[key] = comp_id_new
        return comp_id_new
def process_case(domino_counts):
    comp_manager = ComponentManager()
    dp = {(0, comp_manager.get_start_id()): 1}
    for u in range(7):
        for v in range(u + 1, 7):
            count = domino_counts[u][v]
            if count == 0:
                continue
            odd = pow(2, count - 1)
            even_pos = odd - 1
            new_dp = defaultdict(int)
            bit_mask = (1 << u) | (1 << v)
            for (parity_mask, comp_id), ways in dp.items():
                new_dp[(parity_mask, comp_id)] += ways
                comp_union_id = comp_manager.union(comp_id, u, v)
                new_dp[(parity_mask ^ bit_mask, comp_union_id)] += ways * odd
                if even_pos:
                    new_dp[(parity_mask, comp_union_id)] += ways * even_pos
            dp = new_dp
    # After processing all domino types, account for types we skipped (count == 0)
    # They do not change the DP states, but we must keep existing counts.
    # However, the loop above skipped updating dp when count==0, so dp already contains
    # the correct counts. We just need to evaluate valid final states.
    total = 0
    for (parity_mask, comp_id), ways in dp.items():
        comp_tuple = comp_manager.id_to_tuple[comp_id]
        component_mask = 0
        connected = True
        for comp_mask in comp_tuple:
            if comp_mask:
                if component_mask == 0:
                    component_mask = comp_mask
                elif comp_mask != component_mask:
                    connected = False
                    break
        if component_mask == 0 or not connected:
            continue
        odd_vertices = (parity_mask & component_mask).bit_count()
        if odd_vertices not in (0, 2):
            continue
        total += ways
    return total
def main():
    input_data = sys.stdin.read().strip().split()
    if not input_data:
        return
    it = iter(input_data)
    t = int(next(it))
    outputs = []
    for _ in range(t):
        m = int(next(it))
        counts = [[0] * 7 for _ in range(7)]
        for _ in range(m):
            x = int(next(it)) - 1
            y = int(next(it)) - 1
            if x > y:
                x, y = y, x
            counts[x][y] += 1
        result = process_case(counts)
        outputs.append(str(result))
    sys.stdout.write("\n".join(outputs))
    if outputs:
        sys.stdout.write("\n")
if __name__ == "__main__":
    main()