


## My DWM Build
This is my build of the linux window tiling window manager dwm, the application launcher dmenu, and the status bar dwmstatus. I have included the terminal st as well, although it is optional to use and the dwm config.h file must be changed to use it. All of these 
are from the suckless software utilities and are tailored 
to my own personal usage on an Arch Linux minimal install. 

The original source code without my modifications 
can be found here : https://git.suckless.org/dwm/

## Changes from Original
* dmenu
  * gruvbox colorscheme
* dwm 
  * transparency support
  * gruvbox colorscheme
  * uses UbuntuMono fonts
  * larger border pixels and snap 
  * changed layout names for clarity to T for tiling and F for float
* dwmstatus
  * tracks cpu load
  * tracks ram load
  * tracks laptop battery
  * tracks mouse battery
  * displays date in local time with 12 hour formatting

* st
  * Transparency support
  * Scrolling
  * gruvbox colorscheme


![desktop2](https://user-images.githubusercontent.com/70598503/114765291-fccab200-9d32-11eb-9bea-8861ee2402ee.png)
## Installation
Package names will depend on system. You will need the xorg c libraries to compile. For apt, install the following dependencies. If you are not on Ubuntu or one of its deriviatives please consult your package manager to find the name for the UbuntuMono font. You will need that as well. 

```
sudo apt install xorg libxft-dev libx11-dev libxinerama1 lib xinerama-dev build-essential fonts-font-awesome
```

Then clone the repository
```
git clone https://github.com/C-Loftus/dwmBuild.git
```
cd into the directory where you cloned it. Run the following command to compile and install binaries for DWM, dmenu, and st all at once.
```
sudo make all
```

If you ever want to only install one, simply cd into its directory and run this instead. This will only instead the program that is in the given directory. (for instance, it would only install st if ran in the st directory)
```
sudo make clean install
```

If you want to install my dwmstatus monitor as well
```
cd dwmstatus
sudo ./install.sh
```
For more complex build procedures please consult the makefile in each directory.

## Running
DWM needs a way to start on login. On a minimal Linux installation, the easiest way is to do this is to add DWM to you xinitrc
If you do not have an xinitrc, I have provided a sample one in the repository. Be careful, as running cp on this will overwrite your earlier .xinitrc
```
cd XServerFiles
cp .copyOfxinitrc ~/.xinitrc
```
If you using a desktop manager like XFCE and want to use my DWM config alongside it, it may be advisable to place a config for DWM in you xsessions folder. This will allow you to switch which environment you want on log in. 

```
cd XServerFiles
cp DWM.desktop /usr/share/xsessions/DWM.desktop
```
## Configuring
If you want terminal transparency, you will need to install a compositor like Compton, xcompmgr, or picom. I have not done this since it creates bugs with screen sharing applications like zoom.

If you want to change which terminal DWM uses, do the following from where you cloned the repository. Vim can be any text editor of choice
```
cd dwmbuild/dwm
vim config.h
```
Change terminal on line 64 from "xfce4-terminal" to your terminal name. For example, "gnome-terminal" or "st". Then recompile just dwm. You do not need to recompile dmenu or st. In the dwm directory run this command
```
sudo make clean install
```

## Changing Colorscheme
In the photo provided, I am using the gruvbox colorscheme on my terminal. This is handled by the terminal, which is xfce4-terminal for me, and not DWM. I have only included color schemes for DWM and st, not other terminals since there are too many too possibly handle all in this repository. 

## Help
In this repository there is a text file containing all the basic commands one would need to know to run DWM. 

## License
Redestributed and modified with permission according to the MIT license. 
[MIT](https://choosealicense.com/licenses/mit/)
