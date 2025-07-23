#include <unordered_map>
#include <mutex>

template <typename Key, typename Value>
class Main_cache {
public:
    void put(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cache[key] = value;
    }

    bool get(const Key& key, Value& value) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_cache.find(key);
        if (it == m_cache.end()) return false;
        value = it->second;
        return true;
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_map<Key, Value> m_cache;
};