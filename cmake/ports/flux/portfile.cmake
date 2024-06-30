vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  tcbrindle/flux
  REF
  d88fd970d26ec6fc297965ce1dd60b81a5b2def1
  SHA512
  d390f586d45b69dbad7f8c099382e1bec436b7d864b3797b97c7b69ade7d510c375b763cdd3dbf63356358cc500dad8260df1538e8f0d942119f440ae53f4bd3
  HEAD_REF
  main)

# Copy header files
file(
  INSTALL ${SOURCE_PATH}/include
  DESTINATION ${CURRENT_PACKAGES_DIR}
  FILES_MATCHING
  PATTERN "*.hpp")

file(
  INSTALL "${SOURCE_PATH}/LICENSE_1_0.txt"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)