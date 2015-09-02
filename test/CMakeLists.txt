add_executable(tests 
  testsetup.cpp
  motorcontrollertests.cpp
  timelapsecontrollertests.cpp
  timelapseconfigbuildertests.cpp
)
target_link_libraries(tests gtest_main lenzhound_core)