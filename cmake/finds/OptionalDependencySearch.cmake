
########################################
##### LOOK FOR GTEST              ######
########################################
IF(MUQ_USE_GTEST)
  find_package(GTEST)
  set(MUQ_NEEDS_GTEST ON)
  
  IF(GTEST_FOUND)
    include(CheckGTEST)	
  ENDIF()
	
  IF(GTEST_FOUND AND NOT GTEST_TEST_FAIL)
    set(MUQ_BUILD_TESTS ON)
    include_directories(${GTEST_INCLUDE_DIRS})
    LIST(APPEND test_link_libs ${GTEST_LIBRARIES})

  ELSE()

    message(WARNING "Could not find GTEST.  No tests can be compiled!")
    set(MUQ_BUILD_TESTS OFF)
    
  ENDIF(GTEST_FOUND AND NOT GTEST_TEST_FAIL)

ELSE(MUQ_USE_GTEST)

    message(WARNING “Tried to compile tests, but MUQ_USE_GTEST is OFF.  Turning off tests.”)
    set(MUQ_BUILD_TESTS OFF)
    set(MUQ_NEEDS_GTEST OFF)
ENDIF(MUQ_USE_GTEST)


########################################
##### LOOK FOR NLOPT              ######
########################################
list (FIND MUQ_REQUIRES NLOPT dindex)
if (${dindex} GREATER -1)
    set(MUQ_NEEDS_NLOPT ON)

    IF(MUQ_USE_NLOPT)

      FIND_PACKAGE(NLOPT)

      IF (NLOPT_FOUND)
        add_definitions(-DMUQ_USE_NLOPT)

        # include the sacado library for linking
        LIST(APPEND MUQ_LINK_LIBS ${NLOPT_LIBRARIES})
        LIST(APPEND MUQ_LINK_LIBS_STATIC ${NLOPT_LIBRARIES_STATIC})

        include_directories(${NLOPT_INCLUDE_DIRS})
        LIST(APPEND MUQ_EXTERNAL_INCLUDES ${NLOPT_INCLUDE_DIRS})

      ELSE()
        set(MUQ_USE_NLOPT OFF)
      ENDIF()

    ENDIF(MUQ_USE_NLOPT)
else()
    set(MUQ_NEEDS_NLOPT OFF)
endif()
    

########################################
##### LOOK FOR MKL                ######
########################################
if(MUQ_USE_MKL)

    # include the mkl library for linking
    if(MUQ_USE_OPENMP)
	  LIST(APPEND MUQ_LINK_LIBS ${MUQ_MKL_DIR}/lib/intel64/libmkl_intel_lp64${CMAKE_SHARED_LIBRARY_SUFFIX} ${MUQ_MKL_DIR}/lib/intel64/libmkl_core${CMAKE_SHARED_LIBRARY_SUFFIX} ${MUQ_MKL_DIR}/lib/intel64/libmkl_gnu_thread${CMAKE_SHARED_LIBRARY_SUFFIX})
    else()
	  LIST(APPEND MUQ_LINK_LIBS ${MUQ_MKL_DIR}/lib/intel64/libmkl_intel_lp64${CMAKE_SHARED_LIBRARY_SUFFIX} ${MUQ_MKL_DIR}/lib/intel64/libmkl_core${CMAKE_SHARED_LIBRARY_SUFFIX} ${MUQ_MKL_DIR}/lib/intel64/libmkl_sequential${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
		
    include_directories(${MUQ_MKL_DIR}/include)
    LIST(APPEND MUQ_EXTERNAL_INCLUDES ${MUQ_MKL_DIR}/include)
    add_definitions(-DEIGEN_USE_MKL_ALL)
    
endif()

########################################
##### LOOK FOR PYTHON             ######
########################################
list (FIND MUQ_REQUIRES PYTHON dindex)
if (${dindex} GREATER -1)
    set(MUQ_NEEDS_PYTHON ON)

    if(MUQ_USE_PYTHON)
        set(PYBIND11_CPP_STANDARD -std=c++11)    

        FIND_PACKAGE(pybind11)
    
        if(NOT pybind11_FOUND)
            add_subdirectory(${CMAKE_SOURCE_DIR}/external/pybind11)
            include_directories(${CMAKE_SOURCE_DIR}/external/pybind11/include)
        endif()

    endif()
else()
    set(MUQ_NEEDS_PYTHON OFF)
    set(MUQ_USE_PYTHON OFF)
endif()

########################################
##### LOOK FOR DOLFIN/Fenics      ######
########################################
list (FIND MUQ_REQUIRES DOLFIN dindex)
message(${MUQ_REQUIRES})
if (${dindex} GREATER -1)
    set(MUQ_NEEDS_DOLFIN ON)
            
    if(MUQ_USE_DOLFIN AND NOT MUQ_USE_PYTHON)
        message(WARNING "Requested compilation with Fenics/Dolfin, but not Python.  Building the Fenics/Dolfin bindings requires building MUQ with Python support.")
        set(MUQ_USE_DOLFIN OFF)
    endif()

    if(MUQ_USE_DOLFIN)
  
        find_package(DOLFIN)

        if(DOLFIN_FOUND)
            include_directories(${DOLFIN_INCLUDE_DIRS})
            include_directories(SYSTEM ${DOLFIN_3RD_PARTY_INCLUDE_DIRS})
        else()
            set(MUQ_USE_DOLFIN OFF)
        endif()
    endif()
else()
    set(MUQ_NEEDS_DOLFIN OFF)
    set(MUQ_USE_DOLFIN OFF)
endif()

########################################
##### REMOVE DUPLICATE INCLUDES   ######
########################################
list( REMOVE_DUPLICATES MUQ_LINK_LIBS)
