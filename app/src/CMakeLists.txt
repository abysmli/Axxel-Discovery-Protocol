
SET ( CMAKE_COLOR_MAKEFILE ON )
SET ( CMAKE_VERBOSE_MAKEFILE ON )
SET (   ADP 	
        axxel_discovery_protokoll.cpp 
        Server.cpp
        Client.cpp
        ControlCenter.cpp
        LocalNetworkInformation.cpp
        Exception.cpp
		soapC.cpp 
		soapServer.cpp
		soapClient.cpp 
		stdsoap2.cpp )

SET ( CMAKE_CXX_FLAGS " -O0 -g3 -Wall -std=c++11 -DWITH_IPV6 -lpthread -D_GLIBCXX_USE_NANOSLEEP ")

MESSAGE( STATUS "The BINARY dir is " ${PROJECT_BINARY_DIR})
MESSAGE( STATUS "The SOURCE dir is " ${PROJECT_SOURCE_DIR})

ADD_EXECUTABLE( adp ${ADP})