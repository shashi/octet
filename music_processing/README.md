This folder contains the processing code that is a slight modification of
what was used towards the end of Technites 2012. Need to make this much much better.

You will need to set the path of the music file to play: this requires a GUI, yes.

Ideally, the music processing should work in isolation, sending signals to all sorts
of gadgets in the psych room.

This means putting processing in its own class: MusicProcessor - this calls
MusicEventHandlers' methods as events occur in music.

An interface, MusicEventHandler should provide a structure for listeners of events
raised by MusicProcessor.

TODO
====
What are some good events?

Taking audio input from a mic is shitty, and just FFT is not stimulating to the mind.
