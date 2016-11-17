# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
add_executable (processTelemFile processTelemFile.cxx)

# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
target_link_libraries (processTelemFile LINK_PUBLIC AnitaTelem AnitaEvent AwareWeb AnitaMagicDisplay )
  
install (TARGETS processTelemFile DESTINATION ${UTIL_BIN_DIR})


option ( FORCE_OLD_GPP_ABI " Force old g++ ABI; this might be necessary if using new g++ with ROOT compiled with older g++ or other similar situations" OFF ) 
if (FORCE_OLD_GPP_ABI) 
  add_definitions( -D_GLIBCXX_USE_CXX11_ABI=0 ) 
endif() 
 
