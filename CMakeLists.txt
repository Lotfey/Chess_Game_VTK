
CMAKE_MINIMUM_REQUIRED(VERSION 3.3) 

PROJECT(Chess) 

find_package(VTK REQUIRED)

if (NOT VTK_FOUND)
  message("Skipping Rampe: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  add_executable(Chess MACOSX_BUNDLE Chess.cpp Chess.h )
  target_link_libraries(Chess PRIVATE ${VTK_LIBRARIES})
else ()
  # include all components
  add_executable(Chess MACOSX_BUNDLE Chess.cpp Chess.h )
  target_link_libraries(Chess PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS Chess
    MODULES ${VTK_LIBRARIES}
    )
endif ()

