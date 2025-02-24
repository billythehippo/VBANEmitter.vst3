# VBANEmitter.vst3
VBAN Emitter VST3 plugin

This is a simple cross-platform VBAN Emitter JUCE based VST3 plugin for Linux, Mac OS and Windows, also AU one for Mac OS and LV2 for Linux.

Now it can save IP, port, Streamname, format and redundancy in presets of our DAWs (tested on Reaper and Ardour on Linux as VST3, on Reaper on Mac OS as AU and also on Carla on Linux as LV2.

The SCAN function is for future to find VBANReceptor.vst3 (coming soon) and new versions of multistream VBAN receptors for Jack and Pipewire (also coming soon).

It has been tested only on Linux and Mac OS but we all know about JUCE

To build it on Linux just clone this repo, go to Builds/LinuxMakeFile, open terminal there and type "make"
It will be places to ~/vst3, just move it to directory what you want (if you want).

To build it on Mac OS use xcodebuild (see its docs or ask GPT).

To build it on/for other platforms just ask GPT how to build basic JUCE project.

Roadmap: to test it on Windows, iOS and even on Android.
