////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 - 2023 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/. This Source Code Form is
// "Incompatible With Secondary Licenses", as defined by the Mozilla Public
// License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SAXON_XDMNODE_h
#define SAXON_XDMNODE_h

#include "saxonc_export.h"
#include "saxonc/XdmItem.h"
#include <string.h>

typedef enum eXdmNodeKind {
  DOCUMENT = 9,
  ELEMENT = 1,
  ATTRIBUTE = 2,
  TEXT = 3,
  COMMENT = 8,
  PROCESSING_INSTRUCTION = 7,
  NAMESPACE = 13,
  UNKNOWN = 0
} XDM_NODE_KIND;

enum class SAXONC_EXPORT EnumXdmAxis {
  ANCESTOR = 0,
  ANCESTOR_OR_SELF = 1,
  ATTRIBUTE = 2,
  CHILD = 3,
  DESCENDANT = 4,
  DESCENDANT_OR_SELF = 5,
  FOLLOWING = 6,
  FOLLOWING_SIBLING = 7,
  NAMESPACE = 8,
  PARENT = 9,
  PRECEDING = 10,
  PRECEDING_SIBLING = 11,
  SELF = 12
};

class XdmValue;

/**
 * This class represents a node in the XDM data model. A Node is an
 * <code>XdmItem</code>, and is therefore an <code>XdmValue</code> in its own
 * right, and may also participate as one item within a sequence value. <p>The
 * XdmNode interface exposes basic properties of the node, such as its name, its
 * string value, and its typed value. </p> <p>Note that node identity cannot be
 * inferred from object identity. The same node may be represented by different
 * <code>XdmNode</code> instances at different times, or even at the same time.
 * The equals() method on this class can be used to test for node identity.</p>
 *
 */
class SAXONC_EXPORT XdmNode : public XdmItem {

public:
  /**
   * XdmNode constructor to create an object which is a wrapper for a Java XdmNode object - internal use only
   * @param objRef - internal Java XdmNode object to be wrapped
   */
  XdmNode(int64_t objRef);

  /**
   * XdmNode constructor to create an object which is a wrapper for a Java XdmNode object - internal use only
   * @param parent - the parent XdmNode to this node object
   * @param objRef - internal Java XdmNode object to be wrapped
   * @param kind - the kind of node, for example
   * <code>XdmNodeKind#ELEMENT</code> or <code>XdmNodeKind#ATTRIBUTE</code>
   */
  XdmNode(XdmNode *parent, int64_t objRef, XDM_NODE_KIND kind);

 /**
 * XdmNode == operator. Corresponds to the <code>equals()</code> relation -
 * which is true between two XdmNode objects if they both represent the same
 * node. That is, it corresponds to the "is" operator in XPath.
 * @param other - the object to be compared
 * @return True if and only if the other object is an XdmNode instance representing the same node
 */
 bool operator==(const XdmNode& other) const;

  /**
   * XdmNode copy constructor.
   * @param other - the node being copied
   */
  XdmNode(const XdmNode &other);

  /**
   * Destructor method for XdmNode
   */
  virtual ~XdmNode();

  /**
   * Determine whether the item is an atomic value or some other type of item
   * @return False
   */
  bool isAtomic();

  /**
   * Get the first item in the sequence consisting of just this item
   * @return This XdmNode
   */
  XdmItem *getHead();

 /**
  * The <code>equals()</code> relation between two XdmNode objects is true if they both represent the same
  * node. That is, it corresponds to the "is" operator in XPath.
  * @param other - the object to be compared
  * @return True if and only if the other object is an XdmNode instance representing the same node
  */
 bool equals(XdmNode * other);


  /**
   * Get the kind of the node.
   *
   * @return The kind of the node, for example <code>XdmNodeKind#ELEMENT</code> or
   * <code>XdmNodeKind#ATTRIBUTE</code>
   */
  XDM_NODE_KIND getNodeKind();

  /**
   * Get the name of the node, as a string in the form of a EQName. Memory deallocation is handled internally.
   *
   * @return The name of the node. In the case of unnamed nodes (for example,
   * text and comment nodes) return NULL.
   */
  const char *getNodeName();

