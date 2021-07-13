# WIP: Linux Hello :WIP

Linux Hello provides facial recognition based authentication for Linux. Use your built-in IR emitters and camera to prove who you are. 

Using the central authentication system (PAM), this works everywhere you would otherwise need your password: Login, lock screen, sudo, su, etc.

## Installation
#### Ubuntu (20.04)
```
```
#### Arch
```
```
#### openSuse
```
```
### Fedora
```
```
## Building
### Dependencies
#### Ubuntu (20.04)
```
sudo apt install g++ libpam0g-dev libopencv-dev libdlib-dev libboost-program-options-dev nlohmann-json3-dev libatlas-base-dev libsqlite3-dev libcereal-dev
```
#### Arch, Artix, Manjaro, etc.
First, install the dependencies on the official repo.
```
Sudo pacman -Sy gcc cmake pam opencv cereal
```
Then, install the dlib AUR package.
```
git clone https://aur.archlinux.org/dlib.git
cd dlib
makepkg -si
```
#### openSUSE
```

```
#### Fedora
```
sudo dnf -y install make cmake gcc gcc-c++ pam pam-devel opencv-devel dlib-devel
```
### Compilation
```
git clone https://github.com/Dines97/Linux-Hello.git
cd Linux-Hello
mkdir build/
cd build/
cmake ..
sudo make install
```

