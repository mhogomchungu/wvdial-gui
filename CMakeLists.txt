
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0017 OLD)
cmake_policy(SET CMP0002 OLD)

set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)

project(wvdial-gui)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

set( CMAKE_INCLUDE_CURRENT_DIR ON )
include_directories( ${Qt5Widgets_INCLUDE_DIRS} )
include_directories( ${Qt5Network_INCLUDE_DIRS} )

add_definitions( ${Qt5Widgets_DEFINITIONS} -std=c++11 )

set( UI_FILES  src/wvdial.ui )

set( MOC_FILES src/wvdial.h )

set( CPP_FILES src/main.cpp src/wvdial.cpp )

set( RESOUCES icons/icons.qrc )

QT5_WRAP_CPP( MOC ${MOC_FILES} )
QT5_WRAP_UI( UI ${UI_FILES} )
QT5_ADD_RESOURCES( TRAY_RC_SRCS ${RESOUCES} )

add_executable( wvdial-gui ${CPP_FILES} ${MOC} ${UI} ${TRAY_RC_SRCS} )

TARGET_LINK_LIBRARIES( wvdial-gui ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} )

set_target_properties( wvdial-gui PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64 -Wextra -Wall -s -fPIE -pthread  -pedantic " )
set_target_properties( wvdial-gui PROPERTIES LINK_FLAGS "-pie" )

install( TARGETS wvdial-gui RUNTIME DESTINATION bin )

install( FILES icons/wvdial-gui.png DESTINATION /usr/share/icons/hicolor/48x48/apps/ )
install( FILES icons/wvdial-gui.png DESTINATION /usr/share/pixmaps )

install( FILES ${PROJECT_BINARY_DIR}/wvdial-gui.desktop DESTINATION share/applications
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# desktop file section
file( WRITE "${PROJECT_BINARY_DIR}/wvdial-gui.desktop"
"[Desktop Entry]
Comment[en_US]=
Comment=
Exec=${CMAKE_INSTALL_PREFIX}/bin/wvdial-gui
GenericName[en_US]=A GUI front end to wvdial tool
GenericName=A GUI front end to wvdial tool
Icon=wvdial-gui
MimeType=
Name[en_US]=wvdial-gui
Name=wvdial-gui
NoDisplay=false
StartupNotify=true
Terminal=false
Type=Application
Categories=Network;X-MandrivaLinux-Internet;\n")
