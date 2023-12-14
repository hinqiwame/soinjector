<p allign="center">
  <img src="img/akane.jpg">
  <h1>Soinjector</h1>
  <div>
    <img src="https://img.shields.io/badge/license-unlicense-green">
    <img src="https://img.shields.io/github/languages/code-size/meth1337/soinjector">
    <img src="https://img.shields.io/badge/language-C-lightgrey">
  </div>
</p>

This is a simple Linux library injector that allows you to inject a shared library into a running process. It uses `ptrace` to attach to the target process, call `dlopen` with the specified library path, and detach from the process.

## Download
You can get the latest stable binary from [here](https://github.com/meth1337/soinjector/releases/latest) (filename: `injector`)

## Usage
To use the injector, you must have root privileges. You can run the program with the following command:
```
sudo ./injector <lib_path> <pid>
```
Where `<lib_path>` is the path to the shared library you want to inject and `<pid>` is the process ID of the target process.<br><br>
Example:
```
sudo ./inject /path/to/libexample.so 1234
```
The injector will attach to the process, call `dlopen` with the specified library path, and detach from the process. If successful, it will print `[*] Library injected successfully!` to the console.

## Docs
The documentation of this tool is [here](https://github.com/meth1337/soinjector/wiki).

  ---
  ```
  [x0@0x0-80tx ~]$ sudo ./injector libexample.so 1952
  [*] Dlopen memory address: 0x7fa06a5b0700
  [*] Found library path: libexample.so
  [*] Library injected successfully!
  [*] Resumed the target process: 1952
  ```
