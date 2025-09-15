#include "SaxonWrapper.h"

#include "saxonc/SaxonProcessor.h"

SaxonWrapper::SaxonWrapper(const char* xml, size_t size) {
    Report::char2BSTR(xml, size, this->m_sXml);
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
    return false;
}