<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:d="http://docbook.org/ns/docbook"
                xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
                xmlns:dyn="http://exslt.org/dynamic"
                xmlns:saxon="http://icl.com/saxon"
                exclude-result-prefixes="doc dyn saxon d"
                version='1.0'>

<xsl:template name="log.message">
  <xsl:param name="level"/>
  <xsl:param name="source"/>
  <xsl:param name="context-desc"/>
  <xsl:param name="context-desc-field-length">12</xsl:param>
  <xsl:param name="context-desc-padded">
    <xsl:if test="not($context-desc = '')">
      <xsl:call-template name="pad-string">
        <xsl:with-param name="leftRight">right</xsl:with-param>
        <xsl:with-param name="padVar"
          select="substring($context-desc, 1, $context-desc-field-length)"/>
        <xsl:with-param name="length" select="$context-desc-field-length"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:param>
  <xsl:param name="message"/>
  <xsl:param name="message-field-length" select="45"/>
  <xsl:param name="message-padded">
    <xsl:variable name="spaces-for-blank-level">
      <!-- * if the level field is blank, we'll need to pad out -->
      <!-- * the message field with spaces to compensate -->
      <xsl:choose>
        <xsl:when test="$level = ''">
          <xsl:value-of select="4 + 2"/>
          <!-- * 4 = hard-coded length of comment text ("Note" or "Warn") -->
          <!-- * + 2 = length of colon-plus-space separator ": " -->
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="0"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="spaces-for-blank-context-desc">
      <!-- * if the context-description field is blank, we'll need -->
      <!-- * to pad out the message field with spaces to compensate -->
      <xsl:choose>
        <xsl:when test="$context-desc = ''">
          <xsl:value-of select="$context-desc-field-length + 2"/>
          <!-- * + 2 = length of colon-plus-space separator ": " -->
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="0"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="extra-spaces"
      select="$spaces-for-blank-level + $spaces-for-blank-context-desc"/>
    <xsl:call-template name="pad-string">
      <xsl:with-param name="leftRight">right</xsl:with-param>
      <xsl:with-param name="padVar"
        select="substring($message, 1, ($message-field-length + $extra-spaces))"/>
      <xsl:with-param name="length"
        select="$message-field-length + $extra-spaces"/>
    </xsl:call-template>
  </xsl:param>
  <!-- * emit the actual log message -->
  <xsl:message>
    <xsl:if test="not($level = '')">
      <xsl:value-of select="$level"/>
      <xsl:text>: </xsl:text>
    </xsl:if>
    <xsl:if test="not($context-desc = '')">
      <xsl:value-of select="$context-desc-padded"/>
      <xsl:text>: </xsl:text>
    </xsl:if>
    <xsl:value-of select="$message-padded"/>
    <xsl:text>  </xsl:text>
    <xsl:value-of select="$source"/>
  </xsl:message>
</xsl:template>

<!-- ===================================== -->
<xsl:template name="get.doc.title">
  <xsl:choose>
    <xsl:when test="//*[local-name() = 'title'
      or local-name() = 'refname']">
      <xsl:value-of select="//*[local-name() = 'title'
        or local-name() = 'refname'][1]"/>
    </xsl:when>
    <xsl:when test="substring(local-name(*[1]),
      string-length(local-name(*[1])-3) = 'info')
      and *[1]/*[local-name() = 'title']">
      <xsl:value-of select="*[1]/*[local-name() = 'title'][1]"/>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<!-- ===================================== -->
<xsl:template name="pad-string">
  <!-- * recursive template to right/left pad the value with -->
  <!-- * whatever padChar is passed in -->
  <xsl:param name="padChar" select="' '"/>
  <xsl:param name="leftRight">left</xsl:param>
  <xsl:param name="padVar"/>
  <xsl:param name="length"/>
  <xsl:choose>
    <xsl:when test="string-length($padVar) &lt; $length">
      <xsl:call-template name="pad-string">
        <xsl:with-param name="padChar" select="$padChar"/>
        <xsl:with-param name="leftRight" select="$leftRight"/>
        <xsl:with-param name="padVar">
          <xsl:choose>
            <!-- * determine whether string should be -->
            <!-- * right- or left-padded -->
            <xsl:when test="$leftRight = 'left'">
              <!-- * pad it to left -->
              <xsl:value-of select="concat($padChar,$padVar)"/>
            </xsl:when>
            <xsl:otherwise>
              <!-- * otherwise, right-pad the string -->
              <xsl:value-of select="concat($padVar,$padChar)"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:with-param>
        <xsl:with-param name="length" select="$length"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of 
        select="substring($padVar,string-length($padVar) - $length + 1)"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
