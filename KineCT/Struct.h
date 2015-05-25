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
        // sub-options
        const wchar_t*      subop[];
    };
}