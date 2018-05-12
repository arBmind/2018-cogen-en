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
        name: "coyield"
        Depends { name: "cpp17" }
        files: [
            "CoYield.cpp",
            "CoYield.h",
        ]

        Export { Depends { name: "cpp17" } }
    }
    Application {
        name: "coyield_test"
        consoleApplication: true
        Depends { name: "coyield" }
        files: [
            "CoYieldTest.cpp",
        ]
    }

}
