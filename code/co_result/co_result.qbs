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
        name: "CoResult"
        Depends { name: "cpp17" }

        files: [
            "CoResult.cpp",
            "CoResult.h",
        ]

        Export {
            Depends { name: "cpp17" }
        }
    }

    Application {
        name: "result_app"
        consoleApplication: true
        Depends { name: "CoResult" }
        files: [
            "main.cpp",
        ]
    }
}
