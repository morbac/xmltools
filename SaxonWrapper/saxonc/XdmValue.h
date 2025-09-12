////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2025 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XDMVALUE_H
#define SAXON_XDMVALUE_H

#include "saxonc_export.h"
#include "saxonc/SaxonCGlue.h"
#include "saxonc/SaxonCXPath.h"
#include "saxonc/SaxonProcessor.h"
#include <deque>
#include <list>
#include <string.h>
#include <typeinfo> //used for testing only
#include <vector>

class SaxonProcessor;
class XdmItem;
class XdmAtomicValue;
class XdmNode;

/** An <code>XdmValue</code> represents a value in the XDM data model. A value
 * is a sequence of zero or more items, each item being an atomic value, a node,
 * or a function item. This class is a wrapper of the XdmValue object created in
 * Java. <p/>
 */
class SAXONC_EXPORT XdmValue {

public:
  /**
   * Default constructor.
   * Creates an empty XdmValue
   */
  XdmValue() { initialize(); }

  /**
   * XdmValue copy constructor.
   * @param other - XdmValue
   */
  XdmValue(const XdmValue &other);

  /**
   * Add an XdmItem to the sequence.
   * See functions in SaxonCXPath of the C library
   * @param val - XdmItem object
   */
  void addXdmItem(XdmItem *val);

 virtual bool operator==(const XdmValue& other) const
 {
  std::cerr<<"C++ XdmValue equals operator called !!!!"<<std::endl;
  return false;
  /*char result = j_xdmNodeEquals(SaxonProcessor::sxn_environ->thread, (void *)value, (void *)other.value);
  if((int)result == SXN_EXCEPTION) {
   throw SaxonApiException();
  }
  return result != 0;*/
 }

  /**
   * Add an XdmItem to the sequence, when the sequence was returned from SaxonC - internal use only.
   * @param val - XdmItem object
   */
  void addXdmItemFromUnderlyingValue(XdmItem *val);

  /**
   * Add Java XdmValue object to the sequence.
   * See methods the functions in SaxonCXPath of the C library
   * @param val - Java object
   */
  void addUnderlyingValue(int64_t val);

  /**
   * A Constructor for handling XdmArray - internal use only.
   * Handles a sequence of XdmValues given as a  wrapped an Java XdmValue
   * object.
   * @param val - Java XdmValue object
   * @param arrFlag - Currently not used but allows for overloading of
   * constructor methods
   */
  XdmValue(int64_t val, bool arrFlag);

  /**
   * XdmValue constructor to create an object which is a wrapper for a Java XdmValue object - internal use only
   * @param val - internal Java XdmValue object to be wrapped
   */
  XdmValue(int64_t val);

  /**
   * Destructor method for XdmValue
   */
  virtual ~XdmValue();

  /**
   * Deprecated: this is deprecated and a no-op, the C++ destructor handles this case.
   * Delete the XdmValue object and clean up all items in the sequence. Release the underlying JNI object.
   * @deprecated
   */
  void releaseXdmValue();

  /**
   * Get the first item in the sequence represented by this XdmValue
   * @return The first XdmItem in the sequence, or nullptr if the sequence is empty.
   * Pointers to XdmItem objects have to be deleted in the calling program.
   * The caller is responsible for memory deallocation.
   */
  virtual XdmItem *getHead();

  /**
   * Get the n'th item in the sequence, counting from zero.
   *
   * @param n - the item that is required, counting the first item in the sequence
   * as item zero
   * @return The n'th item in the sequence making up the XdmValue, counting from
   * zero. Returns nullptr if n is less than zero or greater than or equal to the
   * number of items in the value.
   * Pointers to XdmItem objects have to be deleted in the calling program.
   * The caller is responsible for memory deallocation.
   */
  virtual XdmItem *itemAt(int n);

  /**
   * Get the number of items in the sequence
   * @return The number of items in the XdmValue.
   */
  virtual int size();

  /**
   * Create a string representation of the sequence. This is the result of serializing
   * the sequence using the adaptive serialization method.
   * @param encoding - the encoding of the string returned. If NULL or omitted defaults to the JVM encoding, which in most cases is UTF-8.
   * @return A string representation of the XdmValue.
   * The caller is responsible for memory deallocation using `operator delete`.
   * @throws SaxonApiException if encoding cannot be recognized
   */
  virtual const char *toString(const char *encoding = nullptr);

  /**
   * Get the number of references on this XdmValue - internal use only
   * This method is used for internal memory management.
   */
  int getRefCount() {
    if (getenv("SAXONC_DEBUG_FLAG")) {
      std::cerr << "getRefCount-xdmVal=" << refCount << " ob ref=" << (this)
                << std::endl;
    }
    return refCount;
  }

  /**
   * Increment reference count of this XdmValue - internal use only
   * This method is used for internal memory management.
   */
  virtual void incrementRefCount();

  /**
   * Decrement reference count of this XdmValue - internal use only
   * This method is used for internal memory management.
   */
  virtual void decrementRefCount();

  /**
   * Get the underlying Java XdmValue object - internal use only
   * @return The Graalvm reference to the Java object of the XdmValue.
   */
  virtual int64_t getUnderlyingValue();

  /**
   * Get the type of this XDM value
   * @return The type of the XdmValue as an XDM_TYPE
   */
  virtual XDM_TYPE getType();

  /**
    * Reset associated reference counts on XdmItems that have been relinquished - internal use only
    */
  void resetRelinquishedItems();

  /**
    * Increment the ref count for a relinquished item only once - internal use only
    */
  void incrementRefCountForRelinquishedValue(int i);

protected:
  /**
    * Initialize this XdmValue with default values
    */
  inline void initialize() {
    jValues = SXN_UNSET;
    refCount = 0;
    valueType = nullptr;
    xdmSize = 0;
    toStringValue = nullptr;
    values_cap = 0;
    values = nullptr;
    relinquished_values = nullptr;
    // relinquished_values[0] = 0;
  }

  char *valueType; /*!< Cached. The type of the XdmValue */

  XdmItem **values; /*!< Cached. XdmItems in the XdmValue */
  char *relinquished_values; /*!< Pointer to the array of items which have been relinquished */
  int values_cap; /*!< The number of items in the value */
  int xdmSize;  /*!< Cached. The number of items in the XdmValue */
  int refCount; /*!< The reference count of this XdmValue. If >1 this object
                   should not be deleted */
  int64_t value;            /*!< The Java XdmItem reference in Graalvm  */

private:
  char *toStringValue; /*!< Cached. String representation of the XdmValue, if
                          available */
  int64_t jValues;     /*!< If this XdmValue is a sequence we store values as a
                          ProcessorDataAccumulator object  */

  int addXdmItemToValue(XdmItem *val);
};

#endif /** SAXON_XDMVALUE_H **/
