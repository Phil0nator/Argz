
cmake_minimum_required (VERSION 3.8)
project(argz)
# -- Library --
set( libfiles
"src/argz.c"
)

add_library( argz STATIC ${libfiles})
target_include_directories(argz PUBLIC include)
# -- Examples --

set ( exfiles
"examples/main.c"
)
add_executable( example.exe ${exfiles} )
target_link_libraries( example.exe argz )