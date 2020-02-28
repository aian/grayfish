TECHNICAL NOTES
===============

This is a technical notes for Grayfish.


SUB-COMMANDS AND THEIR PRERQUISITES
-----------------------------------

  | SUB-COMMAND | PROJECT PATH | SITE FILE | LOCAL CONF | SRC PATH | DST PATH |
  |:------------|:------------:|:---------:|:----------:|:--------:|:--------:|
  | main        |      X       |     X     |     X      |    X     |    X     |
  | help        |      X       |     X     |     X      |    X     |    X     |
  | version     |      X       |     X     |     X      |    X     |    X     |
  | setup       |      X       |     X     |     X      |    X     |    X     |
  | config      |      O       |     X     |     X(*2)  |    X     |    X     |
  | update      |      O       |     X     |     X(*2)  |    O     |    X     |
  | build       |      O       |     X     |     X(*2)  |    O     |    O     |
  | clean       |      O       |     X     |     X      |    X     |    X     |
  | list        |      O       |     O(*1) |     X      |    X(*3) |    X     |
  | serve       |      O       |     O(*1) |     X(*2)  |    O     |    O     |


(*1) Required, however, if the file was not found, internally calls `update'
     command to create it.

(*2) Not required to process, however, it is substancially useless without it.
     Grayfish warns against that matter.

(*3) Not required, however, there should be source files logically.
