// RUN: %clang_cc1 %s -fsyntax-only -verify -pedantic -Wno-string-plus-int -triple=i686-apple-darwin9
// This test needs to set the target because it uses __builtin_ia32_vec_ext_v4si

int test1(float a, int b) {
  return __builtin_isless(a, b); // expected-note {{declared here}}
}
int test2(int a, int b) {
  return __builtin_islessequal(a, b);  // expected-error {{floating point type}}
}

int test3(double a, float b) {
  return __builtin_isless(a, b);
}
int test4(int* a, double b) {
  return __builtin_islessequal(a, b);  // expected-error {{floating point type}}
}

int test5(float a, long double b) {
  return __builtin_isless(a, b, b);  // expected-error {{too many arguments}}
}
int test6(float a, long double b) {
  return __builtin_islessequal(a);  // expected-error {{too few arguments}}
}


#define CFSTR __builtin___CFStringMakeConstantString
void test7() {
  const void *X;
  X = CFSTR("\242"); // expected-warning {{input conversion stopped}}
  X = CFSTR("\0"); // no-warning
  X = CFSTR(242); // expected-error {{CFString literal is not a string constant}} expected-warning {{incompatible integer to pointer conversion}}
  X = CFSTR("foo", "bar"); // expected-error {{too many arguments to function call}}
}


// atomics.

void test9(short v) {
  unsigned i, old;

  old = __sync_fetch_and_add();  // expected-error {{too few arguments to function call}}
  old = __sync_fetch_and_add(&old);  // expected-error {{too few arguments to function call}}
  old = __sync_fetch_and_add((unsigned*)0, 42i); // expected-warning {{imaginary constants are a GNU extension}}

  // PR7600: Pointers are implicitly casted to integers and back.
  void *old_ptr = __sync_val_compare_and_swap((void**)0, 0, 0);

  // Ensure the return type is correct even when implicit casts are stripped
  // away. This triggers an assertion while checking the comparison otherwise.
  if (__sync_fetch_and_add(&old, 1) == 1) {
  }
}

// overloaded atomics should be declared only once.
void test9_1(volatile int* ptr, int val) {
  __sync_fetch_and_add_4(ptr, val);
}
void test9_2(volatile int* ptr, int val) {
  __sync_fetch_and_add(ptr, val);
}
void test9_3(volatile int* ptr, int val) {
  __sync_fetch_and_add_4(ptr, val);
  __sync_fetch_and_add(ptr, val);
  __sync_fetch_and_add(ptr, val);
  __sync_fetch_and_add_4(ptr, val);
  __sync_fetch_and_add_4(ptr, val);
}

// rdar://7236819
void test10(void) __attribute__((noreturn));

void test10(void) {
  __asm__("int3");
  __builtin_unreachable();

  // No warning about falling off the end of a noreturn function.
}

void test11(int X) {
  switch (X) {
  case __builtin_eh_return_data_regno(0):  // constant foldable.
    break;
  }

  __builtin_eh_return_data_regno(X);  // expected-error {{argument to '__builtin_eh_return_data_regno' must be a constant integer}}
}

// PR5062
void test12(void) __attribute__((__noreturn__));
void test12(void) {
  __builtin_trap();  // no warning because trap is noreturn.
}

void test_unknown_builtin(int a, int b) {
  __builtin_isles(a, b); // expected-error{{use of unknown builtin}} \
                         // expected-note{{did you mean '__builtin_isless'?}}
}

int test13() {
  __builtin_eh_return(0, 0); // no warning, eh_return never returns.
}

// <rdar://problem/8228293>
void test14() {
  int old;
  old = __sync_fetch_and_min((volatile int *)&old, 1);
}

// <rdar://problem/8336581>
void test15(const char *s) {
  __builtin_printf("string is %s\n", s);
}

// PR7885
int test16() {
  return __builtin_constant_p() + // expected-error{{too few arguments}}
         __builtin_constant_p(1, 2); // expected-error {{too many arguments}}
}

