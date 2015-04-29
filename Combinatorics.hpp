// NO-RECURSIVE EDITION: think as number radix acc 01..09,10..
template <typename T, typename F>
int product(vector<vector<T> > const &s, F f){
    vector<size_t> indices(s.size()); //每个子数组的索引
    size_t i = 0;
    while (i < s.size()) {
        vector<T> r;
        for (size_t j = 0; j < s.size(); j++)
            r.push_back(s[j][indices[j]]);
        f(r);
        i = 0; // rest the changing vector
        while (i < s.size() && ++indices[i] == s[i].size())
            indices[i++] = 0;
    }
}
