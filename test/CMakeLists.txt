# Set up Google Test
add_subdirectory(gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Add test cpp file
cxx_executable(scalar_unittest symbol gtest_main)
