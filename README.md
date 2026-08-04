# Pin Viewer (Qt6)

**Pin Viewer** is a lightweight, high-performance image pinning utility built for Linux (specifically optimized for **Wayland**, **Hyprland**, and **GNOME**). 

It allows you to view images from standard input (`stdin`) in a frameless, floating window—perfect for piping screenshot tool outputs directly onto your desktop.

### 1. Build the Docker Builder Image
```
docker build -t pin-viewer-qt6-builder .
```

### 2.Compile the Project
```
docker run --rm -v "$(pwd)":/usr/src/app pin-viewer-qt6-builder
```

### 3.Install to Local Path

```
cp pin_viewer ~/.local/bin/
```

### Usage Examples

```
cat image.png | pin_viewer
# or
pin_viewer < image.jpg

grim -g "$(slurp -w 0)" - | pin_viewer
```
