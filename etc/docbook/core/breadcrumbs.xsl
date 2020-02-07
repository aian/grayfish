<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns="http://www.w3.org/1999/xhtml"
                version="1.0">

  <xsl:template name="breadcrumbs">
    <xsl:param name="this.node" select="."/>
    <div class="breadcrumbs">
      <xsl:for-each select="$this.node/ancestor::*">
        <span class="breadcrumb-link">
          <a>
            <xsl:attribute name="href">
              <xsl:call-template name="href.target">
                <xsl:with-param name="object" select="."/>
                <xsl:with-param name="context" select="$this.node"/>
              </xsl:call-template>
            </xsl:attribute>
            <xsl:apply-templates select="." mode="title.markup"/>
          </a>
        </span>
        <span class="breadcrumb-next">
          <xsl:text>&#x00BB;</xsl:text>
        </span>
      </xsl:for-each>
      <!-- the current node -->
      <span class="breadcrumb-node">
        <xsl:apply-templates select="$this.node" mode="title.markup"/>
      </span>
    </div>
  </xsl:template>

</xsl:stylesheet>
