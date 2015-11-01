# Set up Google Test
add_subdirectory(googletest/googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Add test cpp file
cxx_executable(expression_unittest symbol gtest_main)
cxx_executable(exp_unittest symbol/impl_ gtest_main)
