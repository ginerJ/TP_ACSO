cmake_minimum_required(VERSION 3.27)
project(TP_ACSO C)

set(CMAKE_C_STANDARD 11)

include_directories(TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src)
include_directories(TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include)
include_directories(TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include-fixed)
include_directories(TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include-fixed/linux)
include_directories(TP1-ARM/src)
include_directories(TP2-x86_64/ej1)
include_directories(TP3-FileSystem)

add_executable(TP_ACSO
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src/gcov-io.c
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src/gcov-io.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src/gcov-iov.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src/libgcov-driver-kernel.c
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src/libgcov-driver.c
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src/libgcov-kernel.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src/libgcov-merge.c
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src/libgcov-profiler.c
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/gcov-src/libgcov.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/arm_neon.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/float.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/iso646.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/omp.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stdalign.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stdarg.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stdatomic.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stdbool.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stddef.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stdfix.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stdint-gcc.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stdint.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/stdnoreturn.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/unwind.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include/varargs.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include-fixed/linux/a.out.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include-fixed/limits.h
        TP1-ARM/aarch64-linux-android-4.9/lib/gcc/aarch64-linux-android/4.9/include-fixed/syslimits.h
        TP1-ARM/src/shell.c
        TP1-ARM/src/shell.h
        TP1-ARM/src/sim.c
        TP2-x86_64/bomb26/bomb.c
        TP2-x86_64/ej1/ej1.c
        TP2-x86_64/ej1/ej1.h
        TP2-x86_64/ej1/main.c
        TP2-x86_64/ej1/tester.c
        TP3-FileSystem/chksumfile.c
        TP3-FileSystem/chksumfile.h
        TP3-FileSystem/directory.c
        TP3-FileSystem/directory.h
        TP3-FileSystem/direntv6.h
        TP3-FileSystem/diskimageaccess.c
        TP3-FileSystem/diskimg.c
        TP3-FileSystem/diskimg.h
        TP3-FileSystem/file.c
        TP3-FileSystem/file.h
        TP3-FileSystem/filsys.h
        TP3-FileSystem/ino.h
        TP3-FileSystem/inode.c
        TP3-FileSystem/inode.h
        TP3-FileSystem/pathname.c
        TP3-FileSystem/pathname.h
        TP3-FileSystem/unixfilesystem.c
        TP3-FileSystem/unixfilesystem.h)
