# lamada

lamada, the C hooking assistant library, is designed to enhance xposed native module development experience.

## Build

Only clang is supported! Do **NOT** use GNU toolchain.

```
$ ANDROID_HOME=/path/to/ndk TARGET_SDK=33 source setup-android.sh
$ make
$ PREFIX=/usr/local make install
```

## Usage

See examples directory for usage.

### Module lookup

```c
runtime_module_t* module = lamada_lookup_module("libmodule_name.so", OPTIONS);
```

Options are recommended to set to `MODLOOKUP_COMBINED | MODLOOKUP_XONLY`,
which `MODLOOKUP_COMBINED` will combind module fregments around, 
and `MODLOOKUP_XONLY` will filter out modules(segments) that does not has X (execut) flag on permissions field. 

### Function lookup

Lamada supports three ways to lookup a function from `runtime_module_t`.

- Export name: `lamada_lookup_symbol(module, "name_of_function", SYMLOOKUP_BYEXPORTNAME);`
- Pattern: `void* fopen_fn = lamada_lookup_symbol(libasan, "<str pattern>", SYMLOOKUP_BYPATTERN | SYMLOOKUP_STRPATTERN);`
    + The format of pattern follows hex bytes seperated by space, and generic matching is supported by using `??`
- Address

Be caultion while searching with pattern!
You must not search a function pattern through another module which imports that function.
The following example will not work

```c
runtime_module_t* libc = lamada_lookup_module("libc.so", MODLOOKUP_COMBINED | MODLOOKUP_XONLY);
void* fopen_fn = lamada_lookup_symbol(libc, "<fopen pattern>", SYMLOOKUP_BYPATTERN | SYMLOOKUP_STRPATTERN);
```

Instead you must do

```c
runtime_module_t* libasan = lamada_lookup_module("libasan.so", MODLOOKUP_COMBINED | MODLOOKUP_XONLY);
void* fopen_fn = lamada_lookup_symbol(libasan, "<fopen pattern>", SYMLOOKUP_BYPATTERN | SYMLOOKUP_STRPATTERN);
```

However, by export name will work this way

```c
void* fopen_fn = lamada_lookup_symbol(libc, "fopen", SYMLOOKUP_BYEXPORTNAME);
```

This is because fopen is imported from `libasan.so` by `libc.so` which does not contains the actual code.
`SYMLOOKUP_BYPATTERN` will only lookup memory regin within that module.

It is recommended to use at least 32 Bytes of pattern on ARM devices, becuase for almost every C function,
it will begin with something like `stp x30, x21, [sp, -0x10]!`, which means there will be functions
that have the same bytes at beginning. ARMv8a has a fixed length for almost every instruction,
so we can know that 32 Bytes are exectly 8 instructions, which is enough for most scenarios.

## Contribution

Please make sure your code is following the same style.
You may need to use tools like `bear` to get auto complete to work.
