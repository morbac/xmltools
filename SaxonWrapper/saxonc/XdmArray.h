////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XDM_ARRAY_h
#define SAXON_XDM_ARRAY_h

#include "saxonc_export.h"
#include "saxonc/XdmFunctionItem.h"
#include <string>

#include <set>
#include <stdlib.h>
#include <string.h>

/**
 * An array in the XDM data model. An array is a list of zero or more members,
 * each of which is an arbitrary XDM value. The array itself is an XDM item.
 * <p>An XdmArray is immutable.</p>
 *
 */
class SAXONC_EXPORT XdmArray : public XdmFunctionItem {

public:
  /**
   * Default constructor.
   * Creates an empty XdmArray
   */
  XdmArray();

  /**
   * XdmArray copy constructor.
   * @param d - XdmArray
   */
  XdmArray(const XdmArray &d);

  /**
   * Destructor method for XdmArray
   */
  virtual ~XdmArray() {}

  /**
   * XdmArray constructor to create an object which is a wrapper for a Java XdmArray object - internal use only
   * @param obj - internal Java XdmArray object to be wrapped
   */
  XdmArray(int64_t obj);

  /**
   * XdmArray constructor to create an object which is a wrapper for a Java XdmArray object - internal use only
   * @param obj - internal Java XdmArray object to be wrapped
   * @param len - The length of the array if known
   */
  XdmArray(int64_t obj, int len);

  /**
   * Get the number of members in the array
   * @return The number of members in the array. (Note that the
   * <code>size()</code> method returns 1 (one), because an XDM array is an
   * item.)
   */
  int arrayLength();

  /**
   * Get the n'th member in the array, counting from zero.
   *
   * @param n - the member that is required, counting the first member in the
   * array as member zero
   * @return The n'th member in the sequence making up the array, counting from
   * zero. The caller is responsible for memory deallocation.
   * @remark If n is less than zero or greater than or equal to the number
   *                                    of members in the array we return null.
   */
  XdmValue *get(int n);

  /**
   * Create a new array in which one member is replaced with a new value.
   *
   * @param n - the position of the member that is to be replaced, counting the
   * first member in the array as member zero
   * @param value - the new member for the new array.
   * The value itself is not stored internally therefore safe for memory deallocation by the caller.
   * @return A new XdmArray, the same length as the original, with one member
   * replaced by a new value. The caller is responsible for memory deallocation.
   * @remark if n is less than zero or greater than or equal to the number
   *                                   of members in the array then return null
   */
  XdmArray *put(int n, XdmValue *value);

  /**
   * Append a new member to an array
   * @param value - the new member.
   * The value itself is not stored internally therefore safe for memory deallocation by the caller.
   * @return A new XdmArray, one item longer than the original
   * @remark If the value is lazily evaluated, and evaluation fails then return
   * null
   */
  XdmArray *addMember(XdmValue *value);

  /**
   * Concatenate another array
   *
   * @param value - the other array
   * The value itself is not stored internally therefore safe for memory deallocation by the caller.
   * @return A new XdmArray, containing the members of this array followed by the
   * members of the other array
   */
  XdmArray *concat(XdmArray *value);

  /**
   * Get the members of the array in the form of a list.
   * @return A std::list of the members of this array.
   */
  std::list<XdmValue *> asList();

  /**
   * Get the members of the XDM array in the form of a C++ array.
   * @return An array of the members of this XdmArray. The caller is responsible for memory deallocation.
   */
  XdmValue **values();

  /**
   * Get the arity of the function
   *
   * @return The arity of the function, that is, the number of arguments in the
   * function's signature (in this case, 1 (one))
   */
  int getArity() { return 1; }

  /**
   * Get the string value of the XdmArray item. There is no string value for
   * function items, so an exception is always thrown.
   * @param encoding - the encoding of the string returned. If NULL or omitted defaults to the JVM encoding, which in most cases is UTF-8.
   * @return Nothing is returned; this method always throws an exception because the XdmArray
   * has no associated string value.
   * @throws SaxonApiException
   */
  const char *getStringValue(const char *encoding = nullptr);

  /**
   * Determine whether the item is an XDM function or some other type of item
   *
   * @return True; an XDM array is a function item
   */
  bool isFunction() { return true; }

  /**
   * Get the type of this XDM value
   * @return The type of the XdmValue as an XDM_TYPE
   */
  XDM_TYPE getType() { return XDM_ARRAY; }

  /**
   * Determine whether the item is an XDM array or some other type of item
   *
   * @return True
   */
  bool isArray() { return true; }

  /**
   * Create a string representation of the XDM array. This is the result of serializing
   * the array using the adaptive serialization method.
   * @return A string representation of the XdmArray.
   * The caller is responsible for memory deallocation using `operator delete`.
   */
  const char *toString(const char *encoding = nullptr);

private:
  int arrayLen;
};

#endif
