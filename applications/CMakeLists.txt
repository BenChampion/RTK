FIND_PACKAGE(Gengetopt)

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# add path to RTK libraries to the install path
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${RTK_INSTALL_LIB_DIR}")
#=========================================================
# Executables
ADD_SUBDIRECTORY(rtkprojections)
ADD_SUBDIRECTORY(rtkparkershortscanweighting)
ADD_SUBDIRECTORY(rtkwangdisplaceddetectorweighting)
ADD_SUBDIRECTORY(rtkfdktwodweights)
ADD_SUBDIRECTORY(rtkramp)
ADD_SUBDIRECTORY(rtkfdk)
ADD_SUBDIRECTORY(rtkinlinefdk)
ADD_SUBDIRECTORY(rtkamsterdamshroud)
ADD_SUBDIRECTORY(rtkforwardprojections)
ADD_SUBDIRECTORY(rtkbackprojections)
ADD_SUBDIRECTORY(rtkrayboxintersection)
ADD_SUBDIRECTORY(rtksart)
ADD_SUBDIRECTORY(rtkrayquadricintersection)
ADD_SUBDIRECTORY(rtkprojectgeometricphantom)
ADD_SUBDIRECTORY(rtkdrawgeometricphantom)
ADD_SUBDIRECTORY(rtkprojectshepploganphantom)
ADD_SUBDIRECTORY(rtkfieldofview)
ADD_SUBDIRECTORY(rtksubselect)
ADD_SUBDIRECTORY(rtkdrawshepploganphantom)
ADD_SUBDIRECTORY(rtkextractshroudsignal)
ADD_SUBDIRECTORY(rtkmedian)
ADD_SUBDIRECTORY(rtklut)
ADD_SUBDIRECTORY(rtktotalvariationdenoising)
ADD_SUBDIRECTORY(rtkadmmwavelets)
ADD_SUBDIRECTORY(rtkadmmtotalvariation)
ADD_SUBDIRECTORY(rtkfourdrooster)
ADD_SUBDIRECTORY(rtkfourdconjugategradient)
ADD_SUBDIRECTORY(rtkconjugategradient)
ADD_SUBDIRECTORY(rtkwaveletsdenoising)

#All the executables below are meant to create RTK ThreeDCircularProjectionGeometry files
ADD_SUBDIRECTORY(rtkvarianobigeometry)
ADD_SUBDIRECTORY(rtksimulatedgeometry)
ADD_SUBDIRECTORY(rtkelektasynergygeometry)
ADD_SUBDIRECTORY(rtkdigisensgeometry)
ADD_SUBDIRECTORY(rtkxradgeometry)
#=========================================================

# Testing of the applications
if(BUILD_TESTING)

  add_test(rtkappsimulatedgeometrytest ${EXECUTABLE_OUTPUT_PATH}/rtksimulatedgeometry -n 180 --sid 1000 --sdd 1500 -o geo)

  add_test(rtkappprojectshepploganphantomtest ${EXECUTABLE_OUTPUT_PATH}/rtkprojectshepploganphantom  -o sheppy.mha -g geo --phantomscale 40 --dimension 128)
  set_tests_properties(rtkappprojectshepploganphantomtest PROPERTIES DEPENDS apprtksimulatedgeometrytest)
 
  if(RTK_USE_CUDA AND CUDA_HAVE_GPU)
    add_test(rtkappfdktest ${EXECUTABLE_OUTPUT_PATH}/rtkfdk -g geo -p . -r sheppy.mha -o fdk_gpu.mha --hardware cuda)
	set_tests_properties(rtkappfdktest PROPERTIES DEPENDS rtkappprojectshepploganphantomtest)
    add_test(rtkappfdkchecktest ${EXECUTABLE_OUTPUT_PATH}/rtkcheckimagequality fdk_gpu.mha)
    set_tests_properties(rtkappfdkchecktest PROPERTIES DEPENDS rtkappfdktest)
  else()
    # Only run the software version if 
    if(NOT FAST_TESTS_NO_CHECKS)
      add_test(rtkappfdktest ${EXECUTABLE_OUTPUT_PATH}/rtkfdk -g geo -p . -r sheppy.mha -o fdk_gpu.mha)
	  set_tests_properties(rtkappfdktest PROPERTIES DEPENDS rtkappprojectshepploganphantomtest)
      add_test(rtkappfdkchecktest ${EXECUTABLE_OUTPUT_PATH}/rtkcheckimagequality fdk_gpu.mha)
      set_tests_properties(rtkappfdkchecktest PROPERTIES DEPENDS rtkappfdktest)
	endif()
  endif()
 


endif()
