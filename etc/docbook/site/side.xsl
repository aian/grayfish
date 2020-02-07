<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns="http://www.w3.org/1999/xhtml"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

  <xsl:template name="user.side.content">
    <!-- Ad : Google side-head@qune.jp -->
    <div class="side-box">
      <xsl:call-template name="ads.google.medium-rectangle"/>
    </div>
    <!-- Site Navigation -->
    <div class="side-container">
      <div class="side-head">Site Navigation</div>
      <div class="side-body">
        <ul class="side-navi">
          <li><a href="/" title="Home"><strong>HOME</strong></a></li>
        </ul>
      </div>
    </div>
    <!-- Ad : Amazon -->
    <div class="side-box">
      <xsl:call-template name="ads.a8.z-kai.medium-rectangle"/>
    </div>
    <!-- SNS Account -->
    <div class="side-container">
      <div class="side-head">SNS Accounts (@aian)</div>
      <div class="side-body">
        <ul class="side-navi">
          <li><a href="https://www.twitter.com/aian" title="aian@Twitter">aian @Twitter</a></li>
          <li><a href="https://www.mstdn.jp/@aian" title="aian@Twitter">aian @Mastodon.JP</a></li>
        </ul>
      </div>
    </div>
    <!-- Shortcut for project page -->
    <!--
    <div class="side-container">
      <div class="side-head">Projects</div>
      <div class="side-body">
        <ul class="side-navi">
          <li><a href="/cunit" title="CUNIT">CUNIT</a></li>
          <li><a href="/nini" title="NINI">NINI</a></li>
        </ul>
      </div>
    </div>
    -->
    <!-- Ad : Google side-bottom@qune.jp -->
    <div class="side-box">
      <xsl:call-template name="ads.google.half-page"/>
    </div>
  </xsl:template>
</xsl:stylesheet>
