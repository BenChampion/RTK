#=========================================================
# Propagate cmake options in a header file
CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/rtkConfiguration.h.in
  ${CMAKE_BINARY_DIR}/rtkConfiguration.h)
SET (RTK_LIBRARIES RTK)
#=========================================================

#=========================================================
ADD_LIBRARY(RTK
            rtkHisImageIO.cxx
            rtkHisImageIOFactory.cxx
            rtkElektaSynergyGeometryReader.cxx
            rtkDbf.cxx
            rtkHndImageIO.cxx
            rtkHndImageIOFactory.cxx
            rtkVarianObiXMLFileReader.cxx
            rtkVarianObiGeometryReader.cxx
            rtkEdfImageIO.cxx
            rtkEdfImageIOFactory.cxx
            rtkImagXImageIO.cxx
            rtkImagXImageIOFactory.cxx
            rtkImagXXMLFileReader.cxx
            rtkThreeDCircularProjectionGeometry.cxx
            rtkReg23ProjectionGeometry.cxx
            rtkThreeDCircularProjectionGeometryXMLFile.cxx
            rtkGeometricPhantomFileReader.cxx
            rtkDigisensGeometryXMLFileReader.cxx
            rtkDigisensGeometryReader.cxx
            rtkXRadGeometryReader.cxx
            rtkXRadImageIO.cxx
            rtkXRadImageIOFactory.cxx
            rtkIOFactories.cxx
            rtkConvertEllipsoidToQuadricParametersFunction.cxx
            )

TARGET_LINK_LIBRARIES(RTK ${ITK_LIBRARIES})
#=========================================================

#=========================================================
# OpenCL library stuff
IF(RTK_USE_OPENCL)
  SET(RTK_LIBRARIES ${RTK_LIBRARIES} rtkopencl)
  ADD_LIBRARY(rtkopencl
              rtkOpenCLUtilities.cxx
              rtkOpenCLFDKBackProjectionImageFilter.cxx)
  TARGET_LINK_LIBRARIES(rtkopencl ${OPENCL_LIBRARIES} ${ITK_LIBRARIES})

  FILE(GLOB openclSRCs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cl)
  ADD_CUSTOM_TARGET(openclSRCCopy ALL COMMENT "Copying OpenCL source files")
  FOREACH(openclSRC ${openclSRCs})
    ADD_CUSTOM_COMMAND(TARGET openclSRCCopy
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${openclSRC}
                                                        ${RTK_BINARY_DIR}/${openclSRC})
  ENDFOREACH(openclSRC)
  ADD_DEPENDENCIES(rtkopencl openclSRCCopy)
ENDIF(RTK_USE_OPENCL)
#=========================================================

#=========================================================
# Cuda library stuff
IF (RTK_USE_CUDA)
  SET (RTK_LIBRARIES ${RTK_LIBRARIES} rtkcuda)
  SET (rtkcuda_CUDA_FILES 
       rtkCudaCropImageFilter.cu 
       rtkCudaUtilities.cu
       rtkCudaFDKBackProjectionImageFilter.cu
       rtkCudaFFTRampImageFilter.cu
       rtkCudaFDKWeightProjectionFilter.cu
       rtkCudaDisplacedDetectorImageFilter.cu
       rtkCudaParkerShortScanImageFilter.cu
       rtkCudaForwardProjectionImageFilter.cu
       rtkCudaBackProjectionImageFilter.cu
       rtkCudaSplatImageFilter.cu
       rtkCudaInterpolateImageFilter.cu
       rtkCudaTotalVariationDenoisingBPDQImageFilter.cu)
       
  # This is a fix for ITK4 to avoid too long cmd lines error
  # (http://www.itk.org/Bug/view.php?id=12198)
  IF(WIN32)
    GET_DIRECTORY_PROPERTY(dirInc INCLUDE_DIRECTORIES)
    FIND_PATH(itkMacroPath itkMacro.h ${dirInc})
    FIND_PATH(itkConfigurePath itkConfigure.h ${dirInc})
    FIND_PATH(rtkConfigurationPath rtkConfiguration.h ${dirInc})
    SET_DIRECTORY_PROPERTIES(PROPERTIES INCLUDE_DIRECTORIES "${itkMacroPath};${itkConfigurePath};${rtkConfigurationPath}")
  ENDIF()

  CUDA_COMPILE (rtkcuda_CUDA_WRAPPERS ${rtkcuda_CUDA_FILES})

  # Rollback to all includes
  IF(WIN32)
    SET_DIRECTORY_PROPERTIES(PROPERTIES INCLUDE_DIRECTORIES "${dirInc}")
  ENDIF()

  ADD_LIBRARY(rtkcuda
              rtkCudaFDKWeightProjectionFilter.cxx
              rtkCudaDisplacedDetectorImageFilter.cxx
              rtkCudaParkerShortScanImageFilter.cxx
              rtkCudaCropImageFilter.cxx
              rtkCudaFDKBackProjectionImageFilter.cxx
              rtkCudaFFTRampImageFilter.cxx
              rtkCudaFDKConeBeamReconstructionFilter.cxx
              rtkCudaBackProjectionImageFilter.cxx
	      rtkCudaSplatImageFilter.cxx
	      rtkCudaInterpolateImageFilter.cxx
	      rtkCudaTotalVariationDenoisingBPDQImageFilter.cxx
              ${rtkcuda_CUDA_WRAPPERS}
              ${rtkcuda_CUDA_FILES})
  SET_TARGET_PROPERTIES (rtkcuda PROPERTIES LINKER_LANGUAGE CXX)
  TARGET_LINK_LIBRARIES(rtkcuda ${CUDA_LIBRARIES} ${CUDA_cufft_LIBRARY} RTK ITKCudaCommon)
