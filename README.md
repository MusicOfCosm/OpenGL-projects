# OpenGL projects

This repository contains most of the projects that I've done in OpenGL.

I learned OpenGL during the summer of 2022. I tried multiple different tutorials at first, but I couldn't manage to stick with any of them. The one by [Yan Chernikov](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2) was good to get started, especially in regards to using Visual Studio, but I find that he talks too much: The first quarter to a third of the videos is often just spent talking. However, his videos on C++ did help me understand the language better. And he did introduce me to [ImGui](https://github.com/ocornut/imgui).

Eventually, I followed the [tutorials by Joey de Vries](https://learnopengl.com/), which I have decided not to include in this repository. I have done the *Getting started*, *Lighting*, and *Model Loading* sections (though I didn't like the last section because I didn't understand it much), and stopped at *Framebuffers* in the *Advanced OpenGL* section.

One thing to know, is that I didn't really agree with the tutorials making classes for each components such as shaders and camera, but I did make a class that has a VAO, a VBO, and a shader program. It can also load textures and framebuffers.



## Chaotic_attractor

Inspired by [this video](https://www.youtube.com/watch?v=idpOunnpKTo), I thought to use that as a gift for a relative, as I often do with these projects. I managed to make it work in a surprisingly short amount of time; setting up controls with ImGui took longer.

I originally did this project in October 2022 in Visual Studio, using [vcpkg](https://learn.microsoft.com/en-us/vcpkg) for the first time. I later remade it using CMake in July 2023, adding a stereoscopic version of it: I got interested in stereograms some years back (January 2020 I believe), and wanted to view the scene in 3D without glasses. After a day or two of testing (which I didn't include in this repository), it turned out that you merely needed to draw everything twice, once while the camera is translated to the left then once to the right (or the other way around), with the viewport taking half the screen.

You can right click to free the cursor, and left click to bring it back. You can also zoom in and out, though it's slower using a mouse.



## Slime_mold

*This project uses vcpkg*

I first did this project using OpenCV, but this time I did it in OpenGL in December 2022.

### V1
In V1, I use another [framebuffer](https://learnopengl.com/Advanced-OpenGL/Framebuffers) for the fading and blurring effects.

**Explanation of the main loop:**
First, the agents are drawn on the created framebuffer to use it as a texture for the default framebuffer, which applies the fading and blurring effects.
The results are sent back to the created framebuffer with `glBlitFramebuffer()`, then the pixels are read from the default one and stored into a dynamic array, which is used to update the agents' position.

The downside of this version is that, since the ImGui window part of the OpenGL context, its pixels also get transferred to the framebuffer texture, so it messes with the simulation. 


### V2

While faster than its OpenCV counterpart, I wasn't that impressed by the simulation's speed. I decided to try my hand at compute shaders, since from what I had seen, they seemed to be exactly what was needed for this kind of simulation.

I found [some](https://learnopengl.com/Guest-Articles/2022/Compute-Shaders/Introduction) [tutorials](https://zestedesavoir.com/tutoriels/1554/introduction-aux-compute-shaders/), though the one that I think helped me most was [this one](https://www.youtube.com/watch?v=nF4X9BIUzx0).

If the starting position is the center or the corner(s), the program starts out at 60 fps, but as the agents take more of the screen space, it drops to around 10 (at least for me). No matter how I dispatch the workgroup, I couldn't make it go faster. Though I suppose that, if we compare the difference in the number of agents compare to V1, this one is better.



## Cellular_automata

*This project was done using Visual Studio, CMake, and vcpkg*

I first tried doing Conway's Game of Life using WebGL (think of it as OpenGL but on the Web) because I wanted to be able to do stuff for relatives that are on MacOS.

[Andrew Adamson's tutorials](https://www.youtube.com/playlist?list=PLPbmjY2NVO_X1U1JzLxLDdRn4NmtxyQQo) were an extremely great ressource to learn WebGL (or even the regular OpenGL).
Unfortunately, due to my lack of knowledge in JavaScript and asynchronous programming, I failed (I figured out months later that I only needed to use a function with `RequestAnimationFrame()` instead of a `while` loop).

I made the *Cellular_automata* folder to try and do, well... cellular automata. I had realized that the double framebuffer setup that I used for *Slime_mold/V1* could be used for a lot of cellular automata and other simulations with simple rules, not just Conway's Game of Life (which I made here). Inspired once again by Sebastian Lague and [one of his videos](https://www.youtube.com/watch?v=kzwT3wQWAHE), I tried [Reaction-Diffusion](https://www.karlsims.com/rd.html), but for the life of me, I couldn't manage it! Whether in be here, or using OpenCV (either with C++ or Python/Cython). I did not include my attempt in the repository.

GPU_Info just makes a text file with your GPU information on it.



## Fractals

*This project was done using vcpkg*

So far, I had only been doing OpenGL projects using C++, but I wanted to see if I could do one only in C.

Inspired by [this video](https://www.youtube.com/watch?v=uc2yok_pLV4), I decided to do the same thing in openGL.

This is the also the first project that I compiled for MacOS. I found out two annoying things about it: the first I figured out (and I couldn't find anything on the internet about), was that you had to double the viewport size on the Mac for some reason (something with the dpi maybe?).
The second is partially due to C not having raw strings literals (unlike C++), which allow me to do the trick where I `#include` the shader files. Unfortunately, it seems the Mac doesn't take into account the path of the executable when using it, so I had to mess with `argv[0]` (the full path of the file) for the executable to read the shader files.

...Or at least those were the problems before. I tested the project on the Mac before committing it to this repository, but it doesn't work properly anymore (except to tell me about shader errors): the screen is all black, even when telling the fragment shader to just output `vec4(1)`. I tested Chaotic attractor, since it's the other project that I have also compiled on the Mac, and while it does show something on the screen, it seems `glClear()` doesn't take effect. Furthermore, when printing GPU information to the terminal, the `glGetString()` are all replaced by `(null)`.
Considering that the executables that I first compiled before work completely fine, I would guess that an update may have messed something up.



## Not included

There are things that I didn't include in this repository:
- Tutorial: I already mentioned that one, it contains stuff that I made following Joey de Vries' tutorials. You can still see files that I used in the ressources folder.
- 2D_physics: I didn't actually do anything with this, except prepare a dummy OpenGL project. I instead used OpenCV, but the most I did was bounce a red ball on the screen. With multiple, I couldn't even make them bounce against one another. I quickly abandoned it since it was the start of the 2022-2023 school year.
- Particle_Life: after the Slime mold simulation and Conway's Game of Life, I wanted to try my hand at other systems with emergent behavior. I attempted to do [Particle Life](https://www.youtube.com/watch?v=p4YirERTVF0), both with the double framebuffer setup, and with a computer shader, but the results were underwhelming. I also tried Boids, or the simulation of flocking behavior, and while the result was better, it still wasn't that great.
- Stereoscopy: This was just a folder where I tested how to do stereoscopy. I was satisfied after I changed a program from the tutorial to be in 3D when crossing your eyes.
- Langton's ant: I've talked about this one in my OpenCV repository. I couldn't manage to do it here, only using Python and OpenCV.

___

*Originally, I had and would commit and push this repository to GitHub every time I cleaned up a project. However, Git and/or GitHub absolutely suck and decided to hit me with a merge conflict where there was none (it claimed there was conflicting changes in README.md, but I was just appending to it). After solving the conflict and pushing to GitHub, Slime_mold and Cellular_Automata had disappeared! I couldn't repair the damage so decided to delete the repository and remake it.*

*That wasn't the best idea, in Cellular_automata, it deleted 9 files (including CMakeLists.txt) and a folder! More than that, all the source files in Slime_mold, including .sln and .vcxproj are gone too! Thankfully, I had a backup.*