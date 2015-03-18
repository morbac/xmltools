<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output encoding="UTF-8" version="1.0" method="xml" standalone="yes"/>
  <xsl:template match="/">
    <test>
      <xsl:value-of select="."/>
      <toto> </toto>
    </test>
  </xsl:template>
</xsl:stylesheet>