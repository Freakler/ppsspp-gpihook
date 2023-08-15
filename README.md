# ppsspp-gpihook
Emulates GPI switches normally found on developer hardware only. 

Sometimes developers have hidden functionality behind these switches (especially with early builds) in their games. This can be everything from on-screen info to unlocking debug menus!


## Installation
Place all files from release in ```/PSP/PLUGINS/gpihook```


## How to use
Adjust the ```/PSP/PLUGINS/gpihook/config.ini``` values where each key represents the respective pyhsical switch.
```
[GPI]
0 = FALSE
1 = TRUE
2 = TRUE
3 = TRUE
4 = FALSE
5 = FALSE
6 = TRUE
7 = TRUE
```
This works while the game is running and takes effect after a couple milliseconds! You can take a look at the log file to see live if and when a game checks for switch settings.


## Examples

### Killzone Liberation (UCET-00278 v0.2)
Setting switch 1 to TRUE enables a small menu where you can select between FPS and memory display options.
![ref0](https://github.com/Freakler/ppsspp-gpihook/blob/master/capture_000.png)

### Tony Hawk's Underground 2 (ULUS-10014)
![ref1](https://github.com/Freakler/ppsspp-gpihook/blob/master/capture_001.png)


thanks to qwikrazor87 & Peter Lustig and greetings to the UMDatabase Discord