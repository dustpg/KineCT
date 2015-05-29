#pragma once
// License: MIT  http://opensource.org/licenses/MIT
// Author: dustpg   mailto:dustpg@gmail.com


// lengthof array
#define lengthof(x) (sizeof((x))/(sizeof(*(x))))

// using kinect ver1
#define KINECT_INCLUDE_VERSION_1
// using kinect ver2
#define KINECT_INCLUDE_VERSION_2

// no vtable
#ifdef _MSC_VER
#define KINECT_NOVTABLE __declspec(novtable)
#else
#define KINECT_NOVTABLE
#endif

// argument in
#define KINECT_IN
// argument out
#define KINECT_OUT
// argument optional
#define KINECT_OPTIONAL
// you should check the pointer
#define KINECT_CHECKPOINTER



