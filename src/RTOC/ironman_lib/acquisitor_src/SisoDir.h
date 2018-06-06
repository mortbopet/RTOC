#pragma once
#ifndef SISODIR_H
#define SISODIR_H

#include <cstdlib>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

static const char sisodir_envname[] = "SISODIR5";
#if defined(_WIN64) || (__WORDSIZE == 64)
static const char sisodir_envname_platform[] = "SISODIR5_64";
#else /* 64 bit */
static const char sisodir_envname_platform[] = "SISODIR5_32";
#endif /* 64 bit */

#ifdef _WIN32
#define DIRSEP "\\"
#define DIRSEP_CHAR '\\'
#else
#include <stdint.h>
#include <stdio.h>
#define DIRSEP "/"
#define DIRSEP_CHAR '/'
typedef uint32_t DWORD;
#endif

#define SISODIR_MAX_LEN 1024
#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

#include <os_shlib.h>
#include <os_macros.h>
#include <siso/templates/platform/SpinLock.h>
using namespace siso::templates::platform;

#ifdef _WIN32
#include <windows.h>
#ifndef strcasecmp
    #define strcasecmp _stricmp
    #define HAS_STRCASECMP
#endif
#endif

// Runtime library init functions for static loaders
extern "C" int Fg_OpenLib(const char * path);
extern "C" int SHA_OpenLib(void);
extern "C" int SHW_OpenLib(void);
extern "C" int HAPRT_OpenLib(void);

template <typename Dummy> struct EXPORT_VISIBILITY_HIDDEN SisoLibStatic
{
    static SpinLockVariable gmInitLock;
    static bool gmIsInitialized;
    static char gmSisoDir[SISODIR_MAX_LEN];
};

template <typename Dummy> SpinLockVariable SisoLibStatic<Dummy>::gmInitLock = SPINLOCK_VARIABLE_INITIALIZER;
template <typename Dummy> bool SisoLibStatic<Dummy>::gmIsInitialized = false;
template <typename Dummy> char SisoLibStatic<Dummy>::gmSisoDir[SISODIR_MAX_LEN];

class EXPORT_VISIBILITY_HIDDEN SisoDir : private SisoLibStatic<void>
{
    SisoDir(const SisoDir &);
    SisoDir operator=(const SisoDir &);

public:
    // \brief Get SISODIR
    // \returns Pointer to SISODIR string; SISODIR will never include the trailing directory separator
    static const char * GetSisoDir()
    {
        SpinLockVariableGuard guard(&gmInitLock);

        if (gmIsInitialized) {
            return gmSisoDir;
        }

        // Get SISODIR from module handle
        size_t len = 0;
#if defined(_WIN32)
        HMODULE hModule = NULL;
        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, gmSisoDir, &hModule)) {
            len = GetModuleFileNameA(hModule, gmSisoDir, sizeof(gmSisoDir));
            if (len == sizeof(gmSisoDir) && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                len = 0;
            }
        }
#elif !defined(QNX)
        Dl_info info;
        if (dladdr(gmSisoDir, &info)) {
            len = strlen(info.dli_fname);
            if (len > 0 && len < sizeof(gmSisoDir)) {
                strcpy(gmSisoDir, info.dli_fname);
            } else {
                len = 0;
            }
        }
#endif
        if (len > 0 && len < sizeof(gmSisoDir)) {
            // Look for last directory separator
            char * dirspec = strrchr(gmSisoDir, DIRSEP_CHAR);
            if (dirspec != NULL) {
                // Now we need to find the root for the SISODIR
                while (dirspec > gmSisoDir) {
                    // Look at last directory in path ...
                    while (*dirspec == DIRSEP_CHAR && dirspec > gmSisoDir) {
                        --dirspec;
                    }
                    *(dirspec + 1) = '\0';
                    while (*dirspec != DIRSEP_CHAR && dirspec > gmSisoDir) {
                        --dirspec;
                    }

                    if (!strcasecmp(dirspec + 1, "bin") ||
                        !strcasecmp(dirspec + 1, "dll") ||
                        !strcasecmp(dirspec + 1, "Hardware Applets") ||
                        !strcasecmp(dirspec + 1, "lib") ||
                        !strcasecmp(dirspec + 1, "lib64")) {
                            while (*dirspec == DIRSEP_CHAR && dirspec > gmSisoDir) {
                                --dirspec;
                            }
                            *(dirspec + 1) = '\0';
                            gmIsInitialized = true;
                            return gmSisoDir;
                    }
                }
            }
        }

        // Get SISODIR_32/_64 environment variable, fallback to SISODIR5
        const char * sisodirenv = getenv(sisodir_envname_platform);
        if (sisodirenv == NULL)
            sisodirenv = getenv(sisodir_envname);
        if (sisodirenv != NULL) {
            strncpy(gmSisoDir, sisodirenv, sizeof(gmSisoDir));
            gmSisoDir[sizeof(gmSisoDir) - 1] = '\0';
            char * dirspec = gmSisoDir + strlen(gmSisoDir) - 1;
            while (*dirspec == DIRSEP_CHAR && dirspec > gmSisoDir) {
                --dirspec;
            }
            *(dirspec + 1) = '\0';
            gmIsInitialized = true;
            return gmSisoDir;
        }

        return NULL;
    }

    // \brief Load runtime library
    // \param libname Name of the library (without extension .dll, .so, ...)
    // \param path Path to Runtime root directory (optional; not path to library!)
    // \returns new SharedLibrary Object
    static SharedLibrary * LoadRuntimeLibrary(const char * libname, const char * path = NULL)
    {

        const char * sisodir = path;
        if (sisodir == NULL)
            sisodir = GetSisoDir();
        if (sisodir != NULL) {
            char path[SISODIR_MAX_LEN];

#if defined(_WIN32)
            sprintf(path, "%s\\bin\\%s.dll", sisodir, libname);
#elif defined(__APPLE__)
            sprintf(path, "%s/lib64/lib%s.dylib", sisodir, libname);
#else
#	if defined(__x86_64__)
            sprintf(path, "%s/lib64/lib%s.so", sisodir, libname);
#	else
            sprintf(path, "%s/lib/lib%s.so", sisodir, libname);
#endif
#endif
            SharedLibrary * lib = new SharedLibrary(path, false);
            if (lib->isOpen()) {
                return lib;
            }

            delete lib;
        }

        return NULL;
    }

    // \brief Load supplementary library
    // \param libname Name of the library (without extension .dll, .so, ...)
    // \param path Path to Runtime root directory (optional; not path to library!)
    // \returns new, unguarded SharedLibrary Object
    static SharedLibrary * LoadSupplementaryLibrary(const char * libname, const char * path = NULL)
    {

        const char * sisodir = path;
        if (sisodir == NULL)
            sisodir = GetSisoDir();
        if (sisodir != NULL) {
            char path[SISODIR_MAX_LEN];

#if defined(_WIN32)
            sprintf(path, "%s\\lib\\%s.dll", sisodir, libname);
#elif defined(__APPLE__)
            sprintf(path, "%s/lib64/lib%s.dylib", sisodir, libname);
#else
#	if defined(__x86_64__)
            sprintf(path, "%s/lib64/lib%s.so", sisodir, libname);
#	else
            sprintf(path, "%s/lib/lib%s.so", sisodir, libname);
#endif
#endif
            SharedLibrary * lib = new SharedLibrary(path, false);
            if (lib->isOpen()) {
                return lib;
            }

            delete lib;
        }

        return NULL;
    }
};

