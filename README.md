# Daz To Unity

This is the same code base as the [official](https://github.com/daz3d/DazToRuntime/) open sourced version with the following modifications:

- The default use `\\` in paths which is not cross platform compatible. In Qt world you can use forward slash on any platform as Qdir takes care of handling the right separator itself.

I plan to use this repository as a fork so I might diverge with master in the future.