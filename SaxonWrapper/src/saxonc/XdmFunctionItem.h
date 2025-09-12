////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XDMFUNCTIONITEM_h
#define SAXON_XDMFUNCTIONITEM_h

#include "saxonc_export.h"
#include "saxonc/XdmAtomicValue.h"
#include "saxonc/XdmItem.h"
#include "saxonc/XdmNode.h"

#include <string>

#include <stdlib.h>
#include <string.h>

/**
 * The class XdmFunctionItem represents a function item
 */
class SAXONC_EXPORT XdmFunctionItem : public XdmItem {

public:
  /**
   * Default constructor.
   * Creates an empty XdmFunctionItem
   */
  XdmFunctionItem();

  /**
   * XdmFunctionItem constructor to create an object which is a wrapper for a Java XdmFunctionItem object - internal use only
   * @param obj - internal Java XdmFunctionItem object to be wrapped
   */
  XdmFunctionItem(int64_t obj);

  /**
   * XdmFunctionItem copy constructor.
   * @param d - XdmFunctionItem
   */
  XdmFunctionItem(const XdmFunctionItem &d);

  /**
   * Destructor method for XdmFunctionItem
   */
  virtual ~XdmFunctionItem() {
    if (fname != nullptr) {
      delete fname;
    }
  }

  /**
   * Get the name of the function as an EQName.
   * The expanded name, as a string using the notation devised by EQName.
   * If the name is in a namespace, the resulting string takes the form
   * <code>Q{uri}local</code>. Otherwise, the value is the local part of the
   * name.
   *
   * @return The function name as a string in the EQName notation, or null for
   * an anonymous inline function item.
   * The caller is responsible for memory deallocation using `operator delete`.
   */
  const char *getName();

  /**
   * Get the arity of the function
   *
   * @return The arity of the function, that is, the number of arguments in the
   * function's signature
   */
  virtual int getArity();

  /**
   * Get the string value of the XdmFunctionItem. There is no string value for
   * function items, so an exception is always thrown.
   *
   * @return Nothing is returned; this method always throws an exception because the XdmFunctionItem
   * has no associated string value.
   * @throws SaxonApiException
   */
  const char *getStringValue(const char *encoding = nullptr);

  /**
   * Get a system function.
   * This can be any function defined in XPath 3.1 functions and operators,
   * including functions in the math, map, and array namespaces. It can also be
   * a Saxon extension function, provided a licensed Processor is used.
   *
   * @param processor - the Saxon Processor object required to get the system
   * function
   * @param name - the name of the function
   * @param arity - the number of arguments in the function
   * @return The requested function, or null if there is no such function. Note
   * that some functions (those with particular context dependencies) may be
   * unsuitable for dynamic calling. The caller is responsible for memory deallocation.
   */
  static XdmFunctionItem *getSystemFunction(SaxonProcessor *processor,
                                     const char *name, int arity);

  /**
   * Call the function
   *
   * @param processor - the SaxonProcessor object required in the call of the
   * function
   * @param arguments  - the values to be supplied as arguments to the function.
   * The "function conversion rules" will be applied to convert the arguments to
   * the required type when necessary.
   * @param argument_length - the length of the array of arguments
   * @return The result of calling the function. The caller is responsible for memory deallocation.
   */
  XdmValue *call(SaxonProcessor *processor, XdmValue **arguments,
                 int argument_length);

  /**
   * Determine whether the item is an atomic value or some other type of item
   * @return False
   */
  bool isAtomic() { return false; }

  /**
   * Get the type of this XDM value
   * @return The type of the XdmValue as an XDM_TYPE
   */
  XDM_TYPE getType() { return XDM_FUNCTION_ITEM; }

  /**
   * Determine whether the item is a function or some other type of item
   * @return True
   */
  bool isFunction() { return true; }

protected:
  XdmValue *getXdmValueSubClass(int64_t value); /*!< Creates the right type of result value */

  char *fname; /*!< The name of the function item */

private:
  int arity; /*!< The arity of this function */
};

#endif
