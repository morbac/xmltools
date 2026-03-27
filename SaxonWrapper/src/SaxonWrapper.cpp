#include "SaxonWrapper.h"

#include "saxonc/SaxonProcessor.h"
#include "saxonc/XdmArray.h"
#include "saxonc/XdmAtomicValue.h"
#include "saxonc/XdmFunctionItem.h"
#include "saxonc/XdmItem.h"
#include "saxonc/XdmMap.h"
#include "saxonc/XdmNode.h"
#include "saxonc/XdmValue.h"

SaxonWrapper::SaxonWrapper(const char* xml, size_t size) {
    this->data = std::string(xml, size);
}

SaxonWrapper::~SaxonWrapper() {
    this->resetErrors();
}

void SaxonWrapper::loadOptions() {

}

void SaxonWrapper::saveOptions() {

}

int SaxonWrapper::getCapabilities() {
    return XmlCapabilityType::ALL_OPTIONS;
}

bool SaxonWrapper::checkSyntax() {
    bool res = true;
    this->resetErrors();

    SaxonProcessor* processor = new SaxonProcessor(false);
    XdmNode* doc = nullptr;

    if (processor != nullptr) {
        try {
            doc = processor->parseXmlFromString(this->data.c_str());
        }
        catch (SaxonApiException& e) {
            res = false;
            this->buildErrorsVector(e);
        }
    }
    else {
        res = false;
        this->errors.push_back({
                FALSE,
                0,
                0,
                0,
                L"Unable to initialize Saxon processor. Operation failed."
            });
    }

    if (processor != nullptr) delete processor;

    return res;
}

bool SaxonWrapper::checkValidity(std::wstring schemaFilename, std::wstring validationNamespace) {
    return false;
}

std::vector<XPathResultEntryType> SaxonWrapper::xpathEvaluate(std::wstring xpath, std::wstring ns) {
    std::vector<XPathResultEntryType> res;
    return res;
}

bool SaxonWrapper::xslTransform(std::wstring xslfile, XSLTransformResultType* out, std::wstring options, UniMode srcEncoding) {
    Xslt30Processor* xsltProc = NULL;

    bool res = true;

    this->resetErrors();

    SaxonProcessor* processor = new SaxonProcessor(false);
    if (processor != NULL) {
        xsltProc = processor->newXslt30Processor();
        if (xsltProc != NULL) {

        }
        else {
            this->errors.push_back({
                    FALSE,
                    0,
                    0,
                    0,
                    L"Unable to initialize Saxon XSLT3 processor. Operation failed."
                });
            res = false;
        }
    }
    else {
        this->errors.push_back({
                FALSE,
                0,
                0,
                0,
                L"Unable to initialize SaxonC. Operation failed."
            });
        res = false;
    }

    if (processor != NULL) delete processor;
    if (xsltProc != NULL) delete xsltProc;

    return res;
}

void SaxonWrapper::buildErrorsVector(SaxonApiException& exception, const wchar_t* szDesc) {
    this->resetErrors();

    ErrorEntryType err;
    err.positioned = TRUE;
    err.line = exception.getLineNumber() >= 0 ? exception.getLineNumber() : 0;
    err.linepos = 0;
    err.filepos = 0;
    err.reason = Report::widen(exception.getMessage());

    this->errors.push_back(err);
}