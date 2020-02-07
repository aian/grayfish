<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns="http://www.w3.org/1999/xhtml"
                version="1.0">

  <xsl:template name="user.tail.content">
    <xsl:text>copyright &#xa9; </xsl:text>
    <xsl:value-of select="//copyright[1]/year"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="//copyright[1]/holder"/>
    <xsl:text>.</xsl:text>
  </xsl:template>

</xsl:stylesheet>