  /**
   * Get the local name of the node. Memory deallocation is handled internally.
   *
   * @return The local name of the node. In the case of unnamed nodes (for example,
   * text and comment nodes) return nullptr.
   */
  const char *getLocalName();

  /**
   * Get the typed value of this node, as defined in XDM
   * @return The typed value. If the typed value is a single atomic value, this
   * will be returned as an instance of <code>XdmAtomicValue</code>
   */
  XdmValue *getTypedValue();

  /**
   * Get the line number of the node in a source document.
   * @return The line number of the node, or -1 if not available.
   */
  int getLineNumber();


  /**
   * Get the column number of the node in a source document.
   * @return The column number of the node, or -1 if not available.
   */
  int getColumnNumber();

  /**
   * Get the base URI of this node. Memory deallocation is handled internally.
   * @return The base URI, as defined in the XDM model. The value may be null if
   * no base URI is known for the node, for example if the tree was built from a
   * StreamSource with no associated URI, or if the node has no parent.
   */
  const char *getBaseUri();

  /**
   * Get the string value of the node.
   * The result is the same as applying the XPath string() function.
   * <p>For atomic values, the result is the same as the
   * result of calling <code>toString</code>. This is not the case for nodes,
   * where <code>toString</code> returns an XML serialization of the node.</p>
   * <p>The caller is responsible for memory deallocation using `operator delete`.</p>
   * @param encoding - the encoding of the string returned. If NULL or omitted defaults to the JVM encoding, which in most cases is UTF-8.
   * @return The string value of the node.
   */
  const char *getStringValue(const char *encoding = nullptr);


  /**
   * The toString() method returns a simple XML serialization of the node with defaulted serialization parameters.
   * <p>In the case of an element node, the result will be a well-formed
   * XML document serialized as defined in the W3C XSLT/XQuery serialization
   * specification, using options method="xml", indent="yes",
   * omit-xml-declaration="yes".</p> <p>In the case of a document node, the
   * result will be a well-formed XML document provided that the document node
   * contains exactly one element child, and no text node children. In other
   * cases it will be a well-formed external general parsed entity.</p> <p>In
   * the case of an attribute node, the output is a string in the form
   * <code>name="value"</code>. The name will use the original namespace
   * prefix.</p> <p>In the case of a namespace node, the output is a string in
   * the form of a namespace declaration, that is <code>xmlns="uri"</code> or
   * <code>xmlns:pre="uri"</code>.</p> <p>Other nodes, such as text nodes,
   * comments, and processing instructions, are represented as they would appear
   * in lexical XML. Note: this means that in the case of text nodes, special
   * characters such as <code>&amp;</code> and <code>&lt;</code> are output in
   * escaped form. To get the unescaped string value of a text node, use
   * <code>getStringValue()</code> instead.</p>
   *
   * <p>The caller is responsible for memory deallocation using `operator delete`.</p>
   * @param encoding - the encoding of the string returned. If NULL or omitted defaults to the JVM encoding, which in most cases is UTF-8.
   * @return A simple XML serialization of the node. Under error conditions the
   * method may return an error message which will always begin with the label
   * "Error: ".
   */
  const char *toString(const char *encoding = nullptr);

  /**
   * Get the parent of this node
   *
   * @return The parent of this node (a document or element node), or NULL if
   * this node has no parent.
   */
  XdmNode *getParent();

  /**
   * Get the string value of a named attribute (in no namespace) of this element.
   * The caller is responsible for deallocation of memory for the attribute nodes using `operator delete`.
   * @param name - the name of the required attribute, interpreted as a
   * no-namespace name
   * @return NULL if this node is not an element, or if this element has no
   * attribute with the specified name. Otherwise return the string value of the
   * selected attribute node.
   */
  const char *getAttributeValue(const char *name);

  /**
   * Get the number of attribute nodes for this node
   * @return The number of attributes on this node; returns zero if
   * this node has no attributes or is not an element node.
   */
  int getAttributeCount();

  /**
   * Get the attribute nodes of this element as an array.
   *
   * @param cache - deprecated.
   * The caller is responsible for deallocation of memory for the attribute nodes using `delete`.
   * @return NULL if this node is not an element node, or if this element has no
   * attributes. Otherwise return the attribute nodes as
   * a pointer array.
   */
  XdmNode **getAttributeNodes(bool cache = false);

