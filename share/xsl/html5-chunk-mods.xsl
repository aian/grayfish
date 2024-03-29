<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:d="http://docbook.org/ns/docbook"
  xmlns:exsl="http://exslt.org/common"
  xmlns="http://www.w3.org/1999/xhtml"
  exclude-result-prefixes="exsl d"
  version="1.0">

<!-- call HTML5  header and footer templates for navigation -->
<xsl:template name="chunk-element-content">
  <xsl:param name="prev"/>
  <xsl:param name="next"/>
  <xsl:param name="nav.context"/>
  <xsl:param name="content">
    <xsl:apply-imports/>
  </xsl:param>

  <xsl:call-template name="user.preroot"/>

  <html>
    <xsl:call-template name="root.attributes"/>
    <xsl:call-template name="html.head">
      <xsl:with-param name="prev" select="$prev"/>
      <xsl:with-param name="next" select="$next"/>
    </xsl:call-template>

    <body>
      <xsl:call-template name="body.attributes"/>

      <xsl:call-template name="html5.header.navigation">
        <xsl:with-param name="prev" select="$prev"/>
        <xsl:with-param name="next" select="$next"/>
        <xsl:with-param name="nav.context" select="$nav.context"/>
      </xsl:call-template>

      <xsl:call-template name="user.header.content"/>

      <div id="body-content">
        <main>
          <xsl:copy-of select="$content"/>
        </main>
        <aside>
          <xsl:call-template name="user.side.content"/>
        </aside>
      </div>

      <xsl:call-template name="user.footer.content"/>

      <xsl:call-template name="html5.footer.navigation">
        <xsl:with-param name="prev" select="$prev"/>
        <xsl:with-param name="next" select="$next"/>
        <xsl:with-param name="nav.context" select="$nav.context"/>
      </xsl:call-template>

    </body>
  </html>
  <xsl:value-of select="$chunk.append"/>
</xsl:template>

<!-- Add HTML5 <header>  wrapper, and convert some attributes to styles -->
<xsl:template name="html5.header.navigation">
  <xsl:param name="prev" select="/d:foo"/>
  <xsl:param name="next" select="/d:foo"/>
  <xsl:param name="nav.context"/>

  <xsl:variable name="content">
    <header>
      <xsl:call-template name="user.header.navigation">
        <xsl:with-param name="prev" select="$prev"/>
        <xsl:with-param name="next" select="$next"/>
        <xsl:with-param name="nav.context" select="$nav.context"/>
      </xsl:call-template>

      <xsl:call-template name="header.navigation">
        <xsl:with-param name="prev" select="$prev"/>
        <xsl:with-param name="next" select="$next"/>
        <xsl:with-param name="nav.context" select="$nav.context"/>
      </xsl:call-template>
    </header>
  </xsl:variable>

  <!-- And fix up any style atts -->
  <xsl:call-template name="convert.styles">
    <xsl:with-param name="content" select="$content"/>
  </xsl:call-template>
</xsl:template>

<!-- Add HTML5 <footer>  wrapper, and convert some attributes to styles -->
<xsl:template name="html5.footer.navigation">
  <xsl:param name="prev" select="/d:foo"/>
  <xsl:param name="next" select="/d:foo"/>
  <xsl:param name="nav.context"/>

  <xsl:variable name="content">
    <!-- 2021.11.08 Removed the default footer
    <footer>
      <xsl:call-template name="user.footer.navigation">
        <xsl:with-param name="prev" select="$prev"/>
        <xsl:with-param name="next" select="$next"/>
        <xsl:with-param name="nav.context" select="$nav.context"/>
      </xsl:call-template>

      <xsl:call-template name="footer.navigation">
        <xsl:with-param name="prev" select="$prev"/>
        <xsl:with-param name="next" select="$next"/>
        <xsl:with-param name="nav.context" select="$nav.context"/>
      </xsl:call-template>
    </footer>
    -->
    <footer>
      <xsl:call-template name="user.tail.content"/>
    </footer>
  </xsl:variable>

  <!-- And fix up any style atts -->
  <xsl:call-template name="convert.styles">
    <xsl:with-param name="content" select="$content"/>
  </xsl:call-template>
</xsl:template>

</xsl:stylesheet>
