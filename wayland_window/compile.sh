# wayland-scanner private-code <../../../../../../usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml> xdg-shell-protocol.c
# wayland-scanner client-header <../../../../../../usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml> xdg-shell-client-protocol.h

# /home/osten/Documents/RandomSideCodes/RandomSideQuests

time gcc wayland_window/main.c wayland_window/xdg-shell-protocol.c -o testing -g -O0 -Wall -lwayland-client
