## Motors

### Directory Contents

This directory contains a number of simple sketches for testing the main motors of the robot.

#### OmniTestSoftware_1

This sketch drives the two motors connected to one Uno and effectively moves the Robot diagonally. *Note that we keep the speed of motor `M1` fixed and vary the speed of motor `M2`*.

This sketch attempts to make each encoder yield the same number of ticks over time and hopefully move in a straight line. We reset the encoder counts every `delayTime` milliseconds, but we keep a running total, i.e., we adjust speed based on the counts for the last period

**This does not work and the total counts diverge**.

#### OmniTestSoftware_2

This sketch is similar to `OmniTestSoftware_1`, but we adjust the motor speeds based on the total number of ticks and not just the number in each period.

**This works much better than `OmniTestSoftware_1`**.

#### OmniTestSoftware_2

Again, this sketch is similar to `OmniTestSoftware_1`, but its main use is to check that we are using the correct encoders, i.e., if we switch one motor on, then that motor's encoder should have a non-zero count. *Note that the other motor should have a zero count and if it does not, then there is something wrong with the hardware*.
