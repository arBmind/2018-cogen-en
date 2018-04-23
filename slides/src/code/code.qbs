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

    Application {
        name: "array"
        consoleApplication: true
        Depends { name: "cpp17" }
        files: [
            "array/main.cpp",
        ]
    }

    Application {
        name: "from_gen"
        consoleApplication: true
        Depends { name: "cpp17" }
        files: [
            "from_gen/main.cpp",
        ]
    }

    Application {
        name: "cogen"
        consoleApplication: true
        Depends { name: "cpp17" }
        files: [
            "cogen/main.cpp",
        ]
    }
}