# Cricket Audio Delay Project

## About
This code delays audio for a configurable amount of time, at a high enough bit-depth and sampling rate for AM radio. Currently the delay is set to be around 5 seconds, which is sufficient for the nominal delay between 774 ABC Melbourne's cricket broadcast, and Channel 9's vision.

## Requirements
1. Arduino (mine is an Arduino Uno R3)
2. [Nootropic Audio Hacker Shield](http://nootropicdesign.com/audiohacker/)
3. An AM radio source
4. 3.5mm audio cables

## Known Issues
1. Not sure if the delay is entirely accurate, thought it is < 0.5 seconds.
2. The samples seem to be played slower than they are recorded for the first few seconds. Unclear why
3. The formula for determining the size of the memory should include reference to sampleRate. Currently it does not, and instead has a figure of 20kHz instead of 30kHz to make it work. Need to determine why this is the case.

## Roadmap
Add an LCD shield and digital pot to the circuit, which would allow dynamically changing the delay, and showing what the current delay is.