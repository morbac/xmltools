////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XDMATOMICVALUE_h
#define SAXON_XDMATOMICVALUE_h

#include "saxonc_export.h"
#include "saxonc/XdmItem.h"
#include <string>

#include <stdlib.h>
#include <string.h>

/**
 * The class XdmAtomicValue represents an item in an XPath sequence that is an
 * atomic value. The value may belong to any of the 19 primitive types defined
 * in XML Schema, or to a type derived from these primitive types, or the XPath
 * type xs:untypedAtomic. The type may be either a built-in type or a
 * user-defined type. <p>An <code>XdmAtomicValue</code> is immutable.</p>
 */
class SAXONC_EXPORT XdmAtomicValue : public XdmItem {

public:
  /**
   * Default constructor.
   * Creates an empty atomic value
   */
  XdmAtomicValue();

  /**
   * XdmAtomicValue copy constructor.
   * @param other - XdmAtomicValue
   */
  XdmAtomicValue(const XdmAtomicValue &other);

  /**
   * Destructor method for XdmAtomicValue
   */
  virtual ~XdmAtomicValue();

  /**
   * XdmAtomicValue constructor to create an object which is a wrapper for a Java XdmAtomicValue object - internal use only
   * @param val - internal Java XdmAtomicValue object to be wrapped
   */
  XdmAtomicValue(int64_t val);

  /**
   * XdmAtomicValue constructor to create an object which is a wrapper for a Java XdmAtomicValue object - internal use only
   * @param val - internal Java XdmAtomicValue object to be wrapped
   * @param ty - type of the XdmAtomicValue
   */
  XdmAtomicValue(int64_t val, const char *ty);

  /**
   * Get the primitive type of this atomic value, as a QName. The primitive types for this purpose are
   * the 19 primitive types of XML Schema, plus xs:integer, xs:dayTimeDuration
   * and xs:yearMonthDuration, and xs:untypedAtomic. For external objects, the
   * result is xs:anyAtomicType.
   *
   * @return A string of the QName naming the primitive type of this atomic
   * value. This will always be an atomic type. Memory deallocation is handled internally.
   */
  const char *getPrimitiveTypeName();

  /**
   * Get the value converted to a boolean using the XPath casting rules
   *
   * @return the result of converting to a boolean (Note: this is not the same
   * as the effective boolean value).
   * @remark false if the value cannot be cast to a boolean. Exception will be
   * thrown which can be retrieved via the getException method on the Processor
   */
  bool getBooleanValue();

  /**
   * Get the value converted to a double using the XPath casting rules.
   * <p>If the value is a string, the XSD 1.1 rules are used, which means that
   * the string
   * "+INF" is recognised.</p>
   *
   * @return the result of converting to a double
   * @remark zero if the value cannot be cast to a double. Exception will be
   * thrown which can be retrieved via the getException method on the Processor
   */
  double getDoubleValue();

  /**
   * Get the value converted to an integer using the XPath casting rules
   *
   * @return the result of converting to an integer
   * @remark zero if the value cannot be cast to an integer.  Exception will be
   * thrown which can be retrieved via the getException method on the Processor
   */
  long getLongValue();

  /**
   * Set the type annotation of the atomic value
   * @param ty - the annotated type given as a string
   */
  void setType(const char *ty);

  /**
   * Get the first item in the sequence consisting of just this item
   * @return This XdmAtomicValue
   */
  XdmItem *getHead();

  /**
   * Determine whether the item is an atomic value or some other type of item
   *
   * @return True
   */
  bool isAtomic() { return true; }

  /**
   * Get a hashcode that reflects the rules for equality matching
   * @return A suitable hashcode
   */
  int getHashCode();

  /**
   * Get the type of this XDM value
   * @return The type of the XdmValue as an XDM_TYPE
   */
  XDM_TYPE getType() { return XDM_ATOMIC_VALUE; }

private:
  std::string valType; /*!< The type of this XdmAtomicValue */
};

#endif
