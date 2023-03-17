# LKM SAMPLES
For vscode with `C/C++ extension`, it's recommended to add linux-headers to `includePath` to activate C/C++ IntelliSense in kernel module src file. A possible configuration file `./.vscode/c_cpp_properties.json` may look like:

```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/src/linux-headers-5.15.0-67-generic/**"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c17",
            "cppStandard": "gnu++17",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
```
Note that there is no need to add system include path (eg. /usr/include) to the extension's includePath if `compilerPath` is settled.
