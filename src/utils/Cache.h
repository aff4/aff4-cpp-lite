/*-
 This file is part of AFF4 CPP.

 AFF4 CPP is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 AFF4 CPP is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with AFF4 CPP.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file Cache.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 21-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief LEU Cache implementation with loader function support.
 */

#ifndef SRC_UTILS_CACHE_H_
#define SRC_UTILS_CACHE_H_

#include <functional>
#include <list>
#include <mutex>
#include <map>

namespace aff4 {
namespace util {

/**
 * @brief LRU Cache with loader function support.
 *
 * This cache does not have an explicit/public put() function. Rather elements are loaded into the cache on demand
 * when get() is called and the key doesn't exist in the cache. The cache will invoke the provided function of the
 * cache loader provided.
 *
 * Base implementation is MT-SAFE.
 */
template<typename key_t, typename value_t>
class cache {
public:
	/**
	 * Key Value Pair for the list of items held in cache
	 */
	typedef typename std::pair<key_t, value_t> keyValuePair_t;
	/**
	 * Iterator of list entry, used in map to locate item in cache item list.
	 */
	typedef typename std::list<keyValuePair_t>::iterator listIterator_t;

	/**
	 * Create a new cache instance containing up to max elements.
	 *
	 * @param maxSize The maximum number of elements.
	 * @param loader Function pointer to load values for the cache, if the key doesn't exist.
	 */
	cache(uint64_t maxSize, std::function<value_t(key_t)> loader) :
			maxSize(maxSize), loader(loader) {
	}

	~cache() {
		invalidate();
	}

	/**
	 * Get the element from the cache.
	 *
	 * @param key The Key to get.
	 * @return The item from the cache.
	 */
	const value_t& get(const key_t& key) noexcept {
		std::lock_guard<std::recursive_mutex> lock(cacheLock);
		auto it = cacheMap.find(key);
		if (it == cacheMap.end()) {
			// element doesn't exist, so invoke the load function to acquire the element.
			value_t value = loader(key);
			put(key, value);
			// Simply get the key now.
			it = cacheMap.find(key);
			cacheItems.splice(cacheItems.begin(), cacheItems, it->second);
			return it->second->second;

		} else {
			cacheItems.splice(cacheItems.begin(), cacheItems, it->second);
			return it->second->second;
		}
	}

	/**
	 * Does the key exist within the cache
	 *
	 * @param key The key to look for.
	 * @return TRUE if the key exists.
	 */
	bool exists(const key_t& key) noexcept {
		std::lock_guard<std::recursive_mutex> lock(cacheLock);
		return cacheMap.find(key) != cacheMap.end();
	}

	/**
	 * Get the number of elements held by the cache.
	 * @return The number of elements held by the cache.
	 */
	uint64_t size() noexcept {
		std::lock_guard<std::recursive_mutex> lock(cacheLock);
		return cacheMap.size();
	}

	/**
	 * Invalidate the entire cache.
	 *
	 * @return TRUE if the operation succeeded.
	 */
	bool invalidate() noexcept {
		std::lock_guard<std::recursive_mutex> lock(cacheLock);
		cacheItems.clear();
		cacheMap.clear();
		return true;
	}

private:

	/**
	 * Insert the given element into the cache, and remove element if cache exceeds maxSize.
	 * <p>
	 * It is expected that the mapLock already be held before calling this method.
	 *
	 * @param key The key
	 * @param value The value.
	 */
	void put(const key_t& key, const value_t& value) {
		auto it = cacheMap.find(key);
		cacheItems.push_front(keyValuePair_t(key, value));
		if (it != cacheMap.end()) {
			cacheItems.erase(it->second);
			cacheMap.erase(it);
		}
		cacheMap[key] = cacheItems.begin();

		if (cacheMap.size() > maxSize) {
			auto last = cacheItems.end();
			last--;
			cacheMap.erase(last->first);
			cacheItems.pop_back();
		}
	}

	/**
	 * Lock for the cache. (use std::lock_guard to acquire).
	 *
	 * Note: Using recursive mutex to allow the cache loader function to reenter the cache if need be.
	 */
	std::recursive_mutex cacheLock;
	/**
	 * Collection of all list items.
	 */
	std::list<keyValuePair_t> cacheItems;
	/**
	 * Map of all keys, to iterator of entry in cacheItems.
	 */
	std::map<key_t, listIterator_t> cacheMap;
	/**
	 * The maximum number of entries for this cache.
	 */
	uint64_t maxSize;
	/**
	 * Function pointer for load function.
	 */
	std::function<value_t(key_t)> loader;
};

}/* namespace util */
}/* namespace aff4 */

#endif /* SRC_UTILS_CACHE_H_ */
