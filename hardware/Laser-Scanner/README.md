## laser scanner

### Directory Contents

This directory contains a number of simple sketches for testing the laser scanner.

#### laser_moto_test_1

This sketch checks that the DC motor & encoder are working. The function `encoderIS()` handles the encoder interrupts. There should be 3360 per revolution of the laser scanner. The sketch also contains code to drive the DC motor at different speeds.

#### laser_moto_test_2

This is essentially the same as `laser_moto_test_1` except that it stops after 15 revolution of the laser scanner.

#### laser-single-distance

This example is supplied by the manufacturer.

#### laser-continuous-distance

This example is supplied by the manufacturer.

#### laser-continuous-distance-interrupt

This is a modification of `laser-continuous-distance` that uses interrupts to determine when a new reading has arrived.

#### combined_test_1

This is a combined test of the laser and DC motor, i.e., the head rotates and laser fires continuously. **This sketch needs more work.**

### Issues

1. It is essential that we handle every interrupt generated by the encoder. **How can we assure that this happens even when other code is executing? For example, does the `Serial` library interfere with interrupts, e.g., by disabling them at any stage?**
1. Using `laser_moto_test_2` it is clear that the head does not finish in same position as it started. **Why?**
1. The `combined_test_1` sketch does not perform very well; it behaviour is some what erratic. This probably due time issues and lost interrupts.
1. **If the inaccuracies in the rotation the the scanner and the laser range finding are significant what can we do in software to improve accuracy? **  
