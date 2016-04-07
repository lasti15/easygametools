Check out the source for EGT. You'll need the source for RakNet and the include folder from the Irrlicht source. Put both the raknet source and irr includes into the includes folder of the project root.

You also need to set your build path to include the project lib & include folders.

Lastly, you'll need to add the ${project}/protoc/cpp directory to your include path as well.

As much as I would love to make this platform independent I develop on a windows machine so I tend to use win32 libraries.

Currently I have only tested this in Windows 7 64-bit, though it should be compiling to a 32 bit binary.

The server will need mysql setup and configured on the local environment. I need to do some work on the poms to not point at my personal maven server, so if you would like to help with that please do :)