# Injector

DLL injection/ejection command-line utility.

[![Build status](https://ci.appveyor.com/api/projects/status/lhpqy99c5lxjxaxv?svg=true)](https://ci.appveyor.com/project/nefarius/injector) [![Discord](https://img.shields.io/discord/346756263763378176.svg)](https://discord.vigem.org/) [![Website](https://img.shields.io/website-up-down-green-red/https/vigem.org.svg?label=ViGEm.org)](https://vigem.org/) [![PayPal Donate](https://img.shields.io/badge/paypal-donate-blue.svg)](<https://paypal.me/NefariusMaximus>) [![Support on Patreon](https://img.shields.io/badge/patreon-donate-orange.svg)](<https://www.patreon.com/nefarius>) [![GitHub followers](https://img.shields.io/github/followers/nefarius.svg?style=social&label=Follow)](https://github.com/nefarius) [![Twitter Follow](https://img.shields.io/twitter/follow/nefariusmaximus.svg?style=social&label=Follow)](https://twitter.com/nefariusmaximus)

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

## Builds

[Latest automated builds end up here](https://buildbot.vigem.org/builds/Injector/master/); might contain bugs, use with care 😉
