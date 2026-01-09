


#ifndef RUNA_RUNAAPI_H
#define RUNA_RUNAAPI_H

#ifdef _WIN32
#ifdef RUNA2_ENGINE_EXPORTS
#define RUNA_API __declspec(dllexport)
#else
#define RUNA_API __declspec(dllimport)
#endif
#else
#define RUNA_API
#endif

#endif
