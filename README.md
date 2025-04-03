# Installation

### Ubuntu 20.04 (focal)
There is an apt repository available:
```
wget -qO - https://www.emblem.net.au/apt/emblem-pgp-key.public | sudo apt-key add -
echo "deb [arch=amd64] https://www.emblem.net.au/apt focal main" | sudo tee /etc/apt/sources.list.d/emblem.net.au.list
sudo apt install echo3-linux-standalone
```

For any other available platform you can replace `linux` with the platform name.

# Building

```
# Clone
hg clone http://developer.emblem.net.au/source/echo3/

By default Echo and dependencies are installed to /opt/echo3 to avoid overwriting

# Build dependencies
cd echo3/dependencies

# Optional set ECHO_INSTALL_DIR for this command
sudo ./edepbuild linux

# Set your echo directory so Echo can find the standalone dependencies
export ECHO_ENGINE_INSTALL_DIR=/opt/echo3

# There is a helper script to target configure for a platform
./cmakeplatform linux

# Build echo
cd build-linux
make
sudo make install

```

NOTE: Echo and the dependencies by default aren't installed to the regular system locations. This is to avoid conflicts with system dependencies. It is possible to customise your build by copying the dependencies/scripts folder that most closely matches your platform (e.g. linux) then configure the dependency builds accordingly. When building dependencies you can specify ECHO_INSTALL_DIR as the target for dependency installation.

Echo installation uses the CMAKE_INSTALL_PREFIX for the installation target as per usual but also needs ECHO_ENGINE_INSTALL_DIR specified to locate the platform dependencies.

# Using the generate project script to generate a project from a template.
```
# Generate a new project from a template
cd ${ECHO_ENGINE_INSTALL_DIR}/templates/cmake
./GenerateProject.sh ProjectName EchoApplicationProject /path/to/create/project
# The target path will result in /path/to/create/project/ProjectName
```

## Building your project through an IDE
You should be able to open vscode and point it to your project folder. It should be able to build correctly with the provided settings file as long as ECHO_ENGINE_INSTALL_DIR is set in your environment.

## Building your project via command line
You can build or run from the command line by executing some commands in the terminal.

There is a helper script generated in the project
```
buildplatform linux
```

To run your project

```
runplatform linux
```

Check out the helper scripts, they are very simple.

## Building for other platforms

Building for additional platforms follows much the same steps above and specifying compiler variables. There is a handy script that you can source to set up your shell to target the platform. The cmakeplatform script helps manage this for you and one is generated for each project.

```
# Build dependencies
cd dependencies
./edepbuild platform

# Build echo
cd ..
cmakeplatform platform
buildplatform platform
cd build-platform
sudo make install
```

NOTE: You can install multiple platforms along side one another. They are put into adjacent folders.
