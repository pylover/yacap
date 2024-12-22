_foo() {
  local cur prev words cword
  _get_comp_words_by_ref -n '=:' cur prev words cword
  echo "cur: ${cur}, prev: ${prev}, words: ${words[*]}, cword: \
	  ${cword}" >> /tmp/gg
        
  echo "point: ${COMP_POINT} ${COMP_LINE}"  >> /tmp/gg
  COMPREPLY=(${words[*]})
} && complete -F _foo foo 
