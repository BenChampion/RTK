WRAP_GGO(rtkfdktwodweights_GGO_C rtkfdktwodweights.ggo)
ADD_EXECUTABLE(rtkfdktwodweights rtkfdktwodweights.cxx ${rtkfdktwodweights_GGO_C})
TARGET_LINK_LIBRARIES(rtkfdktwodweights RTK)

# Installation code
IF(NOT RTK_INSTALL_NO_EXECUTABLES)
  FOREACH(EXE_NAME rtkfdktwodweights) 
    INSTALL(TARGETS ${EXE_NAME}
      RUNTIME DESTINATION ${RTK_INSTALL_RUNTIME_DIR} COMPONENT Runtime
      LIBRARY DESTINATION ${RTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
      ARCHIVE DESTINATION ${RTK_INSTALL_ARCHIVE_DIR} COMPONENT Development)
  ENDFOREACH(EXE_NAME) 
ENDIF(NOT RTK_INSTALL_NO_EXECUTABLES)

