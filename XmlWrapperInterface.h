#pragma once

// WORK IN PROGRESS

/*
* This abstract class is the interface for external XML-API wrappers.
* The purpose of this interface is to define the expected methods for
* an XML-API wrapper.
*/

class XmlWrapperInterface {
public:
    XmlWrapperInterface() {}
    virtual ~XmlWrapperInterface() {}

    // expected functions might be:
    // - check xml syntax
    // - check xml validity
    // - current xml path --> might be done with simplexml or quickxml
    // - xpath evaluation
    // - xsl transform

    // get error detail
    // 
    virtual void method1() = 0;    // "= 0" part makes this method pure virtual, and
                                   // also makes this class abstract.
    virtual void method2() = 0;
};