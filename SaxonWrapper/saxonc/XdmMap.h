////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2025 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XDM_MAP_h
#define SAXON_XDM_MAP_h

#include "saxonc_export.h"
#include "saxonc/XdmArray.h"
#include "saxonc/XdmFunctionItem.h"
#include <string>

#include <map>
#include <set>
#include <stdlib.h>
#include <string.h>

/**
 * A map in the XDM data model. A map is a list of zero or more entries, each of
 * which is a pair comprising a key (which is an atomic value) and a value
 * (which is an arbitrary value). The map itself is an XDM item. <p>An XdmMap is
 * immutable.</p>
 * @since 11
 */
class SAXONC_EXPORT XdmMap : public XdmFunctionItem {

public:
  /**
   * Default constructor.
   * Creates an empty XdmMap
   */
  XdmMap();

  /**
   * XdmItem copy constructor.
   * @param d - XdmArray
   */
  XdmMap(const XdmMap &d);

  /**
   * Destructor method for XdmMap
   */
  virtual ~XdmMap() {}

  /**
   * XdmMap constructor to create an object which is a wrapper for a Java XdmMap object - internal use only
   * @param obj - internal Java XdmMap object to be wrapped
   */
  XdmMap(int64_t obj);

  /**
   * Create an XdmMap supplying the entries in the form of a std::map.
   * The keys and values in the std::map are XDM values
   * @param map - a std::map whose entries are the (key, value) pairs
   */
  XdmMap(std::map<XdmAtomicValue *, XdmValue *> map);

  /**
   * Get the number of entries in the map
   *
   * @return the number of entries in the map. (Note that the
   * <code>size()</code> method returns 1 (one), because an XDM map is an item.)
   */
  int mapSize();

  /**
   * Returns the value to which the specified key is mapped, or NULL if this map contains no mapping for the key.
   *
   * @param key - the key whose associated value is to be returned.
   * @return The value to which the specified key is mapped, or
   * NULL if this map contains no mapping for the key.
   * The caller is responsible for memory deallocation using `delete`.
   */
  XdmValue *get(XdmAtomicValue *key);

  /**
   * Returns the value to which the specified string-valued key is mapped, or
   * NULL if this map contains no mapping for the key.
   * This is a convenience method to save the trouble of converting the String
   * to an <code>XdmAtomicValue</code>.
   * @param key - the key whose associated value is to be returned. This is
   * treated as an instance of <code>xs:string</code> (which will also match
   *            entries whose key is <code>xs:untypedAtomic</code> or
   * <code>xs:anyURI</code>)
   * @return The value to which the specified key is mapped, or
   * NULL if this map contains no mapping for the key.
   * The caller is responsible for memory deallocation using `delete`.
   */
  XdmValue *get(const char *key);

  /**
   * Returns the value to which the specified integer-valued key is mapped or
   * NULL if this map contains no mapping for the key.
   * This is a convenience method to save the trouble of converting the integer
   * to an <code>XdmAtomicValue</code>.
   *
   * @param key - the key whose associated value is to be returned. This is
   * treated as an instance of <code>xs:integer</code> (which will also match
   *            entries whose key belongs to another numeric type)
   * @return The value to which the specified key is mapped, or NULL if this map
   * contains no mapping for the key.
   * The caller is responsible for memory deallocation using `delete`.
   */
  XdmValue *get(int key);

  /**
   * Returns the value to which the specified double-valued key is mapped, or
   * NULL if this map contains no mapping for the key.
   * This is a convenience method to save the trouble of converting
   * the double to an <code>XdmAtomicValue</code>.
   * @param key - the key whose associated value is to be returned. This is
   * treated as an instance of <code>xs:double</code> (which will also match
   *            entries whose key belongs to another numeric type)
   * @return The value to which the specified key is mapped, or
   * NULL if this map contains no mapping for the key.
   * The caller is responsible for memory deallocation using `delete`.
   */
  XdmValue *get(double key);

