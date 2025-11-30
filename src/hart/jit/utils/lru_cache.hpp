#pragma once

#include <iostream>
#include <list>
#include <unordered_map>

namespace utils {

template <typename KeyT, typename ValueT>
class lru_cache final {
private:
    const size_t capacity_;
    std::list<std::pair<KeyT, ValueT>> cacheList_;
    std::unordered_map<KeyT, typename std::list<std::pair<KeyT, ValueT>>::iterator> cacheMap_;

public:
    explicit lru_cache(size_t cache_entries = 4096) : capacity_(cache_entries) {

    }
    ValueT* lookup(const KeyT key)
    {
        if (cacheMap_.find(key) == cacheMap_.end()) {
            return nullptr;
        }
        // cacheList_.splice(cacheList_.begin(), cacheList_, cacheMap_[key]);
        return &cacheMap_[key]->second;
    }

    void install(KeyT key, ValueT &&value)
    {
        if (cacheMap_.find(key) != cacheMap_.end()) {
            cacheMap_[key]->second = std::move(value);
            // cacheList_.splice(cacheList_.begin(), cacheList_, cacheMap_[key]);
        } else {
            if (cacheList_.size() == capacity_) {
                cacheMap_.erase(cacheList_.back().first);
                cacheList_.pop_back();
            }
            cacheList_.push_front({key, std::move(value)});
            cacheMap_[key] = cacheList_.begin();
        }
    }

    size_t size() const { return cacheList_.size(); }//may be slow sometimes
};

}