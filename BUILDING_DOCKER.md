Building for Docker 
=================

Prerequisites
-------------

Of course, you must install Docker (either Desktop or command line is fine).

To run the LASSIE GUI, you'll need an X11-compatible display server.

Installing requirements and recommendations on:
-----------------------------------------------

### Linuxes
Unofficial and official Docker packages are equivalent for our purposes.

You probably already have an X11 display server (even if you're not using X11)!

### Not-Linux
Docker provides official installations for Windows and macOS [here](https://www.docker.com/products/docker-desktop/).

If you're on Windows or macOS, you must install a display server compatible with X11, like Xming (freeware) or [VcXsrv](https://sourceforge.net/projects/vcxsrv/) for Windows and [XQuartz](https://www.xquartz.org/) for macOS.

Getting DISSCO
-----------------
Just `git clone` this repo; explicitly:

    git clone https://github.com/tipei/DISSCO-2.0.2.git

Building, Running, and Starting the Container
----------------------
You first have to allow X11 to allow local connections from root with

    xhost local:root

In the project's root directory (by default, `/DISSCO-2.0.2/` or `\DISSCO-2.0.2\`) with Docker running (and you may need to run these with `sudo`):

    docker build . -t ubuntu_dissco_202 && docker run --name dissco_202 -v $HOME:$HOME -w $HOME -e DISPLAY=$DISPLAY --net=host -it ubuntu_dissco_202

You will have made a container named `dissco_202` from an image named `ubuntu_dissco_202`. You should be inside the `bash` shell in the container as `root` and can skip to the **Building** section of [`BUILDING_LINUX.md`](BUILDING_LINUX.md#building) to continue.

If you had previously built DISSCO with `premake4` outside of the container, then you'll need to `premake4 clean` and `premake4` and `make` again so that the appropriate libraries are linked.

If you wish to start the same container again in the future, do `docker start dissco_202 -i`.

Known Issues
------------
(at least on macOS) If you receive the error `Cannot open display` when trying to run LASSIE, try attaching the running container in VS Code and checking `echo DISPLAY`. Try running LASSIE again, in the VS Code terminal. (I'm not sure why this works; it's a hack at the moment.) (You might also consider running the container (i.e., making a new container) with the additional flag `-e DISPLAY="${IP}:0"`. It seems like the number following the `:` goes up by one at some point, so you may try increasing numbers (within reason) until you get a hit.)

For Developers
--------------
Note that the container is off an Ubuntu 16.04 image and thus will display quirks befitting its age. The current effort is to get it working stably on Debian 12.
    
