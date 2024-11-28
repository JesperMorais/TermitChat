# Project info
This project aims to make a fully terminal driven chat written in c++.

Currently Using ftxui for Terminal UX and Asio for network. Will add MQTT support later on

## FOR DEVS - HOW TO BUILD PROJECT

## SET `VCPKG_ROOT` ENV VERIBLE

TO BUILD THIS POOJECT YOU NEED TO SET "VCPKG_ROOT" as ENVIORMENT VERIOBLE after you have installed **vcpkg** on your system. [vcpkg][https://github.com/microsoft/vcpkg]. This will alow AUTOMATIC building with correct tool chain.

### Windows

#### Method 1: Use cmd

1. **Open CMD** as admin:
   -Open CMD as ADMIN

3. **SET `VCPKG_ROOT` VERIBLE** with the following command:

   ```cmd
   setx VCPKG_ROOT "C:\path\to\vcpkg" /M

*This should be set to the ROOT of vcpkg.*

#### Method 2: Create in manually
- Simple go into ENV Veribles, click "new" and fill in name "VCPKG_ROOT" and the path "C:\path\to\vcpkg" and click "ok".
- You should see the new verible be in the list of other veribles

### BUILDING THE PROJECT

## Create `vcpkg.json` 

The Project uses **vcpkg** to handle packages. There for you need a `vcpkg.json` in the projects Root:

do the follow command in the terminal to add he libs needed: 

```vcpkg add port asio```

```vcpkg add port ftxui```

```vcpkg add port paho-mqtt```

```json
{
  "name": "termitchat",
  "version-string": "0.0.1",
  "dependencies": [
    "asio",
    "ftxui"
  ]
}
```

After its done type:
vcpkg install in the terminal ( IN THE ROOT ), this should start installing all the needed packages

## Time to BUILD!

To build Build map: Root/ *"cmake -B build -S ."*   ***

To compile The whole project Root/ *"cmake --build build"*

## TO RUN SERVER/CLIENT

to run specific part: 

Server: ./build/server/debug/server

Client: ./build/client/debug/client

TODO: ADD BETTER/EASIER WAY TO RUN EACH

### DEBUG / BUILD ISSUES

1. Normal Issues can be fixed with removal of BUILD map and rebuild project.


### KNOWN ISSUES
1. Opening the project and trying to build provides error. Fixed by rebuildning the build map

