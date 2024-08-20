# Xorg root Daemon


Simple script that give you realtime datetime and battery capacity for X Window.

simulating `xsetroot -name "text"`

example:
<img src="https://github.com/fadhil-riyanto/xrootd/blob/master/scr_img/87.png?raw=true">

# Usage & build

<pre>
git clone https://github.com/fadhil-riyanto/xrootd.git
cd xrootd
mkdir build
cd build
cmake ..
make -j4
./xsetrootd
</pre>

or install
<pre>
sudo make install
xsetrootd
</pre>

build dependencies:
- X11
- gdk-pixbuf
- glib-2.0
- libnotify

## Contributing
Your contributions are always welcome! 

## Maintainer
[Fadhil Riyanto](https://github.com/fadhil-riyanto)

## License
GPL-3.0

