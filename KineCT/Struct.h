#pragma once

// License: MIT  http://opensource.org/licenses/MIT
// Author: dustpg   mailto:dustpg@gmail.com



// kinect ct namespace
namespace KineCT {
    // CT Option
    struct CTOPTION {
        // title text
        const wchar_t*      title;
        // sub-option length
        size_t              size;
#ifdef _MSC_VER
#pragma warning(disable: 4200)
        // sub-options
        const wchar_t*      subop[0];
#else 
        // sub-options
        const wchar_t*      subop[0];
#endif
    };
    // CT Media Type
    struct CTMEDIATYPE {
        // width of output
        uint32_t            width;
        // height of output
        uint32_t            height;
    };
}