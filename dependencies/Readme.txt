edepbuild

The edepbuild command is designed easily to configure and build Echo's
dependencies for your platform.

useage:

edepbuild targetName [singlelibrary]

A directory will be created named targetName for your target and all library
sources will be copied to it.

The script will look for targetName.config to give you the chance to configure
environment variables for the configure script. If the file does not exist a
default is created.

You can optionally set CFLAGS, CPPFLAGS and LDFLAGS before you execute
edepbuild to automaticially include these within the targetName.config file.
NOTE: This only applies to new platforms. Existing targetName.config files
will remain unchanged.

If you require additional flags to be set before the configure script edit
the targetName.config.

For extra library specific flags you can override the targetName.config file
by creating a targetName-libraryName.config file the library name must match
the library folder name.

You can tell the script not to configure, make and/or make install by using
a combination of the following flags the following command.

NO_CONFIGURE=1 ./edepbuild
NO_MAKE=1 ./edepbuild
NO_MAKE_INSTALL=1 ./edepbuild

NO_CONFIGURE=1 NO_MAKE=1 NO_MAKE_INSTALL=1 ./edepbuild

The final example just sets up the folder structure and not configure, make
or make install. This can actually be achieved by using the following.

ONLY_SETUP=1 ./edepbuild

The script checks for existing library directories to minimise copying of
data. You can override this and force the libraries to be copied using the
following

FORCE_LIB_COPY=1 ./edepbuild

Directory variables
ECHO_DEP_ROOT_DIR is the root directory for edepbuild (the executing directory) is absolute.
ECHO_DEP_SCRIPTS_DIR points to the scripts directory and is absolute.
ECHO_DEP_BUILD_DIR , `scripts` and `build` directories respectively. All are absolute paths.
ECHO_LIB_DIR specifies the dependency library directory. Default is $ECHO_DEP_ROOT_DIR/opt
ECHO_X_DIR specifies the [unbuilt] library source directories. Default is X.

You can execute ./edepbuild without a targetName for this information in summary.
