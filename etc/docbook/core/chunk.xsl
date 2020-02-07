<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                xmlns="http://www.w3.org/1999/xhtml"
                version="1.0" exclude-result-prefixes="exsl">

<xsl:import href="docbook.xsl"/>
<xsl:import href="chunk-common.xsl"/>

<xsl:include href="chunk-code.xsl"/>

<!-- User cumstomization for qune.jp -->
<xsl:include href="breadcrumbs.xsl"/>

</xsl:stylesheet>
