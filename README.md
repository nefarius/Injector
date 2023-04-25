<img src="assets/NSS-128x128.png" align="right" />

# Injector

DLL injection/ejection command-line utility.

![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/nefarius/Injector/msbuild.yml) [![GitHub All Releases](https://img.shields.io/github/downloads/nefarius/Injector/total)](https://somsubhra.github.io/github-release-stats/?username=nefarius&repository=Injector)

## About

Inject any DLL into any running process with ease! Injector is a command line tool abstracting the messy injection process away from you. It can't be more simple:

`Injector.exe --process-name notepad.exe --inject C:\Temp\myHooks.dll C:\Temp\myOverlay.dll`

You may use it in you post-build events in Visual Studio to save time and take away complexity of code by "outsourcing" the injection process. You may of course use it for any othe scenario which comes on your mind. Check out the possible command line arguments:

- `-n|--process-name` identifies the target process by its module name
- `-w|--window-name` identifies the target process by its main windows name
- `-p|--process-id` identifies the target process by its PID
- `-i|--inject` or `-e|--eject` specifies the action to perform (inject or eject the DLL)

Then simply append one or more absolute (or relative) path(s) of the DLL to be in-/ejected.

Invoke `Injector.exe -h` for help.

## Credits

Many thanks to [Joshua "Cypher" Boyce](http://www.raptorfactor.com/) who initially wrote the injection-classes and shared them with the world.

[Argh! by Adi Shavit](https://github.com/adishavit/argh) is used to parse the command-line arguments.

## Downloads

Head over to [Releases](https://github.com/nefarius/Injector/releases/latest) to download x86 and x64 binaries.
