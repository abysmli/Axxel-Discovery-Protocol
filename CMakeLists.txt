PROJECT (ADP)

SET ( CMAKE_COLOR_MAKEFILE ON )
SET ( CMAKE_VERBOSE_MAKEFILE ON )

ADD_SUBDIRECTORY ( app	bin )

INSTALL(FILES COPYRIGHT README DESTINATION share/doc/adp/)
INSTALL(PROGRAMS adp.sh DESTINATION /etc/init.d/)
INSTALL(DIRECTORY doc/ DESTINATION share/doc/adp/)
INSTALL(PROGRAMS build/bin/src/adp DESTINATION /usr/sbin/)