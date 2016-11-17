# make sure that the FLANN library is available
set(CMAKE_REQUIRED_LIBRARIES ${FLANN_LIBRARIES})
set(CMAKE_REQUIRED_INCLUDES ${FLANN_INCLUDE_DIR})
set(CMAKE_REQUIRED_FLAGS "${CMAKE_CXX_FLAGS}")
CHECK_CXX_SOURCE_COMPILES(
  "
#include <flann/flann.hpp>

#include <stdio.h>

using namespace flann;

int main(int argc, char** argv)
{
    int nn = 3;

    Matrix<float> dataset;
    Matrix<float> query;
    char str[128];

    Matrix<int> indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows, nn);

    // construct an randomized kd-tree index using 4 kd-trees
    Index<L2<float> > index(dataset, flann::KDTreeIndexParams(4));
    index.buildIndex();                                                                                               

    // do a knn search, using 128 checks
    index.knnSearch(query, indices, dists, nn, flann::SearchParams(128));

    delete[] dataset.ptr();
    delete[] query.ptr();
    delete[] indices.ptr();
    delete[] dists.ptr();
    
    return 0;
}
  
  "
  FLANN_COMPILES)

	
	if(NOT FLANN_COMPILES)
		set(FLANN_TEST_FAIL 1)
	else()
		set(FLANN_TEST_FAIL 0)
	endif()
