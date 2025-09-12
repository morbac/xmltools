////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2025 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XDMITEM_h
#define SAXON_XDMITEM_h

#include "saxonc_export.h"
#include "saxonc/XdmValue.h"

class SaxonProcessor;

/**
 * The class XdmItem represents an item in a sequence, as defined by the XDM
 * data model. An item is either an atomic value, a node, or a function item.
 * <p>An item is a member of a sequence, but it can also be considered as a
 * sequence (of length one) in its own right. <tt>XdmItem</tt> is a subtype of
 * <tt>XdmValue</tt> because every item in the XDM data model is also a
 * value.</p> <p>It cannot be assumed that every sequence of length one will be
 * represented by an <tt>XdmItem</tt>. It is quite possible for an
 * <tt>XdmValue</tt> that is not an <tt>XdmItem</tt> to hold a singleton
 * sequence.</p> <p>Saxon provides a number of concrete subclasses of
 * <code>XdmItem</code>, namely XdmNode, XdmAtomicValue and XdmFunctionItem. Users must not
 * attempt to create additional subclasses.</p>
 */
class SAXONC_EXPORT XdmItem : public XdmValue {

public:
  /**
   * Default constructor.
   * Creates an empty XdmItem.
   */
  XdmItem();

  /**
   * XdmItem constructor to create an object which is a wrapper for a Java XdmItem object - internal use only
   * @param objRef - internal Java XdmItem object to be wrapped
   */
  explicit XdmItem(int64_t objRef);

  /**
   * XdmItem copy constructor.
   * @param item - XdmItem
   */
  XdmItem(const XdmItem &item);

  /**
   * Destructor method for XdmItem
   */
  virtual ~XdmItem();

  /**
   * Increment reference count of this XdmItem - internal use only.
   * This method is used for internal memory management.
   */
  virtual void incrementRefCount();

  /**
   * Decrement reference count of this XdmItem - internal use only.
   * This method is used for internal memory management.
   */
  virtual void decrementRefCount();

  /**
   * Determine whether the item is an atomic value or some other type of item
   *
   * @return True if the item is an atomic value, false if it is a node or a
   * function (including maps and arrays)
   */
  virtual bool isAtomic();

  /**
   * Determine whether the item is a node or some other type of item
   *
   * @return True if the item is a node, false if it is an atomic value or a
   * function (including maps and arrays)
   */
  virtual bool isNode();

  /**
   * Determine whether the item is an XDM function or some other type of item
   *
   * @return True if the item is an XDM function item (including maps and
   * arrays), false if it is an atomic value or a node
   */
  virtual bool isFunction();

  /**
   * Determine whether the item is an XDM map or some other type of item
   *
   * @return True if the item is an XDM map item, false if it is some other item
   */
  virtual bool isMap();

  /**
   * Determine whether the item is an XDM array or some other type of item
   *
   * @return True if the item is an XDM Array item, false if it is some other item
   */
  virtual bool isArray();

  /**
   * Get the underlying Java XdmValue object - internal use only
   * @return The Java object of the XdmValue in its JNI representation
   */
  virtual int64_t getUnderlyingValue();

  /**
   * Get the string value of the item.
   * For a node, this gets the string value of the node.
   * For an atomic value, it has the same effect as casting the
   * value to a string.
   * For a function item, there is no string value, so an exception is thrown.
   * In all cases the result is the same as applying the
   * XPath string() function. <p>For atomic values, the result is the same as
   * the result of calling <code>toString</code>. This is not the case for
   * nodes, where <code>toString</code> returns an XML serialization of the
   * node. The caller is responsible for memory deallocation using `operator delete`.</p>
   * @param encoding - the encoding of the string returned. If NULL or omitted it defaults to the JVM encoding, which in most cases is UTF-8.
   * @return The result of converting the item to a string.
   * @throws SaxonApiException if the item is a function
   */
  virtual const char *getStringValue(const char *encoding = nullptr);

  /**
   * Create a string representation of the item. This is the result of serializing
   * the item using the adaptive serialization method.
   * @param encoding - the encoding of the string returned. If NULL or omitted it defaults to the JVM encoding, which in most cases is UTF-8.
   * @return A string representation of the XdmItem.
   * The caller is responsible for memory deallocation using `operator delete`.
   */
  const char *toString(const char *encoding = nullptr);

  /**
   * Get the first item in the sequence consisting of just this item
   * @return This XdmItem
   */
  virtual XdmItem *getHead();

  /**
   * Get the n'th item in the sequence consisting of just this item, counting from zero.
   *
   * @param n - the item that is required, counting the first item in the sequence
   * as item zero
   * @return If n is zero, then return this XdmItem. Otherwise return nullptr
   * if n is not zero, or if the value is lazily
   * evaluated and the delayed evaluation fails with a dynamic error.
   * The caller is responsible for memory deallocation.
   */
  XdmItem *itemAt(int n);

  /**
   * Get the number of items in the sequence
   * @return The number of items in the XdmValue. For an XdmItem this is always 1 (one).
   */
  int size();

  /**
   * Get the type of this XDM value
   * @return The type of the XdmValue as an XDM_TYPE
   */
  virtual XDM_TYPE getType();

protected:

  const char *stringValue;  /*!< Cached. String representation of the XdmValue,
                               if available */
  const char *itemToString; /*!< Cached. String representation of the XdmValue,
                               if available */

private:
  bool operator==(const XdmItem& other) const
  {return false;}

};

#endif
