#include <bits/stdc++.h>

using namespace std;

struct Key {
    uint64_t h1;
    uint64_t h2;
    bool operator==(const Key &other) const {
        return h1 == other.h1 && h2 == other.h2;
    }
};

struct KeyHasher {
    size_t operator()(const Key &k) const noexcept {
        return std::hash<uint64_t>()((k.h1 << 1) ^ (k.h2 + 0x9e3779b97f4a7c15ULL));
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    if (!(cin >> N)) {
        return 0;
    }

    vector<pair<int, int>> edges;
    edges.reserve(N - 1);
    vector<int> deg(N + 1, 0);

    for (int i = 0; i < N - 1; ++i) {
        int u, v;
        cin >> u >> v;
        edges.emplace_back(u, v);
        deg[u]++;
        deg[v]++;
    }

    vector<vector<int>> adj(N + 1);
    for (int i = 1; i <= N; ++i) {
        adj[i].reserve(deg[i]);
    }
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<int> parent(N + 1, 0);
    vector<int> order;
    order.reserve(N);
    vector<int> stack;
    stack.reserve(N);
    stack.push_back(1);
    parent[1] = 0;

    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        order.push_back(u);
        for (int v : adj[u]) {
            if (v == parent[u]) continue;
            parent[v] = u;
            stack.push_back(v);
        }
    }

    vector<int> type(N + 1, 0);
    vector<int> multiplicity(N + 1, 1);
    unordered_map<Key, int, KeyHasher> id_map;
    id_map.reserve(N * 2);

    const uint64_t INIT1 = 1469598103934665603ULL;
    const uint64_t INIT2 = 1099511628211ULL;
    const uint64_t MUL1 = 1099511628211ULL;
    const uint64_t MUL2 = 1469598103934665603ULL;

    for (int idx = (int)order.size() - 1; idx >= 0; --idx) {
        int u = order[idx];
        vector<pair<int, int>> children;
        children.reserve(adj[u].size());
        for (int v : adj[u]) {
            if (parent[v] == u) {
                children.emplace_back(type[v], v);
            }
        }

        sort(children.begin(), children.end(), [](const pair<int, int> &a, const pair<int, int> &b) {
            if (a.first != b.first) return a.first < b.first;
            return a.second < b.second;
        });

        uint64_t h1 = INIT1;
        uint64_t h2 = INIT2;
        for (auto &[t, node] : children) {
            uint64_t val = (uint64_t)t + 0x9e3779b97f4a7c15ULL;
            h1 ^= val;
            h1 *= MUL1;
            uint64_t val2 = (uint64_t)t + 0xbf58476d1ce4e5b9ULL;
            h2 ^= val2;
            h2 *= MUL2;
        }
        h1 ^= (uint64_t)children.size() + 0x94d049bb133111ebULL;
        h2 ^= ((uint64_t)children.size() << 32) ^ 0x2545F4914F6CDD1DULL;

        Key key{h1, h2};
        auto it = id_map.find(key);
        if (it == id_map.end()) {
            int new_id = (int)id_map.size() + 1;
            id_map.emplace(key, new_id);
            type[u] = new_id;
        } else {
            type[u] = it->second;
        }

        int i = 0;
        while (i < (int)children.size()) {
            int j = i;
            while (j < (int)children.size() && children[j].first == children[i].first) ++j;
            int count = j - i;
            for (int k = i; k < j; ++k) {
                multiplicity[children[k].second] = count;
            }
            i = j;
        }
    }

    vector<char> unique_up(N + 1, 0);
    long long answer = 0;
    unique_up[1] = 1;

    for (int u : order) {
        if (unique_up[u]) ++answer;
        for (int v : adj[u]) {
            if (parent[v] == u) {
                unique_up[v] = unique_up[u] && (multiplicity[v] == 1);
            }
        }
    }

    cout << answer << '\n';

    return 0;
}

