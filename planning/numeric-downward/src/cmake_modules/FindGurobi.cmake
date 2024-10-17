# - Find the Gurobi LP solver.
# This code defines the following variables:
#
#  GUROBI_FOUND                 - TRUE if Gurobi was found.
#  GUROBI_INCLUDE_DIRS          - Full paths to all include dirs.
#  GUROBI_LIBRARIES             - Full paths to all libraries.
#  GUROBI_RUNTIME_LIBRARY       - Full path to the dll file on windows
#
# Usage:
#  find_package(gurobi)
#
# Note that the standard FIND_PACKAGE features are supported
# (QUIET, REQUIRED, etc.).

foreach(BUILDMODE "RELEASE" "DEBUG")
    set(GUROBI_HINT_PATHS_${BUILDMODE}
        ${DOWNWARD_GUROBI_ROOT_${BUILDMODE}}
        $ENV{DOWNWARD_GUROBI_ROOT_${BUILDMODE}}
        ${DOWNWARD_GUROBI_ROOT}
        $ENV{DOWNWARD_GUROBI_ROOT}
    )
endforeach()

find_path(GUROBI_INCLUDE_DIRS
    NAMES
    gurobi_c++.h
    HINTS
    ${GUROBI_HINT_PATHS_RELEASE}
    ${GUROBI_HINT_PATHS_DEBUG}
    PATH_SUFFIXES
    include
)

set(GUROBI_LIBRARY_PATH_SUFFIX_RELEASE "lib")
set(GUROBI_LIBRARY_PATH_SUFFIX_DEBUG ${GUROBI_LIBRARY_PATH_SUFFIX_RELEASE})

find_library(GUROBI_C_LIBRARY_RELEASE
    NAMES
    gurobi
    gurobi95
    gurobi91
    gurobi90
    HINTS
    ${GUROBI_HINT_PATHS_RELEASE}
    PATH_SUFFIXES
    ${GUROBI_LIBRARY_PATH_SUFFIX_RELEASE}
)

find_library(GUROBI_CPP_LIBRARY_RELEASE
    NAMES
    gurobi_c++
    HINTS
    ${GUROBI_HINT_PATHS_RELEASE}
    PATH_SUFFIXES
    ${GUROBI_LIBRARY_PATH_SUFFIX_RELEASE}
)

find_library(GUROBI_C_LIBRARY_DEBUG
    NAMES
    gurobi
    gurobi95
    gurobi91
    gurobi90
    HINTS
    ${GUROBI_HINT_PATHS_DEBUG}
    PATH_SUFFIXES
    ${GUROBI_LIBRARY_PATH_SUFFIX_DEBUG}
)

find_library(GUROBI_CPP_LIBRARY_DEBUG
    NAMES
    gurobi_c++
    HINTS
    ${GUROBI_HINT_PATHS_RELEASE}
    PATH_SUFFIXES
    ${GUROBI_LIBRARY_PATH_SUFFIX_RELEASE}
)

if((GUROBI_C_LIBRARY_RELEASE AND GUROBI_CPP_LIBRARY_RELEASE) OR (GUROBI_C_LIBRARY_DEBUG AND GUROBI_CPP_LIBRARY_DEBUG))
    set(GUROBI_LIBRARIES
        optimized ${GUROBI_C_LIBRARY_RELEASE} ${GUROBI_CPP_LIBRARY_RELEASE}
        debug ${GUROBI_C_LIBRARY_DEBUG} ${GUROBI_CPP_LIBRARY_DEBUG}
    )
endif()


# Check if everything was found and set GUROBI_FOUND.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Gurobi
    REQUIRED_VARS GUROBI_INCLUDE_DIRS GUROBI_LIBRARIES
)

mark_as_advanced(
    GUROBI_INCLUDE_DIRS GUROBI_LIBRARIES GUROBI_LIBRARY_PATH_SUFFIX
    GUROBI_LIBRARY_PATH_SUFFIX_RELEASE GUROBI_LIBRARY_PATH_SUFFIX_DEBUG
    GUROBI_LIBRARY_RELEASE GUROBI_LIBRARY_DEBUG GUROBI_RUNTIME_LIBRARY_PATH
)

