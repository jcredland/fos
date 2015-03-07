# Tests
There should be a more comprehensive collection of tests for key classes.   Also, the build setup doesn't really make running the tests very easy.  Test related stuff is in _test folders but it'd be nice to be able to call it all from a master Makefile.  Also, the test are often running locally on my system, but that's using clang rather than the same version of GCC so they might not be valid in all cases. 

# Structure
A better map of the kernel architecture would help.  Some of this is started on Google Drive. 

# Auto-build for the disk image
disk2.img should be created by the Makefile.  Right now it's a manual job just containing test files.
