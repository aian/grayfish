<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns="http://www.w3.org/1999/xhtml"
                version="1.0">

  <xsl:param name="amazon.url" select="'http://www.amazon.co.jp/exec/obidos/ASIN/'"/>
  <xsl:param name="amazon.id" select="'qune-22'"/>

  
  <xsl:template name="biblioentry.title">
    <em>
      <xsl:choose>
        <xsl:when test="../biblioid[@class='isbn']">
          <a>
            <xsl:attribute name="href">
              <xsl:value-of select="$amazon.url"/>
              <xsl:value-of select="../biblioid[@class='isbn']"/>
              <xsl:text>/</xsl:text>
              <xsl:value-of select="$amazon.id"/>
            </xsl:attribute>
            <xsl:apply-templates mode="bibliography.mode"/>
          </a>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates mode="bibliography.mode"/>
        </xsl:otherwise>
      </xsl:choose>
    </em>
  </xsl:template>

</xsl:stylesheet>
