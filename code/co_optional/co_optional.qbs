import qbs

Project {
    minimumQbsVersion: "1.7.1"

    Product {
        name: "cpp17"

        Export {
            Depends { name: "cpp" }
            cpp.cxxLanguageVersion: "c++17"
            cpp.cxxFlags: {
                if (qbs.toolchain.contains('msvc')) return ["/await", "/permissive-"];
                if (qbs.toolchain.contains('clang')) return ["-fcoroutines-ts"];
            }
            cpp.cxxStandardLibrary: {
                if (qbs.toolchain.contains('clang')) return "libc++";
            }
            cpp.staticLibraries: {
                if (qbs.toolchain.contains('clang')) return ["c++", "c++abi"];
            }
        }
    }

    StaticLibrary {
        name: "CoOptional"
        Depends { name: "cpp17" }

        files: [
            "CoOptional.cpp",
            "CoOptional.h",
        ]

        Export {
            Depends { name: "cpp17" }
        }
    }

    Application {
        name: "optional_app"
        consoleApplication: true
        Depends { name: "CoOptional" }
        files: [
            "main.cpp",
        ]
    }
}
