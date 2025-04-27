#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <scruffy/scruffy.h>

TEST(kevin, bob) {
    EXPECT_TRUE(1 == 2);
    EXPECT_TRUE(1 == 1);

    float a = 3.12;
    EXPECT_FLOAT_EQ(a, 3.12f);
    EXPECT_FLOAT_EQ(a, 3.0f);

    double b = 12.34567;
    EXPECT_DOUBLE_EQ(b, 12.34);
}

TEST(kevin, sam) {
    EXPECT_TRUE(1 == 1);
    EXPECT_TRUE(1 == 1);
    EXPECT_TRUE(1 == 1);
    EXPECT_FALSE(1);
}

RUN_ALL();