#if 0 /* Deprecated old API */

#ifdef _WIN32

/**
 * \brief return the runtime base directory, relative to library instance
 * 
 * \param moduleroot buffer to hold the module root directory
 * \param size buffer size
 * \param library handle to library instance
 * \param depth directory hierarchy depth relative to base directory
 *              (1 for runtime library, 2 for standard applet)
 *
 * This will look up the runtime directory of the current process
 * using the loaded library, and SISODIR5 environment variables as fallback.
 */
static inline const char *
GetSisodirFromLibraryInstance(char * moduleroot, size_t size, HINSTANCE library, unsigned int depth)
{
	const char * sisodir = NULL;

	DWORD result = 0;

	// Get SiSo root directory for this DLL
	if ((library != NULL) && (moduleroot != NULL) && (size > 0)) {
		result = GetModuleFileNameA(library, moduleroot, (DWORD) size);
		if (result && (result < size)) {
			char * file = strrchr(moduleroot, '\\');
			if (file != NULL) {
				(* file) = '\0';
				while (depth--)
					strncat(moduleroot, "\\..", size);
				sisodir = moduleroot;
			}
		}
	}

	// Fallback to SISODIR5_32/64
	if (sisodir == NULL)
		sisodir = getenv(sisodir_envname_platform);

	// Fallback to SISODIR5
	if (sisodir == NULL)
		sisodir = getenv(sisodir_envname);

	return sisodir;
}
#endif

#include "os_macros.h"

#ifdef WITH_QT4
#include <qglobal.h>
#include <QString>

#include <string>

/**
 * \brief return the runtime base directory as QString
 * 
 * This is a Qt-version of GetSisodir().
 */
static inline QString
GetQSisodir(void)
{
	QString res = QString::fromUtf8(SisoLib::GetSisoDir());
	return res;
}

#endif /* WITH_QT4 */

/**
 * \brief return the runtime base directory
 * 
 * This will look up the runtime directory of the current installation
 * using the SISODIR5 environment variables.
 */
static inline const char *
GetSisodir(void)
{
	const char * sisodir = getenv(sisodir_envname_platform);

	if (sisodir == NULL)
		sisodir = getenv(sisodir_envname);

	return sisodir;
}

#ifndef _WIN32
typedef void * HINSTANCE;
#endif

/**
 * \brief return the runtime base directory, relative to library
 * 
 * \param moduleroot buffer to hold the module root directory
 * \param size buffer size
 * \param libname name of the library
 * \param depth directory hierarchy depth relative to base directory
 *              (1 for runtime library, 2 for standard applet)
 *
 * This will look up the runtime directory of the current process
 * using the loaded library, and SISODIR5 environment variables as fallback.
 */
static inline const char *
GetSisodirFromLibrary(char * moduleroot, size_t size, const char * libname, unsigned int depth)
{
	HINSTANCE library = NULL;

#ifdef _WIN32
	// Get SiSo root directory for this DLL
	library = GetModuleHandleA(libname);
#else
    UNUSED(libname);
#endif

	return GetSisodirFromLibraryInstance(moduleroot, size, library, depth);
}

#ifdef __cplusplus
/**
 * \brief return the runtime library directory
 * \return the library directory
 * 
 * This will return the library directory where the shared libraries
 * are located.
 * 
 * The return value either contains the directory name including the
 * trailing '/' or is empty in case no environment variable was set.
 */
static inline std::string
GetSisoLibdir(void)
{
	const char *envdir = GetSisodir();
	if (envdir == NULL)
		return std::string();

	std::string res = envdir;
#if !defined(_WIN32) && (__WORDSIZE == 64)
	// 64 bit Linux platforms use /lib64 to store the libraries
	res += "/lib64/";
#else /* 64 bit Un*x */
	res += "/lib/";
#endif /* 64 bit Un*x */

	return res;
}
#endif /* __cplusplus */
#endif

#endif /* SISODIR_H */
