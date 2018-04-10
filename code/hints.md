

## Generate Visual Studio Project

Custom Build Step:

* Command: `C:/Qt/qtcreator-4.6.1/bin/qbs.exe`
* Arguments: `generate -g visualstudio2017 -d %{buildDir}\msvc --settings-dir %{Session:NativePath}\qbs\1.11.0 config:%{CurrentBuild:Name}`
* WorkingDirectory: `%{sourceDir}`

Clean Filters Step:

* Command: `C:/Windows/System32/cmd.exe`
* Arguments: `/c del %{buildDir}\msvc\*.filters`
