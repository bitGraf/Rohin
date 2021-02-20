#pragma once

#include "Engine/Core/Base.hpp"
#include "Engine/GameObject/Pool.hpp"

namespace Engine {    
    constexpr size_t max_components = 100;
    using reg_type = GameObject_type;

    class Registry {
    private:
        std::vector<reg_type> m_RegList;
        std::unordered_map<size_t, Pool> m_Pools;
    public:
        template<typename Component>
        using component_set = componentSparseSet<Component>;

        Registry() {
        }

        reg_type Create() {
            auto& nextID = GetHighestID();
            nextID++;
            m_RegList.push_back(nextID);
            ENGINE_LOG_INFO("GameObject {0} created!", nextID);
            return nextID;
        }

        const std::vector<reg_type>& GetRegList() {
            return m_RegList;
        }

        template<typename Component, typename... Args>
        Component& emplace(reg_type go, Args &&... args) {
            Component c(std::forward<Args>(args)...);
            if (assure<Component>().insert(go, c)) {
                ENGINE_LOG_TRACE("GameObject {0} given component {1}!", go, DEBUG_OSTR(c));
                return assure<Component>().searchComp(go);
            }
        }

        /*
        void erase(GameObject go) {
            // idk how to do this
            // need to call erase on each Component type,
            // but i have no idea how to know what components a gameobject has
        }
        */

        // in the mean time, just explicitly delete each component
        template<typename Component>
        void erase(reg_type go) {
            assure<Component>().erase(go);
        }

        template<typename Component>
        bool has(reg_type go) {
            return assure<Component>().search(go) != -1;
        }

        template<typename Component>
        std::vector<Component>& view() {
            return assure<Component>().GetDenseComp();
        }

        template<typename Component>
        Component& get(reg_type go) {
            return assure<Component>().searchComp(go);
        }

    private:
        template<typename Component>
        component_set<Component>& assure() {
            size_t comp_hash = typeid(Component).hash_code();
            if (m_Pools.find(comp_hash) == m_Pools.end()) {
                m_Pools[comp_hash] = Pool(comp_hash);
                m_Pools[comp_hash].Set<Component>();
            }

            return m_Pools[comp_hash].Get<Component>();
        }

        reg_type& GetHighestID() {
            static reg_type nextID = 0;
            return nextID;
        }
    };
}
