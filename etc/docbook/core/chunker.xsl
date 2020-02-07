<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                xmlns="http://www.w3.org/1999/xhtml"
                version="1.0"
                exclude-result-prefixes="exsl"
                extension-element-prefixes="exsl">

<xsl:param name="chunker.output.method" select="'xml'"/>
<xsl:param name="chunker.output.encoding" select="'UTF-8'"/>
<xsl:param name="chunker.output.indent" select="'no'"/>
<xsl:param name="chunker.output.omit-xml-declaration" select="'no'"/>
<xsl:param name="chunker.output.standalone" select="'no'"/>
<xsl:param name="chunker.output.doctype-public" select="'-//W3C//DTD XHTML 1.0 Transitional//EN'"/>
<xsl:param name="chunker.output.doctype-system" select="'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'"/>
<xsl:param name="chunker.output.media-type" select="''"/>
<xsl:param name="chunker.output.cdata-section-elements" select="''"/>

<xsl:variable name="chunk.base.dir">
  <xsl:choose>
    <xsl:when test="string-length($base.dir) = 0"/>
    <xsl:when test="substring($base.dir, string-length($base.dir), 1) = '/'">
      <xsl:value-of select="$base.dir"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="concat($base.dir, '/')"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:template name="make-relative-filename">
  <xsl:param name="base.dir" select="'./'"/>
  <xsl:param name="base.name" select="''"/>

  <xsl:choose>
    <xsl:when test="count(parent::*) = 0">
      <xsl:value-of select="concat($base.dir,$base.name)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$base.name"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="write.chunk">
  <xsl:param name="filename" select="''"/>
  <xsl:param name="suppress-context-node-name" select="0"/>

  <xsl:param name="method" select="$chunker.output.method"/>
  <xsl:param name="encoding" select="$chunker.output.encoding"/>
  <xsl:param name="indent" select="$chunker.output.indent"/>
  <xsl:param name="omit-xml-declaration" select="$chunker.output.omit-xml-declaration"/>
  <xsl:param name="standalone" select="$chunker.output.standalone"/>
  <xsl:param name="doctype-public" select="$chunker.output.doctype-public"/>
  <xsl:param name="doctype-system" select="$chunker.output.doctype-system"/>
  <xsl:param name="media-type" select="$chunker.output.media-type"/>
  <xsl:param name="cdata-section-elements" select="$chunker.output.cdata-section-elements"/>

  <xsl:param name="content"/>

  <!--
  <xsl:message>
    <xsl:text>Writing </xsl:text>
    <xsl:value-of select="$filename"/>
    <xsl:if test="name(.) != '' and $suppress-context-node-name = 0">
      <xsl:text> for </xsl:text>
      <xsl:value-of select="name(.)"/>
      <xsl:if test="@id or @xml:id">
        <xsl:text>(</xsl:text>
        <xsl:value-of select="(@id|@xml:id)[1]"/>
        <xsl:text>)</xsl:text>
      </xsl:if>
    </xsl:if>
  </xsl:message>
  -->
  
  <exsl:document href="{$filename}" method="{$method}" encoding="{$encoding}"
                 indent="{$indent}" omit-xml-declaration="{$omit-xml-declaration}"
                 cdata-section-elements="{$cdata-section-elements}"
                 media-type="{$media-type}" doctype-public="{$doctype-public}"
                 doctype-system="{$doctype-system}" standalone="{$standalone}">
    <xsl:copy-of select="$content"/>
  </exsl:document>
</xsl:template>

</xsl:stylesheet>
