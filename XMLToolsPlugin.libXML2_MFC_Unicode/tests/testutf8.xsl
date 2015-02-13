<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output encoding="UTF-8"
              version="1.0"
              method="xml"
              standalone="yes"/>

  <xsl:template match="/">
    <test>
      <xsl:value-of select="."/>
    </test>
  </xsl:template>
</xsl:stylesheet>