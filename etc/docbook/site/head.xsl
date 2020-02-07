<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns="http://www.w3.org/1999/xhtml"
                version="1.0">

  <xsl:template name="user.head.content">
    <!-- Highlight.js -->
    <script type="text/javascript">hljs.initHighlightingOnLoad();</script>
  </xsl:template>

  <xsl:template name="user.header.navigation">
    <xsl:element name="div">
      <xsl:attribute name="id">
        <xsl:text>head</xsl:text>
      </xsl:attribute>
      <!-- Ad : Google site-head@qune.jp -->
      <!--
      <xsl:call-template name="ads.google.big-banner"/>
      -->
      <!-- logo -->
      <xsl:element name="a">
        <xsl:attribute name="href">
          <xsl:text>/</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="title">
          <xsl:text>qune</xsl:text>
        </xsl:attribute>
        <xsl:text>qune</xsl:text>
      </xsl:element>
    </xsl:element>
  </xsl:template>

</xsl:stylesheet>
