GuitarButtons v0.3
===========
Arduino firmware re-enabling the Squier Stratocaster guitar buttons with the XBox 360 MIDI Pro Adapter.

## Background
The now-discontinued Squier Stratocaster controller for Rock Band 3 is a *real* guitar with 132 sensors to detect any combination of the 6 strings held to the 22 frets and additional sensors for strum detection. The same guitar can be used for Wii and Playstation 3, since a Midi Pro Adaptor specific to each console connects to the Guitar. The cool thing is that Rock Band accessories use the MIDI standard, allowing the Squier to be connected to any MIDI device, the use of the Pro Keyboard accessory as a real keyboard, the use of a real keyboard as a game controller, or game play with real drums possessing MIDI output.

This modification is neccesarry because Microsoft disallowed the functionality of Xbox controlling buttons on the guitar. The guitar still sends the MIDI signals (custom SYSEX messages), and the buttons work when used with other consoles. The added arduino is wired to the MIDI input of the adaptor and to each of the adaptors controller buttons. The correct button is pressed when the MIDI signal for it is detected.

## Making the mod
For a clean-looking result, you need an Arduino board which will fit in the Midi Pro Adaptor. I used the [Arduino Pro Mini 328](https://www.sparkfun.com/products/11114), which fit nicely inside of the adaptor. A [programmer](https://www.sparkfun.com/products/9873) is also necessary.

There are a few pictures at http://www.docsrockbandmods.net/viewtopic.php?f=3&t=4639. I took some of my own which I may put up later.

## Credits
I originally found this mod, and accompanying code from the forum at http://www.docsrockbandmods.net/viewtopic.php?f=3&t=4639. My version is derived from the work of user "Big Red", and Warren Ondras.

## TODO
* Allow swapping the Up/Down buttons during use
* Change or allow changing the Forward/Back for Xbox button to another button, because the Forward/Back macro also sends an MIDI off signal for every sensor, and also releases any currently held controller buttons.
