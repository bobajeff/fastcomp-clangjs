// Test clang changes for NaCl Support including:
//    include paths, library paths, emulation, default static
//
// RUN: %clang -### -o %t.o %s 2>&1 \
// RUN:     -target armv7a-unknown-nacl-gnueabihf \
// RUN:   | FileCheck --check-prefix=CHECK-ARM %s
// CHECK-ARM: {{.*}}clang{{.*}}" "-cc1"
// CHECK-ARM: "-target-cpu" "cortex-a8"
// CHECK-ARM: "-resource-dir" "{{.*}}/lib/clang/[[VER:[0-9.]+]]"
// CHECK-ARM: "-internal-isystem" "{{.*}}/../lib/clang/[[VER]]/include"
// CHECK-ARM: "-internal-isystem" "{{.*}}/../arm-nacl/include"
// CHECK-ARM: "-m" "armelf_nacl"
// CHECK-ARM: "-static"
// CHECK-ARM: "-L{{.*}}/../arm-nacl/lib"
// CHECK-ARM: "-L{{.*}}/../lib/clang/[[VER]]/lib/arm-nacl"
//
// RUN: %clang -### -o %t.o %s 2>&1 \
// RUN:     -target i686-unknown-nacl \
// RUN:   | FileCheck --check-prefix=CHECK-I686 %s
// CHECK-I686: {{.*}}clang{{.*}}" "-cc1"
// CHECK-I686: "-target-cpu" "pentium4"
// CHECK-I686: "-resource-dir" "{{.*}}/lib/clang/[[VER:[0-9.]+]]"
// CHECK-I686: "-internal-isystem" "{{.*}}/../lib/clang/[[VER]]/include"
// CHECK-I686: "-internal-isystem" "{{.*}}/../x86_64-nacl/include"
// CHECK-I686: "-m" "elf_i386_nacl"
// CHECK-I686: "-static"
// CHECK-I686: "-L{{.*}}/../x86_64-nacl/lib32"
// CHECK-I686: "-L{{.*}}/../lib/clang/[[VER]]/lib/i686-nacl"
//
// RUN: %clang -### -o %t.o %s 2>&1 \
// RUN:     -target x86_64-unknown-nacl \
// RUN:   | FileCheck --check-prefix=CHECK-x86_64 %s
// CHECK-x86_64: {{.*}}clang{{.*}}" "-cc1"
// CHECK-x86_64: "-target-cpu" "x86-64"
// CHECK-x86_64: "-resource-dir" "{{.*}}/lib/clang/[[VER:[0-9.]+]]"
// CHECK-x86_64: "-internal-isystem" "{{.*}}/../lib/clang/[[VER]]/include"
// CHECK-x86_64: "-internal-isystem" "{{.*}}/../x86_64-nacl/include"
// CHECK-x86_64: "-m" "elf_x86_64_nacl"
// CHECK-x86_64: "-static"
// CHECK-x86_64: "-L{{.*}}/../x86_64-nacl/lib"
// CHECK-x86_64: "-L{{.*}}/../lib/clang/[[VER]]/lib/x86_64-nacl"
