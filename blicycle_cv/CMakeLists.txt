PROJECT( blicycle_cv )
FIND_PACKAGE( OpenCV REQUIRED )
ADD_EXECUTABLE( blicycle blicycle.c )
TARGET_LINK_LIBRARIES( blicycle ${OpenCV_LIBS} )
