- detect shell and do not initialize auto completion if it's not bash.
- compgen cmd
- delete help.h
- code coverage
- example
- test command entrypoint
- readme
- Automatic shortcuts / aliases
- feature: aliases
- feature: shorthands/abbr

- feature: YACAP_OPTION_ARGOPTIONAL
  The argument associated with this option is optional.

- feature: YACAP_OPTION_HIDDEN
  This option isn’t displayed in any help messages.

- feature: YACAP_OPTION_ALIAS
  This option is an alias for the closest previous non-alias option. 
  This means that it will be displayed in the same help entry, and will 
  inherit fields other than name and key from the option being aliased.

- feature: YACAP_OPTION_DOC
  This option isn’t actually an option and should be ignored by the actual 
  option parser. It is an arbitrary section of documentation that should be 
  displayed in much the same manner as the options. This is known as a 
  documentation option.

  If this flag is set, then the option name field is displayed unmodified 
  (e.g., no ‘--’ prefix is added) at the left-margin where a short option 
  would normally be displayed, and this documentation string is left in its 
  usual place. For purposes of sorting, any leading whitespace and punctuation 
  is ignored, unless the first non-whitespace character is ‘-’. This entry is 
  displayed after all options, after OPTION_DOC entries with a leading ‘-’, 
  in the same group.
