#include "gtest/gtest.h"
#include "testsetup.h"
#include "util.h"
#include "typedefs.h"
#include "timelapseconfigbuilder.h"
#include "alloca.h"

TEST(TimelapseConfigBuilder, BuildsConfigFromString) {
  const i32 size = 512;
  void* memory = alloca(size);
  lh::TimelapseConfigBuilder builder;

  lh::TimelapseConfiguration config = {0};
  i32 err = builder.build_configuration(
    &config, memory, size,
    (char*)"^i:1000,d:100,n:2,t:25,s:50,t:100,s:200$");

  ASSERT_EQ(err, 0);
  EXPECT_EQ(config.interval_in_millis, 1000);
  EXPECT_EQ(config.delay_in_millis, 100);
  EXPECT_EQ(config.spline.n, 2);
  EXPECT_EQ(config.spline.xs[0], 0.0f);
  EXPECT_EQ(config.spline.xs[1], 25.0f);
  EXPECT_EQ(config.spline.as[0], 0.0f);
  EXPECT_EQ(config.spline.as[1], 50.0f);
  EXPECT_EQ(0, eval_spline(0.0f, &config.spline));
  EXPECT_EQ(100, eval_spline(50.0f, &config.spline));
  EXPECT_EQ(200, eval_spline(100.0f, &config.spline));
}

TEST(TimelapseConfigBuilder, AppropriatelyOverwritesOldConfig) {
  const i32 size = 512;
  void* memory = alloca(size);
  lh::TimelapseConfigBuilder builder;

  lh::TimelapseConfiguration config = {0};
  i32 err = builder.build_configuration(
    &config, memory, size,
    (char*)"^i:1000,d:100,n:2,t:25,s:50,t:100,s:200$");

  ASSERT_EQ(err, 0);

  err = builder.build_configuration(
    &config, memory, size,
    (char*)"^i:1000,d:100,n:1,t:1000,s:5000$");

  ASSERT_EQ(err, 0);
  EXPECT_EQ(config.interval_in_millis, 1000);
  EXPECT_EQ(config.delay_in_millis, 100);
  EXPECT_EQ(config.spline.n, 1);
  EXPECT_EQ(config.spline.xs[0], 0.0f);
  EXPECT_EQ(config.spline.xs[1], 1000.0f);
  EXPECT_EQ(config.spline.as[0], 0.0f);
  EXPECT_EQ(config.spline.as[1], 5000.0f);
}

TEST(TimelapseConfigBuilder, HandlesNotEnoughMemory) {
  const i32 size = 64;
  void* memory = alloca(size);
  lh::TimelapseConfigBuilder builder;

  lh::TimelapseConfiguration config = {0};
  i32 err = builder.build_configuration(
    &config, memory, size,
    (char*)"^i:1000,d:100,n:20$");

  ASSERT_EQ(err, -4);
}