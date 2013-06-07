Injector
========

Inject any DLL into any running process with ease! Injector is a command line tool abstracting the messy injection process away from you. It can't be more simple:

`Injector.exe --process-name notepad.exe --module-name C:\Temp\myHooks.dll --inject`

You may use it in you post-build events in Visual Studio to save time and take away complexity of code by "outsourcing" the injection process. You may of course use it for any othe scenario which comes on your mind. Check out the possible command line arguments:

* `--process-name` identifies the target process by it's module name
* `--window-name` identifies the target process by it's main windows name
* `--process-id` identifies the target process by it's PID
* `--module-name` sets the absolute path of the DLL to be in-/ejected
* `--inject` or `--eject` specifies the action to perform (inject or eject the DLL)

Yeah you read right: you can also eject any library with the `--eject` action.

Many thanks to [Joshua "Cypher" Boyce][raptor] who initially wrote the injection-classes and shared them with the world. 

  [raptor]: http://www.raptorfactor.com/  "The Raptor Factor"

Download
--------
* [Injector (x86)][injector86]
* [Injector (x64)][injector64]

  [injector86]: http://nefarius.at/wp-content/uploads/2013/06/Injector_x86.7z
  [injector64]: http://nefarius.at/wp-content/uploads/2013/06/Injector_x64.7z
