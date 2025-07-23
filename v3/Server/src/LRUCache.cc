#include <list>
#include <mutex>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <utility>
using std::list;
using std::pair;
using std::unordered_map;


template <typename Key, typename Value>
class LRUCache{
    public:
    LRUCache(size_t capacity):m_capacity(capacity){}
    
    bool get(const Key& key, Value& value) {
        auto it = m_map.find(key);
        if(it == m_map.end()) return false;
        m_list.splice(m_list.begin(), m_list, it->second);
        value = it ->second -> second;
        return true;
    }

    void put(const Key& key, const Value &value) {
        auto it = m_map.find(key);
        if(it != m_map.end()) {
            it ->second->second = value;
            m_list.splice(m_list.begin(), m_list, it->second);
        } else {
            if(m_list.size() > m_capacity) {
                auto last = m_list.back();
                m_map.erase(last.first);
                m_list.pop_back();
            }
            m_list.emplace_front(key,value);
            m_map[key] = m_list.begin();
        }
        std::lock_guard<std::mutex> lock(m_patch_mutex);
        m_dirty_keys.insert(key);
    }

    std::unordered_set<Key> take_dirty_keys(){
        std::lock_guard<std::mutex> lock(m_patch_mutex);
        auto result = m_dirty_keys;
        m_dirty_keys.clear();
        return result;
    }

    bool get_cache(const Key &key, Value &value) const {
        auto it = m_map.find(key);
        if(it == m_map.end()) return false;
        value = it->second->second;
        return true;
    }

    private:
    size_t m_capacity;
    list<pair<Key,Value>> m_list;
    unordered_map<Key, typename list<pair<Key,Value>>::iterator >m_map;
    std::unordered_set<Key> m_dirty_keys;
    mutable std::mutex m_patch_mutex;
};