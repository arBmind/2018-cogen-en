import qbs

Project {
    minimumQbsVersion: "1.7.1"

    Product {
        name: "cpp17"

        Export {
            Depends { name: "cpp" }
            cpp.cxxLanguageVersion: "c++17"
            // cpp.cxxFlags: {
            //     if (qbs.toolchain.contains('msvc')) return ["/await", "/permissive-"];
            //     if (qbs.toolchain.contains('clang')) return ["-fcoroutines-ts"];
            // }
            // cpp.cxxStandardLibrary: {
            //     if (qbs.toolchain.contains('clang')) return "libc++";
            // }
            // cpp.staticLibraries: {
            //     if (qbs.toolchain.contains('clang')) return ["c++", "c++abi"];
            // }
        }
    }

    StaticLibrary {
        name: "generator"
        Depends { name: "cpp17" }
        files: [
            "Generator.cpp",
            "Generator.h",
        ]

        Export { Depends { name: "cpp17" } }
    }
    Application {
        name: "generator_test"
        consoleApplication: true
        Depends { name: "generator" }
        files: [
            "GeneratorTest.cpp",
        ]
    }

    StaticLibrary {
        name: "parser"
        Depends { name: "generator" }

        files: [ "Parser.cpp", "Parser.h" ]

        Export {
            Depends { name: "generator" }
        }
    }
    Application {
        name: "parser_test"
        consoleApplication: true
        Depends { name: "parser" }
        files: [
            "ParserTest.cpp",
        ]
    }

}
