## Goal of the project
The aim of this project is to make a simple handheld device (with controls similar to a GameBoy advance).
It should run games that a user can easily copy into an sd card.
The user should be mostly free to do what they want (as it pertains to their game) with minimal conditions:
- User should define a single entry point
- Project should supply user with memory.
- Project should provide basic libraries for input/audio/rendering

The main idea is that this project should be a relatively simple launchpad for people to experiment 
with game programming, low level learning, and embedded systems. Ideally a user who's interested in these topics would start 
off by building a simple game, then eventually start playing with the firmware itself. 

To aid with this aim the code should prioritise simplicity and clarity over performance (within sane limits of course).

This project takes inspiration from the goals of [Raylib](https://www.raylib.com/index.html), and the provided
API's should strive for similar usage and simplicity.

## Roadmap
- [ ] hardware selection
- [ ] prototype schematic
- [ ] local build setup (makefile, vendor tools, etc)
- [ ] initial bring up (blinky)
- [ ] automatic testing 
- [ ] HIL automatic testing (learn how it works and decide if necessary)
- [ ] button input driver (GPIO)
- [ ] display driver
- [ ] audio driver
- [ ] sd card integration
- [ ] user facing input API
- [ ] user facing render API
- [ ] user facing audio API
- [ ] pcb schematic v1
- [ ] iterate on final schematics
