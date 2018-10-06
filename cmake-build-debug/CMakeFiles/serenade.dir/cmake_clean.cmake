file(REMOVE_RECURSE
  "libserenade.pdb"
  "libserenade.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/serenade.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
