// File: src/RunaAPI.h
// DLL Export/Import Macros for Windows

#pragma once

#ifdef _WIN32
    #ifdef RUNA2_ENGINE_EXPORTS
        #define RUNA_API __declspec(dllexport)
    #else
        #define RUNA_API __declspec(dllimport)
    #endif
#else
    #define RUNA_API
#endif

