# soinjector
this is a simple linux library injector that allows you to inject a shared library into a running process. it uses `ptrace` to attach to the target process, call `dlopen` with the specified library path, and detach from the process.
