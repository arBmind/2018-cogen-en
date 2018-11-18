
## Qbs Build System

My projects use the qbs build system.

See: https://qbs.io or https://github.com/qbs/qbs


## Generate Visual Studio Project

If you use Qt Creator see my blog post:
https://blog.hnhs.de/posts/2018/07/19/generate-visual-studio-project-for-a-qbs-project-in-qt-creator/

Custom Build Step:

* Command: `C:/Qt/qtcreator-4.6.1/bin/qbs.exe`
* Arguments: `generate -g visualstudio2017 -d %{buildDir}\msvc --settings-dir %{Session:NativePath}\qbs\1.11.0 config:%{CurrentBuild:Name}`
* WorkingDirectory: `%{sourceDir}`

Clean Filters Step:

* Command: `C:/Windows/System32/cmd.exe`
* Arguments: `/c del %{buildDir}\msvc\*.filters`
