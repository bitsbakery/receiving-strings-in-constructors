# Receiving strings in constructors

This test compares different methods of receiving string parameters in constructors, taking into account all optimizations performed by the compiler.

The test counts the number of invocations of all constructors and assignment operators of a special class `DbgStr`.

On `Microsoft Visual Studio Community 2022 (64-bit) Version 17.4.1` with C++ language standard set to `ISO C++20 Standard (/std:c++20)` in project settings, the test produces the following results:

```
ctor(cstr) ctor()  copy()  copy=   move()  move=
1          0       1       0       0       0       struct UserConstRef from a string literal       test string
1          0       1       0       0       0       struct UserConstRef from l-value reference      test string
1          0       1       0       0       0       struct UserConstRef from r-value reference      test string
1          0       1       0       0       0       struct UserConstRef from r-value reference2     test string

1          0       0       0       1       0       struct UserByVal from a string literal          test string
1          0       1       0       1       0       struct UserByVal from l-value reference         test string
1          0       0       0       2       0       struct UserByVal from r-value reference         test string
1          0       0       0       1       0       struct UserByVal from r-value reference2        test string

1          0       1       0       0       0       struct UserByRVal from a string literal         test string
1          0       1       0       0       0       struct UserByRVal from r-value reference        test string
1          0       1       0       0       0       struct UserByRVal from r-value reference2       test string

1          0       0       0       0       0       struct UserPerfectFwd from a string literal     test string
1          0       1       0       0       0       struct UserPerfectFwd from l-value reference    test string
1          0       0       0       1       0       struct UserPerfectFwd from r-value reference    test string
1          0       0       0       1       0       struct UserPerfectFwd from r-value reference2   test string
```

The obtained results show that **the most efficient method of receiving string parameters in constructors is by creating a template constructor and perfect-forwarding string parameters to respective data members:**
```cpp
template<StrignConvertable T>
UserPerfectFwd(T&& str) : name(std::forward<T>(str)) { }
```
