#pragma once
#include "tree_bag.hpp"
#include "searchable_bag.hpp"

class searchable_tree_bag : public tree_bag, public searchable_bag {
public:
    searchable_tree_bag() : tree_bag() {}
    searchable_tree_bag(const searchable_tree_bag& o) : tree_bag(o) {}
    searchable_tree_bag& operator=(const searchable_tree_bag& o) { tree_bag::operator=(o); return *this; }
    virtual ~searchable_tree_bag() {}

    virtual bool has(int v) const {
        node *c = tree;
        while (c) {
            if (v == c->value) return true;
            c = (v < c->value) ? c->l : c->r;
        }
        return false;
    }
};