ENDIF (RTK_USE_CUDA)
#=========================================================

#=========================================================
# Installation code
IF(NOT RTK_INSTALL_NO_DEVELOPMENT)
  # Generate RTKConfig.cmake for the install tree.
  SET (RTK_USE_FILE "${CMAKE_INSTALL_PREFIX}/${RTK_INSTALL_PACKAGE_DIR}/UseRTK.cmake")
  SET (RTK_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${RTK_INSTALL_INCLUDE_DIR}")
  SET (RTK_LIBRARY_DIRS "${CMAKE_INSTALL_PREFIX}/${RTK_INSTALL_LIB_DIR}")
  CONFIGURE_FILE (${PROJECT_SOURCE_DIR}/cmake/RTKConfig.cmake.in ${RTK_BINARY_DIR}/CMakeFiles/RTKConfig.cmake @ONLY)
  INSTALL(FILES ${RTK_BINARY_DIR}/CMakeFiles/RTKConfig.cmake
                ${RTK_SOURCE_DIR}/cmake/UseRTK.cmake
          DESTINATION ${RTK_INSTALL_PACKAGE_DIR})

  # Include .h and .txx files
  FILE(GLOB __files1 "${PROJECT_SOURCE_DIR}/code/*.h")
  FILE(GLOB __files2 "${PROJECT_SOURCE_DIR}/code/*.txx")
  INSTALL(FILES ${__files1} ${__files2} ${RTK_BINARY_DIR}/rtkConfiguration.h
    DESTINATION ${RTK_INSTALL_INCLUDE_DIR}
    COMPONENT Development)
ENDIF(NOT RTK_INSTALL_NO_DEVELOPMENT)

IF(NOT RTK_INSTALL_NO_LIBRARIES)
  INSTALL(TARGETS RTK
    RUNTIME DESTINATION ${RTK_INSTALL_RUNTIME_DIR} COMPONENT Runtime
    LIBRARY DESTINATION ${RTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${RTK_INSTALL_ARCHIVE_DIR} COMPONENT Development)

  IF(RTK_USE_CUDA)
    INSTALL(TARGETS rtkcuda
      RUNTIME DESTINATION ${RTK_INSTALL_RUNTIME_DIR} COMPONENT Runtime
      LIBRARY DESTINATION ${RTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
      ARCHIVE DESTINATION ${RTK_INSTALL_ARCHIVE_DIR} COMPONENT Development)
  ENDIF(RTK_USE_CUDA)

  IF(OPENCL_FOUND)
    INSTALL(TARGETS rtkopencl
      RUNTIME DESTINATION ${RTK_INSTALL_RUNTIME_DIR} COMPONENT Runtime
      LIBRARY DESTINATION ${RTK_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
      ARCHIVE DESTINATION ${RTK_INSTALL_ARCHIVE_DIR} COMPONENT Development)
  ENDIF(OPENCL_FOUND)
ENDIF(NOT RTK_INSTALL_NO_LIBRARIES)
#=========================================================

SET(RTK_LIBRARIES ${RTK_LIBRARIES} PARENT_SCOPE)

