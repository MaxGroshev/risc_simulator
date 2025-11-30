#pragma once

#include <iostream>
#include <list>
#include <unordered_map>

namespace utils {

template <typename KeyT, typename ValueT>
class lru_cache final {
private:
    // using KeyT = const ets::EtsClass *;

    static constexpr size_t CAPACITY = 10;
    std::list<std::pair<KeyT, ValueT>> cacheList_;
    std::unordered_map<KeyT, typename std::list<std::pair<KeyT, ValueT>>::iterator> cacheMap_;

public:
    std::optional<ValueT> Get(const KeyT key)
    {
        if (cacheMap_.find(key) == cacheMap_.end()) {
            return std::nullopt;
        }
        cacheList_.splice(cacheList_.begin(), cacheList_, cacheMap_[key]);
        return cacheMap_[key]->second;
    }

    void Put(KeyT key, const ValueT &value)
    {
        if (cacheMap_.find(key) != cacheMap_.end()) {
            cacheMap_[key]->second = value;
            cacheList_.splice(cacheList_.begin(), cacheList_, cacheMap_[key]);
        } else {
            if (cacheList_.size() == CAPACITY) {
                cacheMap_.erase(cacheList_.back().first);
                cacheList_.pop_back();
            }
            cacheList_.push_front({key, value});
            cacheMap_[key] = cacheList_.begin();
        }
    }
};

}