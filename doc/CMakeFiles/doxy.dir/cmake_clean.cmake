FILE(REMOVE_RECURSE
  "CMakeFiles/doxy"
  "out-www/sourceDoc/index.html"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/doxy.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
