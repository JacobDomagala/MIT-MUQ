# PURPOSE:
# This file sets up the MUQ build targets (e.g., libMuqModeling).  Information is
# used from the compile groups that were processed in the ProcessCompileGroups.cmake
# file.
#

set(MUQ_LIBRARIES )
set(MUQ_PYTHON_LIBRARIES )

message("MUQ_LINK_LIBS = ${MUQ_LINK_LIBS}")
# Build all the targets
foreach(libName ${MUQ_TARGETS})

    list(LENGTH ${libName}_SOURCES strLength)
    if(${strLength} GREATER 0)

        message(STATUS "Creating ${libName} target.")
        string(REGEX MATCH "^pymuq" IsPythonWrapper ${libName})

        if(IsPythonWrapper)
            pybind11_add_module(${libName} SHARED NO_EXTRAS ${${libName}_SOURCES})
            list(APPEND MUQ_PYTHON_LIBRARIES ${libName})
        else()
            ADD_LIBRARY(${libName} SHARED ${${libName}_SOURCES})
            list(APPEND MUQ_LIBRARIES ${libName})
        endif()

        TARGET_LINK_LIBRARIES(${libName} PRIVATE ${MUQ_LINK_LIBS})

        # Set the include directories
        target_include_directories(${libName}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

        # # Add dependencies for any required dependencies that MUQ is going to build internally
        # foreach(depend ${MUQ_REQUIRES})
        #     message(STATUS "Checking for dependency of ${libName} on internal build of ${depend}")
        #     if(USE_INTERNAL_${depend})
        #         message(STATUS "Adding dependency of ${libName} on ${depend}")
        #         add_dependencies(${libName} ${depend})
        #     endif()
        # endforeach()

        if(IsPythonWrapper)
            install(TARGETS ${libName}
                    EXPORT ${CMAKE_PROJECT_NAME}Depends
                    LIBRARY DESTINATION "${PYTHON_INSTALL_PREFIX}/muq"
                    ARCHIVE DESTINATION "${PYTHON_INSTALL_PREFIX}/muq")
        else()
            # Install the target libraries and executables
            install(TARGETS ${libName}
                    EXPORT ${CMAKE_PROJECT_NAME}Depends
                    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
                    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

            # Collect the targets into an export set
            install(
                EXPORT ${CMAKE_PROJECT_NAME}Depends       # Export set containing all targets
                DESTINATION lib/cmake/MUQ                 # Correct path where CMake will look for the config
                NAMESPACE muq::dev::                      # Namespace for the targets
                FILE "muqTargets.cmake"                   # Exported CMake file
                COMPONENT dev                             # This is part of the 'dev' component
            )

            # Add HDF5 include directories
            if(HDF5_FOUND)
                target_include_directories(${libName} PUBLIC $<BUILD_INTERFACE:${HDF5_INCLUDE_DIRS}> $<INSTALL_INTERFACE:include>)
            endif()

            # Add Eigen3 include directories
            if(Eigen3_FOUND)
                target_include_directories(${libName} PUBLIC $<BUILD_INTERFACE:${Eigen3_INCLUDE_DIRS}> $<INSTALL_INTERFACE:include>)
            endif()

        endif()
    endif()

endforeach()

INSTALL (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/MUQ
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")

# If a group depends on an external library that is going to be built by MUQ, then make sure we account for that dependency
foreach(group ${MUQ_GROUPS})
    if(${group}_IS_COMPILED)
        list(LENGTH ${group}_SOURCES strLength)

        # foreach(depend ${POSSIBLE_MUQ_DEPENDENCIES})
        #     list(FIND ${group}_REQUIRES ${depend} needsExternal)
        #     if(USE_INTERNAL_${depend})
        #         if(needsExternal AND ${USE_INTERNAL_${depend}} AND (strLength GREATER 0))
        #             add_dependencies(${${group}_LIBRARY} ${depend})
        #         endif()
	    # endif()
        # endforeach()

        # Add dependencies between different MUQ libraries
        foreach(depend ${${group}_REQUIRES_GROUPS})
            message(STATUS "Thinking about connection between ${${group}_LIBRARY} and ${${depend}_LIBRARY}")
            string(COMPARE EQUAL "${${depend}_LIBRARY}" "" result)
            if(NOT result)
                if(NOT ${${group}_LIBRARY} STREQUAL ${${depend}_LIBRARY})
                    IF( ${depend}_IS_COMPILED )
                        message(STATUS "Trying to add connection between ${${group}_LIBRARY} and ${${depend}_LIBRARY}")
                        target_link_libraries(${${group}_LIBRARY} PUBLIC ${${depend}_LIBRARY})
                        add_dependencies(${${group}_LIBRARY} ${${depend}_LIBRARY})
                    endif()
                endif()
              endif()
        endforeach()
    endif()
endforeach()