  /**
  * Get the array of nodes reachable from this node via a given axis.
  * The caller is responsible for deallocation of memory for the attribute nodes using `delete`.
  * @param axis - identifies which axis is to be navigated. Axis options are as follows:
  * ANCESTOR = 0, ANCESTOR_OR_SELF = 1, ATTRIBUTE = 2, CHILD = 3,
  * DESCENDANT  = 4, DESCENDANT_OR_SELF = 5, FOLLOWING = 6,
  * FOLLOWING_SIBLING = 7, NAMESPACE = 8, PARENT = 9, PRECEDING = 10,
  * PRECEDING_SIBLING = 11, SELF = 12
  * @return An array of nodes on the specified axis, starting from this node as
  * the context node.
  * The nodes are returned in axis order, that is, in document order for a
  * forwards axis and in reverse document order for a reverse axis.
  */
  XdmNode **axisNodes(EnumXdmAxis axis);

  /**
   * Convert an int into the corresponding EnumXdmAxis Enum object
   *
   * @param n - the integer value for the XDM axis
   * @return The corresponding EnumXdmAxis object
   */
  EnumXdmAxis convertEnumXdmAxis(int n) { return static_cast<EnumXdmAxis>(n); }

  /**
   * Get the number of nodes in the <code>nodeAxis</code> array, cached from the last call to axisNodes
   */
  int axisNodeCount();

  /**
   * Get the underlying Graalvm Java object for the XdmNode - internal use only
   * @return The unwrapped Graalvm object for the XdmNode
   */
  int64_t getUnderlyingValue() { return XdmItem::getUnderlyingValue(); }

  /**
   * Determine whether the item is a node or some other type of item
   *
   * @return True
   */
  bool isNode() { return true; }

  /**
   * Get all the child nodes of the current node.
   * The caller is responsible for deallocation of memory using `delete`.
   * @param cache - deprecated and will be removed in a later version.
   * Caching is no longer used, the value of cache is ignored.
   * @return Pointer array of XdmNode objects representing the child nodes
   */
  XdmNode **getChildren(bool cache = false);

  /**
   * Get the ith child node of the current node.
   * The caller is responsible for deallocation of memory using `delete`.
   * @param i - the index of the required child node
   * @param cache - deprecated and will be removed in a later version.
   * Caching is no longer used, the value of cache is ignored.
   * @return Pointer to the ith child node
   */

  XdmNode *getChild(int i, bool cache = false);

  /**
   * Get the number of child nodes of the current node.
   * @return The number of child nodes as an int
   */
  int getChildCount();

  /**
   * Get the type of this XDM value
   * @return The type of the XdmValue as an XDM_TYPE
   */
  XDM_TYPE getType() { return XDM_NODE; }


    /**
      * Reset associated reference counts on XdmItems that have been relinquished - internal use only
      */
    void resetRelinquishedChildren();

    /**
      * Increment the ref count for a relinquished child only once - internal use only
      */
    void incrementRefCountForRelinquishedChildren();

    /**
     * Check if child nodes have been relinquished - internal use only
     */

    bool hasRelinquishedChildren();

  /**
   * Utility method required for the python and PHP extensions to delete a string created in the C++ code-base - internal use only
   * @param fetchedChildren - the array of pointers to child nodes to be deleted
   */
  static void deleteChildrenArray(XdmNode **fetchedChildren) {
    if (fetchedChildren != nullptr) {
      delete[] fetchedChildren;
      fetchedChildren = nullptr;
    }
  }


private:
  const char *baseURI;   /*!< The base URI of this node*/
  const char *nodeName;  /*!< The name of the node, in clark name format*/
  const char *localName; /*!< The local part of the name */
  XdmNode **children; /*!< Cached child nodes when getChildren method is first
                         called; */
  int childCount;     /*!< The number of child nodes on the current node*/
  int axisCount;
  XdmNode **attrValues; /*!< Cached attribute nodes when getAttributeNodes
                           method is first called; */
  int attrCount; /*!< The number of attribute nodes of the current node */
  XDM_NODE_KIND nodeKind; /*!< The node kind for the current node*/
};

#endif
