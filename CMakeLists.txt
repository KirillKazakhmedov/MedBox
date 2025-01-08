cmake_minimum_required(VERSION 3.5.1)

set(PROJECT term_project)
project (${PROJECT})

#подключение библиотеки Boost
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(DCMTK_DIR "D:/Libs/DCMTKLib/cmake")

find_package (DCMTK REQUIRED PATH ${DCMTK_DIR} NO_DEFAULT_PATH)

if (DCMTK_FOUND)
	message ("DCMTK was found at ${DCMTK_DIR}")
elseif (NOT DCMTK_FOUND)
	message (FATAL_ERROR "Could not find DCMTK at ${DCMTK_PATH}")
endif (DCMTK_FOUND)

include_directories("D:/Libs/DCMTKLib/include")
link_directories("D:/Libs/DCMTKLib/lib")

FIND_PACKAGE(Boost 1.64.0 COMPONENTS filesystem system)

set(HEADERS
	ApplicationFactory.h Builder.h CellSize.h Decimator.h Edge.h filter.h grid.h Image.h ImageCollection.h IndexedTriangle.h
	IndexedVertex.h LookUpTable.h MarchingCube.h Mesh.h Normal.h OrderedPair.h Plane.h Recorder.h Segmentation.h Smoother.h
	Triangle.h Vertex.h LineSegment.h)

set(SOURCES
	ApplicationFactory.cpp Builder.cpp Decimator.cpp filter.cpp Image.cpp ImageCollection.cpp MarchingCube.cpp Mesh.cpp Recorder.cpp 
	Smoother.cpp main.cpp)

add_executable (${PROJECT} ${HEADERS} ${SOURCES})

IF(Boost_FOUND)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	TARGET_LINK_LIBRARIES(${PROJECT} ${Boost_LIBRARIES})
ELSE()
	TARGET_LINK_LIBRARIES(${PROJECT} ${CONAN_LIBS})
ENDIF()

target_link_libraries(${PROJECT} ${DCMTK_LIBRARIES})