Lenzhound
=========

##Building
First, make sure you have [Arduino 1.6](http://arduino.cc/en/main/software) installed

And that the `arduino` command in the console ends up pointing to the
1.5.x `arduino` executable. (This is tricky and platform-dependent and
probably deserves its own guide.)

And install [CMake](http://www.cmake.org/download/)

###Linux
Clone the repo:
```
git clone https://github.com/MotionDogs/Lenzhound.git
```

Build:
```
cd Lenzhound/
cmake . -DARDUINO=</absolute/path/to/arduino>
make
```

Test:
```
make run-tests
```

Plug in the transmitter unit and call:
```
make upload-txr
```

Then plug in the receiver unit and call:
```
make upload-rxr
```

And you're set!

###Windows/OSX
Coming soon...

##License
This program is open source software: you can redistribute it and/or
modify it under the terms of the GNU General Public License v3.0 as published
by the Free Software Foundation.
