<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns="http://www.w3.org/1999/xhtml"
                version="1.0">

  <!-- Big Banner (728 x 90) @ Google -->
  <!-- head@qune.jp -->
  <xsl:template name="ads.google.big-banner">
    <div class="ad big-banner">
      <xsl:if test="$ads.enabled">
        <script async="async"
                src="//pagead2.googlesyndication.com/pagead/js/adsbygoogle.js">
        </script>
        <ins class="adsbygoogle"
             style="display:inline-block;width:728px;height:90px"
             data-ad-client="ca-pub-4990801719339278"
             data-ad-slot="6679869328"></ins>
        <script>
          (adsbygoogle = window.adsbygoogle || []).push({});
        </script>
      </xsl:if>
    </div>
  </xsl:template>
  
  <!-- Medium Rectangle (300 x 250) @ Google -->
  <!-- side-head@qune.jp -->
  <xsl:template name="ads.google.medium-rectangle">
    <div class="ad medium-rectangle">
      <xsl:if test="$ads.enabled">
        <script async="async"
                src="//pagead2.googlesyndication.com/pagead/js/adsbygoogle.js">
        </script>
        <ins class="adsbygoogle"
             style="display:inline-block;width:300px;height:250px"
             data-ad-client="ca-pub-4990801719339278"
             data-ad-slot="7323244112"></ins>
        <script>
          (adsbygoogle = window.adsbygoogle || []).push({});
        </script>
      </xsl:if>
    </div>
  </xsl:template>
  
  <!-- Half Page (300 x 600) @ Google -->
  <!-- side-bottom@qune.jp -->
  <xsl:template name="ads.google.half-page">
    <div class="ad half-page">
      <xsl:if test="$ads.enabled">
        <script async="async"
                src="//pagead2.googlesyndication.com/pagead/js/adsbygoogle.js">
        </script>
        <ins class="adsbygoogle"
             style="display:inline-block;width:300px;height:600px"
             data-ad-client="ca-pub-4990801719339278"
             data-ad-slot="3451447716"></ins>
        <script>
          (adsbygoogle = window.adsbygoogle || []).push({});
        </script>
      </xsl:if>
    </div>
  </xsl:template>
  
  <!-- Medium Rectangle (300 x 250) @ Amazon -->
  <xsl:template name="ads.amazon.medium-rectangle">
    <div class="ad medium-rectangle">
      <xsl:if test="$ads.enabled">
        <iframe src="https://rcm-fe.amazon-adsystem.com/e/cm?o=9&amp;p=12&amp;l=ez&amp;f=ifr&amp;linkID=e65e81c5d1a4cf163a40af516ea52137&amp;t=qune-22&amp;tracking_id=qune-22" width="300" height="250" scrolling="no" border="0" marginwidth="0" style="border:none;" frameborder="0"></iframe>
      </xsl:if>
    </div>
  </xsl:template>

  <!-- Medium Rectangle (300 x 250) @ A8 Z-kai -->
  <xsl:template name="ads.a8.z-kai.medium-rectangle">
    <div class="ad medium-rectangle">
      <xsl:if test="$ads.enabled">
        <a href="https://px.a8.net/svt/ejp?a8mat=2ZATIK+3U4L4I+E0Q+15R4NL" target="_blank" rel="nofollow">
          <img border="0" width="300" height="250" alt="" src="https://www21.a8.net/svt/bgt?aid=180223724232&amp;wid=001&amp;eno=01&amp;mid=s00000001817007013000&amp;mc=1"/>
          <img border="0" width="1" height="1" src="https://www11.a8.net/0.gif?a8mat=2ZATIK+3U4L4I+E0Q+15R4NL" alt=""/>
        </a>
      </xsl:if>
    </div>
  </xsl:template>

</xsl:stylesheet>

