#ifndef SAXON_API_EXCEPTION_H
#define SAXON_API_EXCEPTION_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "saxonc_export.h"
#include "saxonc/SaxonProcessor.h"
#include <exception>
#include <iostream>

class SaxonProcessor;

/** <code>SaxonApiException</code>. An exception thrown by the Saxon s9api API
 * (Java). This is always a C++ wrapper for some other underlying exception in
 * Java.
 */
class SAXONC_EXPORT SaxonApiException : public std::exception {

public:
  /**
   * Default constructor. Creates a SaxonApiException
   */
  SaxonApiException(bool staticError = false);

  /**
   * A constructor. Create a SaxonApiException
   * @param err - The detail message string of this throwable.
   */
  SaxonApiException(const char *err);

  /**
   * SaxonApiException copy constructor.
   * @param ex - the exception object to copy
   */
  SaxonApiException(const SaxonApiException &ex);

  /** Returns a pointer to the (constant) error description.
   *  @return A pointer to a const char*. The underlying memory
   *          is in possession of the Exception object. Callers must
   *          not attempt to free the memory.
   */
#ifdef _LIBCPP___EXCEPTION_EXCEPTION_H
 virtual char const* what() const _NOEXCEPT;
#else
 virtual const char *what();
#endif



  /**
   * A constructor. Create a SaxonApiException
   * @param message - The detail message string of this throwable.
   * @param errorCode - The error code of the underlying exception thrown, if
   * known
   * @param systemId - Get the URI of the module associated with the exception,
   * if known.
   * @param linenumber - The line number in the stylesheet related to cause of
   * the exception
   */
  SaxonApiException(const char *message, const char *errorCode,
                    const char *systemId, int linenumber);

  /**
   * A destructor.
   */
  virtual ~SaxonApiException() throw();

  /**
   * Get the error code associated with the current exception, if it exists.
   * The caller does not need to deallocate memory.
   * @return the associated error code, or null if no error code is available.
   * Memory deallocation is handled internally.
   */
  const char *getErrorCode();

  /**
   * Get the line number associated with the exception, if known.
   * @return the line number (typically of a line in a stylesheet, query, or
   * schema, but in the case of validation errors it may be a line in a source
   * document); or -1 if not known
   */
  int getLineNumber();

  /**
   * Returns the detail message string as a char array of the throwable, if
   * there is one. The caller does not need to deallocate memory.
   * @return the detail message string of this <tt>Throwable</tt> instance
   *         (which may be <tt>null</tt>). Memory deallocation is handled internally.
   */
  const char *getMessage();

  /**
   * Returns the detail message string as a char array of the throwable, if
   * there is one.  The caller does not need to deallocate memory.
   * @return the detail message string of this <tt>Throwable</tt> instance with
   * the error code as prefix if there is an error code associated with the
   * exception. Memory deallocation is handled internally.
   */
  const char *getMessageWithErrorCode();

  /**
   * Returns static error messages resulting from compile errors as a char array of the throwable, if
   * there are any.  The caller does not need to deallocate memory.
   * @return the detail message string of this <tt>Throwable</tt> instance with
   * the error code as prefix if there is an error code associated with the
   * exception. Memory deallocation is handled internally.
   */
  const char *getCombinedStaticErrorMessages();


  /**
   * Returns the number of static errors encountered
   */
  int staticErrorCount();

  /**
   * Get the URI of the module associated with the exception, if known. Memory deallocation is handled internally.
   */
  const char *getSystemId();

private:
  bool cppException;
  bool isStaticError;
  std::string message;
  std::string s_message;
  std::string ec_message;
  int lineNumber;
  std::string errorCode;
  std::string systemId;
};

#endif /* SAXON_API_EXCEPTION_H */
