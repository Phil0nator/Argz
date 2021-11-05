
cmake_minimum_required (VERSION 3.8)
project(argz)
# -- Library --
set( libfiles
"src/argz.c"
)

add_library( argz STATIC ${libfiles})
target_include_directories(argz PUBLIC include)
# -- Examples --
# set ( exfiles1
# "examples/main.cpp"
# )
# add_executable( example1.exe ${exfiles1} )
# target_link_libraries( example1.exe argz )

set ( exfiles2
"examples/main.c"
)
add_executable( example2.exe ${exfiles2} )
target_link_libraries( example2.exe argz )