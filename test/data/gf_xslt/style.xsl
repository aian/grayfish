<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output doctype-public=""
              doctype-system=""
              media-type="application/xml"
              omit-xml-declaration="no"
              indent="yes"
              encoding="UTF-8"
              standalone="yes"
              version="1.0"/>

  <xsl:template match="/">
    <document>
      <xsl:apply-templates/>
    </document>
  </xsl:template>

  <xsl:template match="shelf">
    <body>
      <xsl:apply-templates/>
    </body>
  </xsl:template>

  <xsl:template match="book">
    <item>
      <xsl:apply-templates/>
    </item>
  </xsl:template>

  <xsl:template match="title">
    <subject><xsl:apply-templates/></subject>
  </xsl:template>

  <xsl:template match="rating">
    <star><xsl:apply-templates/></star>
  </xsl:template>

</xsl:stylesheet>
