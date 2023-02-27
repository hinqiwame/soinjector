# soinjector
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
the injector will attach to the process, call dlopen with the specified library path, and detach from the process. if successful, it will print `[*] library injected successfully!` to the console.