  /**
   * Returns the value to which the specified integer-valued key is mapped or
   * NULL if this map contains no mapping for the key.
   * This is a convenience method to save the trouble of converting the
   * integer to an <code>XdmAtomicValue</code>.
   * @param key - the key whose associated value is to be returned. This is
   * treated as an instance of <code>xs:integer</code> (which will also match
   *            entries whose key belongs to another numeric type)
   * @return The value to which the specified key is mapped, or
   * NULL if this map contains no mapping for the key.
   * The caller is responsible for memory deallocation using `delete`.
   */
  XdmValue *get(long key);

  /**
   * Create a new map containing an additional (key, value) pair.
   * If there is an existing entry with the same key, it is removed.
   * @param key - the key for the new entry
   * @param value - the value for the new entry
   * @return A new map containing the additional entry. The original map is
   * unchanged.
   * The caller is responsible for memory deallocation using `delete`.
   */
  XdmMap *put(XdmAtomicValue *key, XdmValue *value);

  /**
   * Create a new map in which the entry for a given key has been removed.
   * If there is no entry with the same key, the new map has the same content as
   * the old (it may or may not be the same object)
   * @param key - the key to be removed given as an XdmAtomicValue
   * @return A map without the specified entry. The caller is responsible for memory deallocation using `delete`.
   * The original map is unchanged.
   *
   */
  XdmMap *remove(XdmAtomicValue *key);

  /**
   * Get the keys present in the map in the form of an unordered std::set.
   * @return An unordered std::set of the keys present in this map, in arbitrary
   * order. The caller is responsible for memory deallocation.
   */
  std::set<XdmAtomicValue *> keySet();

  /**
   * Get the keys present in the map in the form of an unordered pointer array of XdmAtomicValues.
   * @return An unordered pointer array of the keys present in this map, in
   * arbitrary order. The caller is responsible for memory deallocation.
   */
  XdmAtomicValue **keys();

  // std::map<XdmAtomicValue*, XdmValue*> asMap();

  /**
   * Check if this map is empty.
   * @return True if this map contains no key-value mappings
   */
  bool isEmpty();

  /**
   * Returns <code>true</code> if this map contains a mapping for the specified key.
   * More formally, returns true if and only if
   * this map contains a mapping for a key k such that
   * <tt>(key==null ? k==null : key.equals(k))</tt>.  (There can be
   * at most one such mapping.)
   *
   * @param key - the key whose presence in this map is to be tested
   * @return True if this map contains a mapping for the specified key
   */
  bool containsKey(XdmAtomicValue *key);

  /**
   * Returns a std::list containing the values found in this map, that is, the value parts of the key-value pairs.
   *
   * @return A std::list containing the values found in this map. The result
   * may contain duplicates, and the ordering of the collection is
   * unpredictable. The caller is responsible for memory deallocation.
   */
  std::list<XdmValue *> valuesAsList();

  /**
   * Returns a pointer array containing the values found in this map, that is, the value parts of the key-value pairs.
   *
   * @return A pointer array containing the values found in this map. The result
   * may contain duplicates, and the ordering of the collection is
   * unpredictable. The caller is responsible for memory deallocation using `delete`.
   */
  XdmValue **values();

  /**
   * Determine whether the item is a function or some other type of item.
   * @return True; an XDM map is a function item
   */
  bool isFunction() { return true; }

  /**
   * Determine whether the item is an XDM map or some other type of item
   * @return True
   */
  bool isMap() { return true; }

  /**
   * Get the type of this XDM value
   * @return The type of the XdmValue as an XDM_TYPE
   */
  XDM_TYPE getType() { return XDM_MAP; }

  /**
   * Create a string representation of the XDM map. This is the result of serializing
   * the map using the adaptive serialization method.
   * @param encoding - the encoding of the string returned. If NULL or omitted defaults to the JVM encoding, which in most cases is UTF-8.
   * @return A string representation of the XdmMap.
   * The caller is responsible for memory deallocation using `operator delete`.
   */
  const char *toString(const char *encoding = nullptr);

};

#endif
