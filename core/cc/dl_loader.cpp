/*
 * Copyright (C) 2017 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dl_loader.h"
#include "log.h"
#include "target.h"

#if TARGET_OS == GAPID_OS_WINDOWS
#   include <windows.h>
#elif TARGET_OS == GAPID_OS_OSX
#   include <dlfcn.h>
#   include <unistd.h>
#else
#   include <dlfcn.h>
#endif

namespace {

// load defs
void* load(const char* name) {
#if TARGET_OS == GAPID_OS_WINDOWS
    return reinterpret_cast<void*>(LoadLibraryExA(name, NULL, 0));
#elif TARGET_OS == GAPID_OS_OSX
    if (name == nullptr) {
        return nullptr;
    }
    // DYLD_FRAMEWORK_PATH takes precedence even with absolute paths.
    // Use a symlink to get to the real library.
    // Credit to apitrace (https://github.com/apitrace) for this nasty, but
    // effective work-around.
    // TODO: not thread-safe.
    void* res = nullptr;
    char tmp[] = "/tmp/dlopen.XXXXXX";
    if (mktemp(tmp) != nullptr) {
        if (symlink(name, tmp) == 0) {
            res = dlopen(tmp, RTLD_NOW | RTLD_LOCAL | RTLD_FIRST);
            remove(tmp);
        }
    }
    return res;
#else
    return dlopen(name, RTLD_NOW | RTLD_LOCAL);
#endif // TARGET_OS
}

void* must_load(const char* name) {
  void* res = load(name);
  if (res == nullptr) {
#if TARGET_OS == GAPID_OS_WINDOWS
    GAPID_FATAL("Can't load library %s: %d", name, GetLastError());
#else
    GAPID_FATAL("Can't load library %s: %s", name, dlerror());
#endif // TARGET_OS
  }
  return res;
}

// resolve defs
#if TARGET_OS ==  GAPID_OS_WINDOWS
void* resolve(void* handle, const char* name) {
    return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(handle), name));
}
#else // TARGET_OS
void* resolve(void* handle, const char* name) {
    return dlsym(handle, name);
}
#endif // TARGET_OS


// close defs
#if TARGET_OS == GAPID_OS_WINDOWS
void close(void* lib) {
    if (lib != nullptr) {
        FreeLibrary(reinterpret_cast<HMODULE>(lib));
    }
}
#else // TARGET_OS
void close(void* lib) {
    if (lib != nullptr) {
        dlclose(lib);
    }
}
#endif // TARGET_OS

}  // anonymous namespace

namespace core {

DlLoader::DlLoader(const char* name) : mLibrary(must_load(name)) {}

DlLoader::~DlLoader() {
    close(mLibrary);
}

#if TARGET_OS == GAPID_OS_WINDOWS
void* DlLoader::lookup(const char* name) {
    return resolve(mLibrary, name);
}
#else  // TARGET_OS
void* DlLoader::lookup(const char* name) {
    return resolve((mLibrary ? mLibrary : RTLD_DEFAULT), name);
}
#endif  // TARGET_OS

bool DlLoader::can_load(const char* lib_name) {
  if (void* lib = load(lib_name)) {
    close(lib);
    return true;
  }
  return false;
}

}  // namespace core
