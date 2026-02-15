#pragma once

#define SS_ENABLE_PROFILING 0

#if SS_ENABLE_PROFILING
    #include <Tracy.hpp>
    #define SS_PROFILE_FRAME_MARK() FrameMark
    #define SS_PROFILE_FUNCTION() ZoneScoped
    #define SS_PROFILE_SCOPE(name) ZoneScopedN(name)
#else
    #define SS_PROFILE_FRAME_MARK()
    #define SS_PROFILE_SCOPE(name)
    #define SS_PROFILE_FUNCTION()
#endif


