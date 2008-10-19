================================================================
               XML Tools plugin for Notepad++
================================================================


This plugin is a small set of useful tools for editing  XML code
with Notepad++ v4.8.5 and following. The plugin features are:

  - XML syntax Check
  - XML Schema + DTD Validation
  - XML tag autoclose
  - Pretty print
  - Linarize XML
  - Current XML Path
  - Conversion XML <-> Text
  - Comment/Uncomment
  - XPath expression evaluation
  - XSL Transformation
  - XML Header recognition


XML Syntax Check
----------------
If enabled, it performs a XML syntax check AFTER each file save.
This means that your data is saved first by  Notepad++  and then
XML syntax is controlled.  A message box is  displayed if parser
finds error in the XML data.
The editor  automatically  goes  to line which is  referenced in
error message.
The XML syntax check can also be launched manually.


XML Schema + DTD Validation
---------------------------
Performs an XML validation against  XML Schema (*.xsd file)  and
DTD (*.dtd file). The function can also be  activated to execute
after each file save.

Note: The validation performs an XML syntax check automatically.
      So if you  activate the  auto-validation,  you should  not
      activate the auto-syntax check (or it will execute twice).

Note: By default XSD validation uses schema defined in attribute
      "xsi:noNamespaceSchemaLocation" of your xml root node. For
      instance:

      <?xml version="1.0" encoding="ISO-8859-1"?>
      <foo xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
           xsi:noNamespaceSchemaLocation="foobar.xsd">
        <bar>any value</bar>
      </foo>

      If it cannot find the attribute, the function opens a file
      selection dialog and you can enter the schema manually.
      However, the use of "noNamespaceSchemaLocation"  attribute
      is recommended if you enable auto-validation.

Note: DTD validation uses DTD declaration or definition given in
      DOCTYPE element before the  xml root node.  You can either
      specify external DTD using following syntax:

      <?xml version="1.0" encoding="ISO-8859-1"?>
      <!DOCTYPE foo SYSTEM "foobar.dtd">
      <foo>
        <bar>any value</bar>
      </foo>

      or define the DTD inline like this:

      <?xml version ="1.0"?>
      <!DOCTYPE message [
         <!ELEMENT foo (bar)>
         <!ELEMENT bar (#PCDATA)>
      ]>
      <foo>
        <bar>any value</bar>
      </foo>


XML tag autoclose
-----------------
Automatically adds closing tags during typing. The function uses
same code than Insertion plugin  available on Notepad++ download
page. It has been added to  XML Tools plugin  to avoid having to
add tons of plugins when one only want to edit XML source.


Pretty print
------------
Re-indent XML code automatically depending on nodes level. There
are actually three variants of Pretty print function:
  - Pretty print (XML only) will only reindent lines which
    start with XML tag
  - Pretty print (Text indent) will perform same work than
    previous one, but will also re-indent text. Example:
        <a>                       <a>
      hello         ====>           hello
        world                       world
        </a>                      </a>
  - Pretty print (XML only - with line breaks) will perform
    similar formatting than "XML only" one, but also adds
    carriage returns automatically. Example:
                                  <a>
      <a><b></b></a>    ====>       <b/>
                                  </a>

Linarize XML
------------
Reformat XML on one single line only. The current version of the
function is not compatible with CDATA blocs, and format comments
also.


Current XML Path
----------------
Opens a dialog box indicating the path of current node. The path
is also copied into clipboard.


Conversion XML <-> Text
-----------------------
Converts XML to text by translating the  '<' and '>'  characters
into '&lt;' and '&gt;'.  The characters  '&'  and  '"'  are also
converted respectively into  '&amp;'  and  '&quot;'. The reverse
operation is also available.
Note that conversion is performed on current selection only.


Comment / Uncomment
-------------------
One of most  constraining thing  in XML is comments.  While most
other languages support  several syntax  for comments,  XML only
uses  <!-- and -->  to define starting and ending comment blocs.
Therefore it is not possible to  comment a  portion of XML which
already contains a comment.
The  Comment and Uncomment  functions have been design to bypass
this limitation.  The function automatically transforms existing
comments so they are not recognized by XML parsers:
  - <!-- is converted into <!{x}** where x is a value
    corresponding to comment level
  - --> is converted into **{x}> where x is a value
    corresponding by comment start converted tag
Note that  Comment/Uncomment  is performed  on current selection
only.


XPath expression evaluation
---------------------------
Opens  a dialog  where  you  can  write a  XPath expression  and
evaluate it on current  XML source.  The result appears in lower
part of the dialog.


XSL Transformation
------------------
Transforms the current XML document using XSLT File. XSLT params
are also supported.


XML Header recognition
----------------------
This is a simple function which reads the 6 first chars of a the
being opened file and checks if it matches the string  '<?xml '.
If it does and current format type is 'Normal text',  it changes
to XML format. The function can be enabled or disabled in plugin
menu. It is disabled by default.


================================================================


Installation
------------
There are two ways for installing the plugin:
  1) using the auto-installer (EXE file)
  2) copying the files manually

For manual installation,  you have to copy the  plugin DLL  file
(xmltools.dll) in Notepad++ "plugin" subfolder.  The plugin also
requires following libXML dependances:
  iconv.dll
  libxml2.dll
  zlib1.dll
These DLL's  are quite common  and may  already be  installed on
your computer. If Notepad++ claims about missing DLL at startup,
simply copy missing DLL  either into Notepad++ root directory or
in windows system32 directory.


================================================================


Source code
-----------
Source code is available on demand at nicolas.crittin@gmail.com.


================================================================


Links
-----
Notepad++:         http://notepad-plus.sourceforge.net
libXML homepage:   http://xmlsoft.org
libXML win32 port: http://www.zlatkovic.com/projects/libxml


================================================================