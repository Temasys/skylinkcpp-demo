# SkylinkCpp demos

This repository contains demos for the Skylink C++ SDK.

## dependencies

Install SDL2 via
```
sudo apt-get install cmake libsdl2-dev
```

Install other dependencies via
```
bash script/init.sh
```

## Project setup

```
mkdir build
cd build
cmake ..
```

## build

```
cd build
make
```

## Running the demo

From the build folder, run :
```
skylinkcpp -k {appKey} -s {secret} -r {roomName} [-a {sendAudio:true/false}] [-v {sendVideo:true/false}] [-l {skylinLogLevel}] [-w {webrtcLogLevel}]
Log levels are: TRACE, INFO, WARN, ERROR, NONE.
```

## Getting an application key

Please visit [temasys.io](https://temasys.io/) for information about the platform, and [console.temasys.io](https://console.temasys.io/) to get an application key.

## Compatibilty

As of today, Temasys only provides a build version for linux.\n
We build and test with Ubuntu 16.04. Your mileage may vary on different versions of linux.

Future builds for Mac and Windows are planned for the near future.

## Known issues

The current demo requires specific pulse audio setting. This is an app level issue.
To render the remote audio properly, edit the pulse audio configuration `sudo vim /etc/pulse/daemon.conf`, and add:
```
default-fragments = 5
default-fragment-size-msec = 2
```
