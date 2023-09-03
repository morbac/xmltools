This repo contains the ARM64-compiled variant of the original tool from @morbac

XMLTools
--------
This plugin is a small set of useful tools for editing XML with Notepad++. The plugin is based on MSXML. The plugin features are:
- XML syntax Check
- XML Schema (XSD) + DTD Validation
- XML tag autoclose
- Pretty print
- Linarize XML
- Current XML Path
- Conversion XML &amp;lt;-&amp;gt; Text
- Comment / Uncomment
- XPath expression evaluation

Author: Nicolas Crittin

Homepage: [https://github.com/morbac/xmltools](https://github.com/morbac/xmltools)

Plugin Usage - ARM64
------------
- You can download the latest release from [https://github.com/jglathe/xmltools/releases](https://github.com/jglathe/xmltools/releases)
- Create a folder XMLTools within Notepad++\Plugins (Typically C:\Program Files\Notepad++\plugins\) and copy the XMLTools.dll there
- Restart Notepad ++. Now you should be able to see the XMLTools menu within the Plugins section 

**Building instructions**
------------
- Build with Visual Studio 2022 on an ARM machine, select "ARM64" as target. Cross-building will probably fail on the post-build event (copying the newly compiled XMLTools.dll to the plugins\XMLTools folder), or at the latest if you try to use it in notepad++. You can disable it, of course, and do the copying manually to the right target.
  -  To make the copy process work notepad++ needs to be closed, and you need to have full access rights to the plugins\ directory.
-  The target ARM64EC and ARM64X doesn't work yet, something od with the #ifdefs in winnt.h
