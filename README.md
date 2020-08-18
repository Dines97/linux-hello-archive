# Linux Hello

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
sudo apt install g++ libpam0g-dev libopencv-dev libdlib-dev libboost-program-options-dev nlohmann-json3-dev libatlas-base-dev libsqlite3-dev
```
#### Arch
```

```
#### openSUSE
```

```
#### Fedora
```

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

