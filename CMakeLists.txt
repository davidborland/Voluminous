cmake_minimum_required( VERSION 2.6 )


project( Voluminous )


set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/" )


#######################################
# Include VTK
#######################################

find_package( VTK REQUIRED )
include( ${VTK_USE_FILE} )

set( VTK_LIBS QVTK ) 


#######################################
# Include Qt
#######################################

# Use what QVTK built with
set( QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "" )
set( QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "" )
set( QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "" )
find_package( Qt )
if( QT_USE_FILE )
  include( ${QT_USE_FILE} )
else( QT_USE_FILE )
  set( QT_LIBRARIES ${QT_QT_LIBRARY} )
endif( QT_USE_FILE )

# Use the include path and library for Qt that is used by VTK.
include_directories( ${QT_INCLUDE_DIR} )

# Set up variables for moc
set( QT_UI MainWindow.ui AboutDialog.ui )
set( QT_QRC Voluminous.qrc )
set( QT_HEADER MainWindow.h AboutDialog.h )
set( QT_SRC Voluminous.cpp MainWindow.cpp AboutDialog.cpp )

# Do moc stuff
qt4_wrap_ui( QT_UI_HEADER ${QT_UI} )
qt4_add_resources( QT_RCC_SRC ${QT_QRC} )
qt4_wrap_cpp( QT_MOC_SRC ${QT_HEADER} )
add_definitions( -DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT )
set_source_files_properties( ${QT_SRC} PROPERTIES OBJECT_DEPENDS "${QT_UI_HEADER}" )


#######################################
# Include QScientific
#######################################

find_package( QScientific REQUIRED )
include( ${QScientific_USE_FILE} )


#######################################
# Include VRPN
#######################################

option( USE_VRPN "Use VRPN for head tracking" OFF) 

if( USE_VRPN )
  find_package( VRPN REQUIRED )
  include_directories( ${VRPN_INCLUDE_DIR} )
endif( USE_VRPN )


#######################################
# Include Voluminous code
#######################################

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set( SRC VTKPipeline.h VTKPipeline.cpp
         Isosurface.h Isosurface.cpp
         Slice.h Slice.cpp )
		 
# Add resource file on Windows		 
if( WIN32 ) 
  set( SRC Voluminous.rc ${SRC} )
endif( WIN32 )		 

set( SHADERS perPixelLighting.xml silhouetteFalloff.xml )
source_group(Shaders FILES ${SHADERS} )

add_executable( Voluminous ${QT_HEADER} ${QT_RCC_SRC} ${QT_SRC} ${QT_MOC_SRC} ${SRC} ${SHADERS} )
target_link_libraries( Voluminous ${VTK_LIBS} ${QT_LIBRARIES} ${QScientific_LIB} ${VRPN_LIBRARY} )


#######################################
# Set installation package properties
#######################################

# Set the version string
set( Voluminous_VERSION_STRING "1.0" )

# Check for 32 or 64-bit
if( ${CMAKE_SIZEOF_VOID_P} MATCHES "^8$" )
  set( ARCH_BITS "64" )
else( ${CMAKE_SIZEOF_VOID_P} MATCHES "^8$" )
  set( ARCH_BITS "32" )
endif( ${CMAKE_SIZEOF_VOID_P} MATCHES "^8$" )

set( CPACK_PACKAGE_DIRECTORY "${Voluminous_SOURCE_DIR}/Installers" )
set( CPACK_PACKAGE_NAME "Voluminous_${ARCH_BITS}" )
set( CPACK_PACKAGE_VENDOR "RENCI" )
set( CPACK_PACKAGE_EXECUTABLES "Voluminous;Voluminous ${Voluminous_VERSION_STRING}" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Voluminous Volume Visualization Tool" )
set( CPACK_PACKAGE_VERSION "${Voluminous_VERSION_STRING}" )
set( CPACK_PACKAGE_VERSION_MAJOR "${Voluminous_MAJOR_NUMBER}" )
set( CPACK_PACKAGE_VERSION_MINOR "${Voluminous_MINOR_NUMBER}" )
set( CPACK_PACKAGE_VERSION_PATCH "${Voluminous_REVISION_NUMBER}" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "Voluminous_${Voluminous_VERSION_STRING}" )
set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Voluminous ${Voluminous_VERSION_STRING}" )
set( CPACK_RESOURCE_FILE_LICENSE "${Voluminous_SOURCE_DIR}/License.txt" )
set( CPACK_RESOURCE_FILE_README  "${Voluminous_SOURCE_DIR}/README.txt" )

if( WIN32 )
  # Windows-specific CPACK instructions for NSIS
  set( CPACK_GENERATOR "NSIS" )
  
  set( CPACK_NSIS_URL_INFO_ABOUT "http://www.renci.org" )
  set( CPACK_NSIS_CONTACT "borland@renci.org" )

  # Both of these need to be set or neither of them is set.
  set( CPACK_NSIS_MUI_ICON "${Voluminous_SOURCE_DIR}/Data/Voluminous.ico" )
  set( CPACK_NSIS_MUI_UNIICON "${Voluminous_SOURCE_DIR}/Data/VoluminousUninstall.ico" )
  
  set( CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\Voluminous.exe" )
  
  get_filename_component( QT_ROOT ${QT_QMAKE_EXECUTABLE} PATH )
  
  install( FILES ${QT_ROOT}/QtCore4.dll ${QT_ROOT}/QtGui4.dll
           DESTINATION bin )
endif( WIN32 )

# Setting the destination to bin makes a few other things much smoother, such as InstallRequiredSystemLibraries
install( TARGETS Voluminous
         RUNTIME DESTINATION bin )

install( FILES ${Voluminous_SOURCE_DIR}/README.txt ${Voluminous_SOURCE_DIR}/License.txt
         DESTINATION . )

# This includes Microsoft runtime libraries when run in Windows environment.
include( InstallRequiredSystemLibraries )

# This must always be last!
include( CPack )
