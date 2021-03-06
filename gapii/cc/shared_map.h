/*
 * Copyright (C) 2015 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * THIS FILE WAS GENERATED BY apic. DO NOT EDIT.
 */

#ifndef GAPII_SHARED_MAP_H
#define GAPII_SHARED_MAP_H

#include <memory>
#include <unordered_map>
#include <stdint.h>

namespace gapii {

// Represents map!(K,V) in the api language.
// It wraps std::shared_ptr<std::unordered_map<K,V>> and it must be non-null.
template<typename K, typename V>
class SharedMap {
 public:
  typedef std::unordered_map<K, V> Map;
  typedef typename Map::iterator iterator;
  typedef typename Map::const_iterator const_iterator;
  typedef typename Map::key_type key_type;
  typedef typename Map::value_type value_type;
  typedef typename Map::mapped_type mapped_type;

  SharedMap() : map(new Map()) { }

  V& operator[](const K& key) { return (*map)[key]; }
  iterator find(const K& key) { return map->find(key); }
  const_iterator find(const K& key) const { return map->find(key); };
  iterator begin() { return map->begin(); }
  const_iterator begin() const { return map->begin(); }
  iterator end() { return map->end(); }
  const_iterator end() const { return map->end(); }
  size_t count(const K& key) const { return map->count(key); }
  size_t size() const { return map->size(); }
  size_t erase(const K& key) { return map->erase(key); }
  bool empty() const { return map->empty(); }

  Map* get() { return map.get(); }
  const Map* get() const { return map.get(); }
  SharedMap<K,V> clone() const { return SharedMap<K,V>(new Map(*get())); }

 private:
  SharedMap(Map* m) : map(m) { }

  std::shared_ptr<Map> map;
};

} // namespace gapii
#endif // GAPII_SHARED_MAP_H
