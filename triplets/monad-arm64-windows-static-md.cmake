set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_C_FLAGS "/arch:armv8.1 /guard:ehcont")
set(VCPKG_CXX_FLAGS "/arch:armv8.1 /guard:ehcont")
set(VCPKG_LINKER_FLAGS "/arch:armv8.1 /force:guardehcont")