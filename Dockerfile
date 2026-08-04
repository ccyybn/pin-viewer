FROM archlinux:latest

RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm \
        gcc \
        make \
        cmake \
        pkgconf \
        qt6-base \
        qt6-wayland \
        qt6-svg \
    && pacman -Scc --noconfirm

WORKDIR /usr/src/app

ENTRYPOINT ["bash", "-c", "g++ -O2 -fPIC $(pkg-config --cflags Qt6Widgets) main.cpp -o pin_viewer $(pkg-config --libs Qt6Widgets)"]
