FILE(REMOVE_RECURSE
  "CMakeFiles/docs"
  "out-www/manual/marsyas-user.pdf"
  "out-www/manual/marsyas-user/index.html"
  "out-www/manual/marsyas-devel.pdf"
  "out-www/manual/marsyas-devel/index.html"
  "out-www/manual/marsyas-cookbook.pdf"
  "out-www/manual/marsyas-cookbook/index.html"
  "out-www/sourceDoc/index.html"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/docs.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
