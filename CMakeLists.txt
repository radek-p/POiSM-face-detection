cmake_minimum_required(VERSION 2.8)
project(FaceRecognition)

SET(CMAKE_BUILD_TYPE Debug)

# compiler and linker flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wall -Wextra")

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Qt4
find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

set(QT_HEADERS_LIST
	src/mainwindow.h
	src/director.h
	src/imagepreviewwidget.h
)
set(QT_FORMS_LIST
	src/mainwindow.ui
)
set(SRC_LIST
	src/declarations.h
	src/main.cpp
	src/mainwindow.cpp
	src/director.cpp
	src/imagepreviewwidget.cpp
	src/facedetector.cpp
	src/faceinfo.cpp
	src/facedetectorconfig.cpp
)
set(RES_LIST
	res/resources.qrc
)

qt4_wrap_cpp(QT_HEADERS_MOC ${QT_HEADERS_LIST})
qt4_wrap_ui(QT_FORMS_HEADERS ${QT_FORMS_LIST})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${FaceRecognition_SOURCE_DIR})
qt4_add_resources(QT_RESOURCES ${RES_LIST})

add_executable(${PROJECT_NAME}
	${SRC_LIST}
	${QT_HEADERS_MOC}
	${QT_FORMS_HEADERS}
	${QT_RESOURCES}
)

target_link_libraries(${PROJECT_NAME}
	${OpenCV_LIBS}
	${QT_LIBRARIES}
)
