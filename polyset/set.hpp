#pragma once
#include "searchable_bag.hpp"

class set {
    searchable_bag* b;
    set();
public:
    set(searchable_bag& bag) : b(&bag) {}
    set(const set& o) : b(o.b) {}
    set& operator=(const set& o) { b = o.b; return *this; }
    ~set() {}

    bool has(int v) const { return b ? b->has(v) : false; }
    void insert(int v) { if (b && !has(v)) b->insert(v); }
    void insert(int *a, int s) { for (int i = 0; i < s; ++i) insert(a[i]); }
    void print() const { if (b) b->print(); }
    void clear() { if (b) b->clear(); }
    searchable_bag& get_bag() const { return *b; }
};
