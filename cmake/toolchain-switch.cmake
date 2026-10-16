# Cross-compilation toolchain file for Nintendo Switch homebrew.
#
# It reuses devkitPro's own CMake toolchain, so that the compiler, the compiler
# flags, the portlibs and the NRO packaging helpers (nx_create_nro()) all come
# from the devkitPro installation and stay up to date with it.
#
# CMake has to be run from the devkitPro MSYS2 shell ("MSYS2 MSYS"), where
# DEVKITPRO and the devkitA64/portlibs paths are set up:
#
#   cmake -S . -B build-switch -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-switch.cmake \
#         -DCMAKE_BUILD_TYPE=Release -DENABLE_SDL3=OFF
#   cmake --build build-switch
#
# Makefile.switch wraps these commands and also builds the .nro package.

if(NOT DEFINED ENV{DEVKITPRO})
	set(ENV{DEVKITPRO} /opt/devkitpro)
endif()
set(DEVKITPRO "$ENV{DEVKITPRO}")

include(${DEVKITPRO}/cmake/Switch.cmake)

# Hatari's data files (e.g. the window icon) are embedded in the NRO and read
# through RomFS (see src/paths.c)
set(BIN2DATADIR "romfs:/share/hatari" CACHE STRING
    "Relative path from bindir to datadir")

# Tells Hatari's CMakeLists that this is the Nintendo Switch port
set(HATARI_SWITCH TRUE)

# devkitA64's newlib ships <sys/times.h>, but neither sysconf() nor times()
# are available at link time, so the configuration check for that header has
# to be overridden: Hatari then uses SDL's wall clock for its performance
# counters instead of the process CPU time.
set(HAVE_SYS_TIMES_H 0 CACHE INTERNAL "<sys/times.h> is not usable on the Switch")

# devkitA64's GCC is a native program that is driven from an msys2 shell, so it
# writes the paths of its system headers (which live outside the source tree)
# into its dependency files in Windows form ("C:/devkitPro/..."). The msys2
# build of CMake does not recognize those as absolute paths and mangles them
# when it merges the compiler dependency files, which then aborts the build of
# the generated CPU emulator sources with "multiple target patterns".
# Letting CMake scan the include statements itself avoids this entirely.
set(CMAKE_DEPENDS_USE_COMPILER FALSE)
