Injector
========

Inject any DLL into any running process with ease! Injector is a command line tool abstracting the messy injection process away from you. It can't be more simple:

`Injector.exe --process-name notepad.exe --module-name C:\Temp\myHooks.dll --inject`

You may use it in you post-build events in Visual Studio to save time and take away complexity of code by "outsourcing" the injection process. You may of course use it for any othe scenario which comes on your mind. Check out the possible command line arguments:

* `--process-name` identifies the target process by it's module name
* `--window-name` identifies the target process by it's main windows name
* `--process-id` identifies the target process by it's PID
* `--module-name` sets the absolute (or relative) path of the DLL to be in-/ejected
* `--inject` or `--eject` specifies the action to perform (inject or eject the DLL)

Yeah you read right: you can also eject any library with the `--eject` action.

Many thanks to [Joshua "Cypher" Boyce][raptor] who initially wrote the injection-classes and shared them with the world. 

  [raptor]: http://www.raptorfactor.com/  "The Raptor Factor"

Download
--------
Head over to [Releases](https://github.com/nefarius/Injector/releases/latest) to download x86 and x64 binaries.
