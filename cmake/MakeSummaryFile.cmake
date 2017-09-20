SET(_log_summary  "${CMAKE_BINARY_DIR}/summary.log")
FILE(REMOVE ${_log_summary})

FILE(APPEND ${_log_summary}
"#############################################
#
#  MUQ configuration:
#        CMAKE_BUILD_TYPE:         ${CMAKE_BUILD_TYPE}
#        BUILD_SHARED_LIBS:        ${BUILD_SHARED_LIBS}
#        BUILD_STATIC_LIBS:        ${build_static_libs}
#        CMAKE_INSTALL_PREFIX:     ${CMAKE_INSTALL_PREFIX}
#        CMAKE_SOURCE_DIR:         ${CMAKE_SOURCE_DIR}
#        CMAKE_BINARY_DIR:         ${CMAKE_BINARY_DIR}
#        CMAKE_CXX_COMPILER name:  ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} on platform ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}
#        CMAKE_CXX_COMPILER path:  ${CMAKE_CXX_COMPILER}
#
"
  )

  IF(${CMAKE_BUILD_TYPE} MATCHES "Release")
FILE(APPEND ${_log_summary}
"#  Compiler flags used for this build:
#        CMAKE_CXX_FLAGS:        ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}
"
  )
elseif(${CMAKE_BUILD_TYPE} MATCHES "Debug")
FILE(APPEND ${_log_summary}
"#  Compiler flags used for this build:
#        CMAKE_CXX_FLAGS:        ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}
"
)	
endif()
FILE(APPEND ${_log_summary}
"#
#  Compiler definitions used for this build:
#        COMPILE_DEFINITIONS:   ${MUQ_COMPILE_DEFINITIONS}
#
"
)

macro (PrintRequired name pad)
    if(USE_INTERNAL_${name})
        if(${name}_FOUND)
            FILE(APPEND ${_log_summary} "#        ${name}${pad}-------------> Met with internal build -- Failed compilation test.\n")
            
        else()
            FILE(APPEND ${_log_summary} "#        ${name}${pad}-------------> Met with internal build -- Could not find library.\n")
        endif()

    elseif(NOT ${MUQ_NEEDS_${name}})
    
        FILE(APPEND ${_log_summary} "#        ${name}${pad}-------------> Not required for selected compile groups.\n")
    else()
    
        FILE(APPEND ${_log_summary} "#        ${name}${pad}-------------> Met with existing library:\n"
                                    "#                                Include Directory:\n"
                                    "#                                  ${${name}_INCLUDE_DIR}\n")

        IF(DEFINED ${name}_LIBRARIES)
            FILE(APPEND ${_log_summary} "#                                Libraries:\n")
            foreach(libName ${${name}_LIBRARIES})
                FILE(APPEND ${_log_summary} "#                                  ${libName}\n") 
            endforeach(libName)
        endif()
    endif()
    FILE(APPEND ${_log_summary} "#\n")
endmacro(PrintRequired)

FILE(APPEND ${_log_summary}
"#  Required dependencies: \n"
)
PrintRequired(EIGEN3 " --")
PrintRequired(BOOST " ---")
PrintRequired(HDF5 " ----")
PrintRequired(FLANN " ---")
PrintRequired(SUNDIALS " ")
FILE(APPEND ${_log_summary} "#\n")

macro(PrintOptional name pad)
    if(MUQ_NEEDS_${name})
        if(DEFINED ${name}_FOUND)
            if(${name}_FOUND AND ${name}_TEST_FAIL)
                FILE(APPEND ${_log_summary} "#        ${name}${pad}----> OFF -- Failed compilation test.\n")
            elseif(NOT ${name}_FOUND)
                FILE(APPEND ${_log_summary} "#        ${name}${pad}----> OFF -- Could not find library.\n")
            else()
                FILE(APPEND ${_log_summary} "#        ${name}${pad}-----> ON.\n"
                                            "#                                Include Directory:\n"
                                            "#                                  ${${name}_INCLUDE_DIR}\n")

                if(DEFINED ${name}_LIBRARIES)
                    FILE(APPEND ${_log_summary} "#                                Libraries:\n")
                    foreach(libName ${${name}_LIBRARIES}) 
                        FILE(APPEND ${_log_summary} "#                                  ${libName}\n") 
                    endforeach(libName) 
                endif()

            endif()

        else()
            FILE(APPEND ${_log_summary} "#        ${name}:${pad}----> OFF.\n")
        endif()
    else()
        FILE(APPEND ${_log_summary} "#        ${name}${pad}----> OFF -- Not required for selected compile groups.\n")
    endif()
    FILE(APPEND ${_log_summary} "#\n")
    
endmacro(PrintOptional)

# print glog status
FILE(APPEND ${_log_summary} "#  Optional dependencies:\n")
PrintOptional(GTEST " -----------")
PrintOptional(NLOPT " -----------")
PrintOptional(DOLFIN " ----------")
FILE(APPEND ${_log_summary} "#\n")
  
FILE(APPEND ${_log_summary}
"#  Optional tools:   
#        OpenMP --------------> ${MUQ_USE_OPENMP}
#        MKL -----------------> ${MUQ_USE_MKL}
"
)
if(pybind11_FOUND AND MUQ_USE_PYTHON)
    FILE(APPEND ${_log_summary} "#        Python --------------> ${MUQ_USE_PYTHON} -- Found pybind11 in ${pybind11_INCLUDE_DIR}.\n")
elseif(MUQ_USE_PYTHON)
    FILE(APPEND ${_log_summary} "#        Python --------------> ${MUQ_USE_PYTHON} -- Building pybind11 from source.\n")
else()
    FILE(APPEND ${_log_summary} "#        Python --------------> ${MUQ_USE_PYTHON}\n")     
endif()

FILE(APPEND ${_log_summary} "#\n#  MUQ Modules: \n")
foreach(target ${MUQ_TARGETS})
    string(REPLACE "muq" "" moduleName ${target})
    FILE(APPEND ${_log_summary} "#        ${moduleName}:\n")
    foreach(group ${MUQ_GROUPS})
        if(${${group}_LIBRARY} STREQUAL ${target})
            FILE(APPEND ${_log_summary} "#            ${group}\n")
        endif()
    endforeach()
endforeach()

FILE(APPEND ${_log_summary} "#\n#############################################\n\n")
