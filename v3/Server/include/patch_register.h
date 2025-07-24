#pragma once

#include <mutex>
#include <unordered_set>

template <typename cache_type>
class patch_register {
public:
    static void register_cache(cache_type* cache) {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_all_caches.insert(cache);
    }
    
    static void unregister_cache(cache_type* cache) {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_all_caches.erase(cache);
    }

    static std::unordered_set<cache_type*> get_all_caches() {
        std::lock_guard<std::mutex> lock(s_mutex);
        return s_all_caches;
    }

private:
    static std::mutex s_mutex;
    static std::unordered_set<cache_type*> s_all_caches;
};

template <typename cache_type>
std::mutex patch_register<cache_type>::s_mutex;

template <typename cache_type>
std::unordered_set<cache_type*> patch_register<cache_type>::s_all_caches;

template<typename cache_type>
inline void first_cache_register(cache_type* cache) {
    thread_local bool cache_registered = false;
    if (!cache_registered) {
        patch_register<cache_type>::register_cache(cache);
        cache_registered = true;
    }
}