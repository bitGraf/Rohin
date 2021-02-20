#pragma once

#include "SparseSet.hpp"

namespace Engine {

    template<typename Component>
    using componentSparseSet = SparseSet<Component>;

    class Pool {
    public:
        Pool() : m_ComponentSet(nullptr), m_Type(0) {}
        Pool(size_t type) : m_ComponentSet(nullptr), m_Type(type) {}

        template<typename T>
        void Set() {
            if (m_ComponentSet)
                delete m_ComponentSet;

            m_ComponentSet = new componentSparseSet<T>(100);
            m_Type = typeid(T).hash_code();
        }

        template<typename T>
        componentSparseSet<T>& Get() {
            ENGINE_LOG_ASSERT(m_ComponentSet, "ComponentSet does not exist currently for this type");
            ENGINE_LOG_ASSERT(typeid(T).hash_code() == m_Type, "Types do not match");

            return *(componentSparseSet<T>*)(m_ComponentSet);
        }

    private:
        void* m_ComponentSet;
        size_t m_Type;
    };
}