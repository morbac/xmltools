#include "EmbeddedResources.h"
#include <wtypes.h>

std::string LoadEmbeddedResourceText(int assetId)
{
    std::string output;

    // If you need to retrieve resources embedded in a binary that is not the current running program, modify this
    // function to pass in a HMODULE value.
    HMODULE module = 0;// ::GetModuleHandle(L"SimpleXmlTests.exe");
    auto name = MAKEINTRESOURCE(assetId);

    auto resourceHandle = ::FindResource(module, name, RT_RCDATA);

    if (resourceHandle != nullptr)
    {
        auto dataHandle = ::LoadResource(module, resourceHandle);

        if (dataHandle != nullptr)
        {
            auto resourceSize = ::SizeofResource(module, resourceHandle);

            if (resourceSize != 0)
            {
                auto firstByte = reinterpret_cast<const char*>(::LockResource(dataHandle));

                if (firstByte != nullptr)
                {
                    output.resize(resourceSize);
                    std::copy(firstByte, firstByte + resourceSize, output.begin());

                    // No need to call ::FreeResource on any 32 or 64 bit version of Windows. See MSDN for details
                    // on why the call is not needed:
                    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms648044%28v=vs.85%29.aspx
                }
            }
        }
    }

    return output;
}