const int test17_n = 0;
const char test17_c[] = {1, 2, 3, 0};
const char test17_d[] = {1, 2, 3, 4};
typedef int __attribute__((vector_size(16))) IntVector;
struct Aggregate { int n; char c; };
enum Enum { EnumValue1, EnumValue2 };

typedef __typeof(sizeof(int)) size_t;
size_t strlen(const char *);

void test17() {
#define ASSERT(...) { int arr[(__VA_ARGS__) ? 1 : -1]; }
#define T(...) ASSERT(__builtin_constant_p(__VA_ARGS__))
#define F(...) ASSERT(!__builtin_constant_p(__VA_ARGS__))

  // __builtin_constant_p returns 1 if the argument folds to:
  //  - an arithmetic constant with value which is known at compile time
  T(test17_n);
  T(&test17_c[3] - test17_c);
  T(3i + 5); // expected-warning {{imaginary constant}}
  T(4.2 * 7.6);
  T(EnumValue1);
  T((enum Enum)(int)EnumValue2);

  //  - the address of the first character of a string literal, losslessly cast
  //    to any type
  T("string literal");
  T((double*)"string literal");
  T("string literal" + 0);
  T((long)"string literal");

  // ... and otherwise returns 0.
  F("string literal" + 1);
  F(&test17_n);
  F(test17_c);
  F(&test17_c);
  F(&test17_d);
  F((struct Aggregate){0, 1});
  F((IntVector){0, 1, 2, 3});

  // Ensure that a technique used in glibc is handled correctly.
#define OPT(...) (__builtin_constant_p(__VA_ARGS__) && strlen(__VA_ARGS__) < 4)
  // FIXME: These are incorrectly treated as ICEs because strlen is treated as
  // a builtin.
  ASSERT(OPT("abc"));
  ASSERT(!OPT("abcd"));
  // In these cases, the strlen is non-constant, but the __builtin_constant_p
  // is 0: the array size is not an ICE but is foldable.
  ASSERT(!OPT(test17_c));        // expected-warning {{folded}}
  ASSERT(!OPT(&test17_c[0]));    // expected-warning {{folded}}
  ASSERT(!OPT((char*)test17_c)); // expected-warning {{folded}}
  ASSERT(!OPT(test17_d));        // expected-warning {{folded}}
  ASSERT(!OPT(&test17_d[0]));    // expected-warning {{folded}}
  ASSERT(!OPT((char*)test17_d)); // expected-warning {{folded}}

#undef OPT
#undef T
#undef F
}

void test18() {
  char src[1024];
  char dst[2048];
  size_t result;
  void *ptr;

  ptr = __builtin___memccpy_chk(dst, src, '\037', sizeof(src), sizeof(dst));
  result = __builtin___strlcpy_chk(dst, src, sizeof(src), sizeof(dst));
  result = __builtin___strlcat_chk(dst, src, sizeof(src), sizeof(dst));

  ptr = __builtin___memccpy_chk(dst, src, '\037', sizeof(src));      // expected-error {{too few arguments to function call}}
  ptr = __builtin___strlcpy_chk(dst, src, sizeof(src), sizeof(dst)); // expected-warning {{incompatible integer to pointer conversion}}
  ptr = __builtin___strlcat_chk(dst, src, sizeof(src), sizeof(dst)); // expected-warning {{incompatible integer to pointer conversion}}
}

void no_ms_builtins() {
  __assume(1); // expected-warning {{implicit declaration}}
  __noop(1); // expected-warning {{implicit declaration}}
  __debugbreak(); // expected-warning {{implicit declaration}}
}

void unavailable() {
  __builtin_operator_new(0); // expected-error {{'__builtin_operator_new' is only available in C++}}
  __builtin_operator_delete(0); // expected-error {{'__builtin_operator_delete' is only available in C++}}
}
