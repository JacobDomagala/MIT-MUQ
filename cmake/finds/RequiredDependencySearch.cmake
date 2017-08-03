# define a macro to look for a package and install a local copy if we can't find it
macro (GetDependency name)
        # check to see if this dependency is required by any group
        list (FIND MUQ_REQUIRES ${name} dindex)
        if (${dindex} GREATER -1)
	    set(MUQ_NEEDS_${name} ON)
            
	    find_package(${name})
	    if(${name}_FOUND)
	        # check to make sure the library can be linked to
		include(Check${name})

		if(NOT ${name}_TEST_FAIL)
			set(USE_INTERNAL_${name} 0)
		else()
			set(USE_INTERNAL_${name} 1)	
		endif()

	    else()
		set(USE_INTERNAL_${name} 1)	
	    endif()
	
	    if(USE_INTERNAL_${name})
		include(Build${name})
	    endif()
	
	    # store include directory information
	    include_directories(${${name}_INCLUDE_DIRS})
	    LIST(APPEND MUQ_EXTERNAL_INCLUDES ${${name}_INCLUDE_DIRS})

	    # store library information
	    LIST(APPEND MUQ_LINK_LIBS ${${name}_LIBRARIES})
	    LIST(APPEND MUQ_LINK_LIBS_STATIC ${${name}_LIBRARIES_STATIC})

        else()
            set(MUQ_NEEDS_${name} OFF)   
        endif()
	
endmacro(GetDependency)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/include)

########################################
##### LOOK FOR AND/OR BUILD Eigen ######
########################################
GetDependency(EIGEN3)

########################################
##### LOOK FOR AND/OR BUILD SUNDIALS ###
########################################
GetDependency(SUNDIALS)

########################################
##### LOOK FOR AND/OR BUILD HDF5  ######
########################################

set(HAVE_HDF5 1)

GetDependency(HDF5)

if(MUQ_USE_OPENMPI)
	find_package(ZLIB)
	include_directories(${ZLIB_INCLUDE_DIRS})
	LIST(APPEND MUQ_LINK_LIBS ${ZLIB_LIBRARIES})
	LIST(APPEND MUQ_LINK_LIBS_STATIC ${ZLIB_LIBRARIES_STATIC})
	LIST(APPEND MUQ_EXTERNAL_INCLUDES ${ZLIB_INCLUDE_DIRS})
	message("ZLIB_LIBRARIES" ${ZLIB_LIBRARIES})
	
endif()

########################################
##### LOOK FOR AND/OR BUILD FLANN ######
########################################

GetDependency(FLANN)

###############################################
##### LOOK FOR BOOST                     ######
###############################################

list (FIND MUQ_REQUIRES BOOST dindex)
if (${dindex} GREATER -1)
    set(MUQ_NEEDS_BOOST ON)
    set(MUQ_USE_BOOST ON)
    
    find_package(BOOSTMUQ)
    if(NOT DEFINED Boost_FOUND)
	set(Boost_FOUND ${BOOST_FOUND})
    endif()

    if(Boost_FOUND)
	# check to make sure the library can be linked to
	include(CheckBoost)

	if(NOT BOOST_TEST_FAIL)
		set(USE_INTERNAL_BOOST 0)
	else()
		set(USE_INTERNAL_BOOST 1)	
	endif()

    else()
	set(USE_INTERNAL_BOOST 1)	
    endif()

    if(USE_INTERNAL_BOOST)
	include(BuildBoost)
    endif()

    # do we want to compile the python interface?
    set(MUQ_PYTHON 0)
    if(MUQ_USE_PYTHON)
        set(MUQ_PYTHON 1)
    endif()

    # do we have nlopt?
    set(MUQ_NLOPT 0)
    if(MUQ_USE_NLOPT)
        set(MUQ_NLOPT 1)
    endif()

    # store include directory information
    if(NOT DEFINED Boost_INCLUDE_DIRS)
        set(Boost_INCLUDE_DIRS ${BOOST_INCLUDE_DIRS})
    endif()

    include_directories(${Boost_INCLUDE_DIRS})
    LIST(APPEND MUQ_EXTERNAL_INCLUDES ${Boost_INCLUDE_DIRS})

    if(NOT DEFINED Boost_LIBRARIES)
        set(Boost_LIBRARIES ${BOOST_LIBRARIES})
        set(Boost_LIBRARIES_STATIC ${BOOST_LIBRARIES_STATIC})
    endif()

    # store library information
    LIST(APPEND MUQ_LINK_LIBS ${Boost_LIBRARIES})
    LIST(APPEND MUQ_LINK_LIBS_STATIC ${Boost_LIBRARIES_STATIC})

else()
    set(MUQ_NEEDS_BOOST OFF)

endif()

########################################
##### REMOVE DUPLICATE INCLUDES   ######
########################################

list( REMOVE_DUPLICATES MUQ_EXTERNAL_INCLUDES)


