<p allign="center">
  <img src="img/akane.jpg">
  <h1>soinjector</h1>
</p>

this is a simple linux library injector that allows you to inject a shared library into a running process. it uses `ptrace` to attach to the target process, call `dlopen` with the specified library path, and detach from the process.

## usage
to use the injector, you must have root privileges. you can run the program with the following command:
```
sudo ./injector <lib_path> <pid>
```
where <lib_path> is the path to the shared library you want to inject and <pid> is the process ID of the target process.<br><br>
example:
```
sudo ./inject /path/to/libexample.so 1234
```
the injector will attach to the process, call `dlopen` with the specified library path, and detach from the process. if successful, it will print `[*] library injected successfully!` to the console.

## docs
the documentation of this tool is [here](https://github.com/meth1337/soinjector/wiki).

  ---
  ```
  [x0@0x0-80tx ~]$ sudo ./injector example.so 1076
  [sudo] password for x0:
  [*] dlopen memory address: 0x7fb735064cd0
  [*] found library path: example.so
  [*] library injected successfully!
  ```
