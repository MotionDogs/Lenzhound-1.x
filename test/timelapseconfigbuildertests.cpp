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
    (char*)"^i:1000,d:100,x:0,x:25,x:100,y:0,y:50,y:200$");

  ASSERT_FALSE(err);
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

TEST(TimelapseConfigBuilder, HandlesNotEnoughMemory) {
  const i32 size = 64;
  void* memory = alloca(size);
  lh::TimelapseConfigBuilder builder;

  lh::TimelapseConfiguration config = {0};
  i32 err = builder.build_configuration(
    &config, memory, size,
    (char*)"^i:1000,d:100,x:0,x:0,x:0,x:0,x:0,x:0,y:0,y:0,y:0,y:0,y:0,y:0$");

  ASSERT_TRUE(err == -1);
}

TEST(TimelapseConfigBuilder, FigureOutHowMuchMemoryWeNeed) {
  const i32 size = 512;
  void* memory = alloca(size);
  lh::TimelapseConfigBuilder builder;

  // 512 mb can support 12 points
  lh::TimelapseConfiguration config = {0};
  i32 err = builder.build_configuration(
    &config, memory, size,(char*)
    "^i:1000,d:100"
    ",x:0,x:0,x:0,x:0,x:0,x:0"
    ",x:0,x:0,x:0,x:0,x:0,x:0"
    ",y:0,y:0,y:0,y:0,y:0,y:0"
    ",y:0,y:0,y:0,y:0,y:0,y:0"
    "$");

  ASSERT_FALSE(err);
}