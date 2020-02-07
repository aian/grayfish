<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns="http://www.w3.org/1999/xhtml"
                version="1.0">

  <xsl:import href="../core/chunk.xsl"/>

  <xsl:import href="./ads.xsl"/>
  <xsl:import href="./head.xsl"/>
  <xsl:import href="./side.xsl"/>
  <xsl:import href="./tail.xsl"/>
  <xsl:import href="./biblio.xsl"/>

  <xsl:param name="ads.enabled"                select="0"             />
  <xsl:param name="html.script"                select="'https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-MML-AM_HTMLorMML https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.12.0/highlight.min.js'"/>
  <xsl:param name="html.stylesheet"            select="'/_/style.css'"/>
  <xsl:param name="img.src.path"               select="'_/'"          />
  <xsl:param name="suppress.navigation"        select="0"             />
  <xsl:param name="header.hr"                  select="0"             />
  <xsl:param name="header.rule"                select="0"             />
  <xsl:param name="footer.rule"                select="0"             />
  <xsl:param name="make.clean.html"            select="1"             />
  <xsl:param name="docbook.css.link"           select="0"             />
  <xsl:param name="docbook.css.source"         select="''"            />
  <xsl:param name="abstract.notitle.enabled"   select="1"             />
  <xsl:param name="direction.align.start"      select="left"          />
  <!-- Admonition: caution, note, tip, etc ...   -->
  <xsl:param name="admon.graphics"             select="1"             />
  <xsl:param name="admon.graphics.extension"   select="'.png'"        />
  <xsl:param name="admon.graphics.path"        select="'/_/'"         />
  <xsl:param name="admon.style"/>              
  <xsl:param name="admon.textlabel"            select="1"             />
  <!-- Do not chunk by section -->             
  <xsl:param name="chunk.first.sections"       select="0"             />
  <xsl:param name="chunk.section.depth"        select="0"             />

  <xsl:param name="part.autolabel"             select="'I'"           />
  <xsl:param name="reference.autolabel"        select="'I'"           />
  <xsl:param name="chapter.autolabel"          select="1"             />
  <xsl:param name="appendix.autolabel"         select="'A'"           />

  <xsl:param name="formal.object.break.after"  select="0"             />

  <xsl:param name="bibliography.numbered"      select="1"             />

  <xsl:param name="formal.title.placement">
    figure    after
    example   before
    equation  before
    table     before
    procedure before
  </xsl:param>

</xsl:stylesheet>
