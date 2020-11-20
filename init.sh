#!/bin/bash -eux
#git clone --recursive https://github.com/ossia/libossia

LIBOSSIA=/home/jcelerier/score/3rdparty/libossia

rm -rf Source/libossia/Private/ossia
cp -rf $LIBOSSIA/src/ossia Source/libossia/Private/

cp -rf $LIBOSSIA/3rdparty/fmt/include/fmt Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/spdlog/include/spdlog Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/hopscotch-map/include/tsl Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/SmallFunction/smallfun/include/smallfun.hpp Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/flat/include/flat Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/flat/include/flat Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/GSL/include/gsl Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/variant/include/eggs Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/brigand/include/brigand Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/rapidjson/include/rapidjson Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/oscpack/oscpack Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/websocketpp/websocketpp Source/libossia/Private/ 
cp -rf $LIBOSSIA/3rdparty/Servus/servus Source/libossia/Private/ 

cp -rf $LIBOSSIA/3rdparty/flat_hash_map/*.hpp Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/nano-signal-slot/include/* Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/readerwriterqueue/*.h Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/concurrentqueue/*.h Source/libossia/Private/

cp -rf $LIBOSSIA/3rdparty/asio/asio/include/asio Source/libossia/Private/
cp -rf $LIBOSSIA/3rdparty/asio/asio/include/asio.hpp Source/libossia/Private/
rm Source/libossia/Private/asio/impl/src.cpp

cp -rf $LIBOSSIA/3rdparty/boost_1_73_0/boost Source/libossia/Private/
rm Source/libossia/Private/boost/asio/impl/src.cpp

(
cd Source/libossia/Private/ossia
rm -rf dataflow editor gfx audio
rm -rf network/artnet 
rm -rf network/joystick
rm -rf network/leapmotion
rm -rf network/libmapper
rm -rf network/midi
rm -rf network/minuit
rm -rf network/phidgets
rm -rf network/wiimote
)