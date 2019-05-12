Kobe
====
An abandoned point and click adventure game for Dos. It implements Vga at 320x240 px (Abrash Mode-X), scrolling effect and a primitive renderer. Compiles to real mode Dos and do not uses more that 640 kb of memory, so technically should work on a original i386 at 16 MHz.

Of course on its unfinished state is more like an engine that a game, not to say that I never tested it outside DosBox.


Features
--------
![screenshot](./readme-images/test1.gif)

Executing `kobe test1` (above). A drawing test for backgrounds using `far` and `huge` pointers, the first type allowing a maximum width of 320 px and 64 kb of size (one memory segment¹); the second one allowing up to 640 kb in size but limited by the Vga memory, in Kobe to a maximum width of 960px².

Executing `kobe test2` ([see gif](./readme-images/test2.gif)) shows the scrolling effect on a 960 px image. Because is hardware accelerated a frame only needs to draw the columns immediately revealed, you can note that the first frame takes time to draw, now the rest are matter of a few milliseconds. Also the game runs at an constantly speed, not matter on what specs runs the scrolling test should always take the same time to complete.

Executing `kobe` ([see screenshot](./readme-images/kobe.png)) its mostly the same but allowing to scroll with the keyboard and drawing an sprite over the background. I was working on sprite support just before put the project on a side, so it is incomplete.

Some reusable modules:
- **Time module** over the PIT. Provides 'get' and 'sleep' functions with millisecond precision. Rather than a busy waiting loop uses the `HLT` instruction. Is independent from video code so it allows to not necessary sync the game logic with the video chip. There are no delta calculation, but it should be easy to implement.
- **Input module**, keyboard support over the PIC, mouse support using interrupts. Nothing fancy but should work, also independent from other code.
- **Image/Sdk modules**, being frankly I have to say that Mode-X is extremely slow to draw anything (except for Carmack or Abrash). So having a 'fast' image format was really useful to load the resources in a single `fread` and drawing them with a few `memcpy` calls, somewhat ignoring the funky planar arrange that the video mode imposes. Of course this is not a requirement to have a fast game (as Wolfenstein 3d show that is possible to draw a real time raytracer using Mode-X³) but think this module and the custom format that reads as a way to avoid the rearrange of upside-down Bmp images during gameplay time. Check the Sdk code, it produces purposely arranged images that the game happily draws without doing anything else⁴. If your idea is to use Mode-13 you still can reuse the Sdk as it provides a left-to-right arrange and Png support.
- **Render module**, only reusable as reference. Is independent from the rest of the code and do not include hardcoded corners (not much), but is somewhat complicated. But check the overly commented 'init' function and the c-only 'draws' functions.


Compilation
-----------
The Sdk (to convert Png images) compiles under Linux or a Posix variant, so in Ubuntu should be something like this:

```
sudo apt install libpng-dev
cd Kobe
make sdk
make resources
```

The game only cross-compiles under Linux or a Posix variant targeting to a Dos output. So install [OpenWatcomV2](https://open-watcom.github.io/open-watcom/) and then:

```
cd Kobe
make kobe
```

Check the releases, they provide compiled Kobe binaries.


Issues/Pull Requests
--------------------
Is not my idea to continue the project, I publishing it as an example of an working Mode-X prototype with the hope of being useful as study material. Programming it really helped me on learn about the x86 architecture, drawing routines and 'retro' programming in general, sadly the comments do not reflex this. So if you have any ask open an issue, the same with pull requests to provide better comments or more legible code (or fix my terrible English).

Of course, fork the project and make it yours!.


License
-------
Under the MIT License.

If you copy some lines or an entry function, a simple 'thanks' in the readme file is more than sufficient (do not change your project license for tiny things).


Notes
-----
(1) The x86 architecture is certainly a minefield product of his own history: https://en.wikipedia.org/wiki/X86_memory_segmentation

(2) Mode-X is really versatile allowing to use all the 256 kb of the Vga chip. The 960 pixels width limit obey the fact that its lefts space for an off-screen drawing buffer.

(3) I am using the "Mode-X" term really vague here. While Wolfenstein 3d do not uses a 320x240 px resolution, actually draws everything (every frame) on a planar arrange.

(4) Sprites follow the Mode-X planar arrange allowing being draw using a `memcpy` call every row. Backgrounds an top-to-bottom fashion as they need to be draw a column at time while scrolling (note that Kobe only scrolls horizontally). If desirable a single screen background can be arranged in planar mode and then draw with the fastest sprite routines.
