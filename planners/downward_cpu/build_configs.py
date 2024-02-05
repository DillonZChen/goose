release = [
    "-DCMAKE_BUILD_TYPE=Release",
    "-DBoost_INCLUDE_DIR=$BOOST_BASE/include",
    "-DBoost_LIBRARY_DIR=$BOOST_BASE/lib/GNU",
]
debug = ["-DCMAKE_BUILD_TYPE=Debug"]
release_no_lp = ["-DCMAKE_BUILD_TYPE=Release", "-DUSE_LP=NO"]
# USE_GLIBCXX_DEBUG is not compatible with USE_LP (see issue983).
glibcxx_debug = [
    "-DCMAKE_BUILD_TYPE=Debug",
    "-DUSE_LP=NO",
    "-DUSE_GLIBCXX_DEBUG=YES",
]
minimal = ["-DCMAKE_BUILD_TYPE=Release", "-DDISABLE_PLUGINS_BY_DEFAULT=YES"]

DEFAULT = "release"
DEBUG = "debug"
