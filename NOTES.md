TECHNICAL NOTES
===============

This is a technical notes for Grayfish.


SUB-COMMANDS
------------

### Overview

  <dl>
    <dt>main</dt>
    <dd>
      <p>The pseudo-command, which represents a top-level process. This sub-command is called at first internally. Processed command-line arguments and so on, it dispatches each concrete sub-commands. Users can't specify this in command line.</p>
    </dd>
    <dt>help</dt>
    <dd>
      <p>Shows help.</p>
    </dd>
    <dt>version</dt>
    <dd>
      <p>Shows version number.</p>
    </dd>
    <dt>setup</dt>
    <dd>
      <p>The sub-command, which sets up the new project directory in the current path.</p>
    </dd>
    <dt>config</dt>
    <dd>
      <p>The sub-command, which shows or edits the local config file. The file `gf.conf` is located at `%PROJECT%\.gf\gf.conf` by default and this is created when the `setup` command is called. The Grayfish config values are overridden in the following order.</p>
      <ol>
        <li>Internally defined values</li>
        <li>Global config file values</li>
        <li>Local project config values</li>
      </ol>
      <p>Because Grayfish uses the internally defined values at last, it can process without the local config file. So Grayfish don't stop processing even if the file doesn't exist (just shows warnings).</>
    </dd>
    <dt>update</dt>
    <dd>
      <p>The sub-command, which traverses the source directory and build up the content database file `site.xml`. The `site.xml` is a XML formatted file that is used for building your website. In addition, this file can be used for the input of the user template file (written in XSLT). Users can use this file to make up a content summary in your website.</p>
    </dd>
    <dt>build</dt>
    <dd>
      <p>The sub-command, which build up the website. Once you converted the source file, this sub-command does not convert as long as the files are not edited. To build your website, it is necessary to call `update` command in order to create `site.xml` file. If `site.xml` file doesn't exists, the `update` command is called internally.</p>
    </dd>
    <dt>clean</dt>
    <dd>The sub-command, which cleans up entire the website directory. The entries of `site.xml` will be marked as 'NOT BUILT'. Because you must re-build the source files, it is not recommended to run this without specific ideas. </dd>
    <dt>list</dt>
    <dd>The sub-command, which lists the document entries and their statuses. This sub-command uses `site.xml` database file. So if this file doesn't exists, sub-command `update` is automatically called.</dd>
    <dt>serve</dt>
    <dd>[EXPERIMENTAL] The sub-command, which brings up the server process. This server process watches over the source file directory and automatically calls appropriate sub-commands along with the changes of this directory.</dd>
  </dl>

### PREREQUISITES

  There is the prerequisites of each sub-command. If it is 


  | SUB-COMMAND | PROJECT PATH | SITE FILE | LOCAL CONF | SRC PATH | DST PATH |
  |:------------|:------------:|:---------:|:----------:|:--------:|:--------:|
  | main        |      X       |     X     |     X      |    X     |    X     |
  | help        |      X       |     X     |     X      |    X     |    X     |
  | version     |      X       |     X     |     X      |    X     |    X     |
  | setup       |      X       |     X     |     X      |    X     |    X     |
  | config      |      O       |     X     |     X(*2)  |    X     |    X     |
  | update      |      O       |     X     |     X(*2)  |    O     |    X     |
  | build       |      O       |     X(*1) |     X(*2)  |    O     |    O     |
  | clean       |      O       |     X     |     X      |    X     |    X     |
  | list        |      O       |     O(*1) |     X      |    X(*3) |    X     |
  | serve       |      O       |     O(*1) |     X(*2)  |    O     |    O     |


(*1) Required, however, if the file was not found, this sub-command internally
     calls `update' command to create it.

(*2) Not required to process, however, it is substantially useless without it.
     Grayfish warns against that matter.

(*3) Not required, however, source files should be there logically.
