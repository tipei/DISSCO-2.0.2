FROM ubuntu:16.04

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8
ENV LANGUAGE=C.UTF-8
ENV DISPLAY=$DISPLAY
ENV DEBIAN_FRONTEND=noninteractive

RUN echo 'tzdata tzdata/Areas select Europe' | debconf-set-selections && \
    echo 'tzdata tzdata/Zones/Europe select London' | debconf-set-selections && \
    echo 'keyboard-configuration keyboard-configuration/layout select "English (US)"' | debconf-set-selections && \
    echo 'keyboard-configuration keyboard-configuration/layoutcode select us' | debconf-set-selections && \
    echo 'keyboard-configuration keyboard-configuration/model select Generic 105-key (Intl) PC' | debconf-set-selections && \
    echo 'keyboard-configuration keyboard-configuration/variant select ' | debconf-set-selections

RUN apt-get update -y && \
    apt-get install -y \
        cowsay \
        g++ \
        gdb \
        git \
        premake4 \
        libmuparser-dev \
        libmuparser2v5 \
        libxerces-c-dev \
        libsndfile1-dev \
        libsndfile1 \
        lilypond \
        libgtkmm-2.4-1v5 \
        libgtkmm-2.4-dev \
        libgtk2.0-dev \
        make \
        xterm \
        x11-apps \
        dbus-x11 \
        vim \
        locales && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN locale-gen en_US.UTF-8 && update-locale

RUN service dbus start

ENTRYPOINT ["/bin/bash"]
