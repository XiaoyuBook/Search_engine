#pragma once

#include <chrono>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <vector>
#include "nlohmann/json.hpp"
#include "patch_register.h"
#include "LRUCache.h"
#include <iostream>

using json = nlohmann::json;
using std::string;
using std::vector;

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

extern Main_cache<std::string, std::vector<string>> web_main_cache;
extern Main_cache<std::string, std::vector<json>> candidate_main_cache;

inline void candidate_patch_loop() {
    while(1) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        auto caches = patch_register<LRUCache<string,vector<json>>>::get_all_caches();
        for(auto* cache : caches) {
            auto keys = cache->take_dirty_keys();
            for(const auto& key : keys) {
                vector<json> val;
                if(cache->get_cache(key, val)) {
                    candidate_main_cache.put(key, val);
                    std::cout << "candidate key " << key << " patched" << std::endl;
                }
            }
        }
    }
}

inline void start_candidate_patch_sync() {
    std::thread(candidate_patch_loop).detach();
}

inline void web_patch_loop() {
    while(1) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        auto caches = patch_register<LRUCache<string,vector<string>>>::get_all_caches();
        for(auto* cache : caches) {
            auto keys = cache->take_dirty_keys();
            for(const auto& key : keys) {
                vector<string> val;
                if(cache->get_cache(key, val)) {
                    web_main_cache.put(key, val);
                    std::cout << "web search key " << key << " patched" << std::endl;
                }
            }
        }
    }
}

inline void start_web_patch_sync() {
    std::thread(web_patch_loop).detach();
}