#pragma once

#include "Engine/Core/Base.hpp"

#include <vector>

namespace Engine {
    
    template<typename Component>
    class SparseSet {
    private:
        using data_type = GameObject_type;
        using index_type = data_type;

        std::vector<index_type> sparse;
        std::vector<data_type> dense;
        std::vector<Component> dense_comp;
        const data_type maxValue;

    public:

        SparseSet(data_type maxV) : maxValue(maxV) {
            sparse.resize(maxV + 1);
        }

        ~SparseSet() {
        }

        int search(data_type x) {
            if (x > maxValue)
                return -1;

            if (sparse[x] < dense.size() && dense[sparse[x]] == x)
                return (sparse[x]);

            return -1;
        }

        Component& searchComp(data_type x) {
            return (dense_comp[sparse[x]]);
        }

        bool insert(data_type x, Component comp) {
            if (x > maxValue)
                return false;
            if (search(x) != -1)
                return false;

            sparse[x] = dense.size();
            dense.push_back(x);
            dense_comp.push_back(comp);

            return true;
        }

        void erase(data_type x) {
            if (search(x) == -1)
                return;

            // Remove element and shuffle back
            data_type temp = dense[dense.size() - 1];
            Component temp_comp = dense_comp[dense_comp.size() - 1];

            dense[sparse[x]] = temp;
            dense_comp[sparse[x]] = temp_comp;

            sparse[temp] = sparse[x];

            // erase old entry
            dense.erase(dense.end() - 1);
            dense_comp.erase(dense_comp.end() - 1);
            sparse[temp] = 0;
        }

        void clear() { 
            dense.clear();
            dense_comp.clear();

            sparse.clear();
            sparse.resize(maxValue);
        }

        const std::vector<data_type>& GetDense() const {
            return dense;
        }

        //const std::vector<Component>& GetDenseComp() const {
        //    return dense_comp;
        //}

        std::vector<Component>& GetDenseComp() {
            return dense_comp;
        }

        //SparseSet* SetIntersect(SparseSet& s);
        //SparseSet* SetUnion(SparseSet& s);
    };
}
