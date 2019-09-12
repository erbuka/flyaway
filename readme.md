# Flyaway

This project doesn't really have a purpose, I was studying some techniques for procedural generation.

The goal was to develop a framework to generate an infinite world with different biomes and without visible pattern repetition. The terrain generation is mostly made with perlin noise, each time with different seeds, and different properities depending on the biome.

![Screenshot](https://github.com/erbuka/flyaway/blob/master/screenshots/flyaway1.jpg)
![Screenshot](https://github.com/erbuka/flyaway/blob/master/screenshots/flyaway2.jpg)
![Screenshot](https://github.com/erbuka/flyaway/blob/master/screenshots/flyaway3.jpg)
![Screenshot](https://github.com/erbuka/flyaway/blob/master/screenshots/flyaway4.jpg)
![Screenshot](https://github.com/erbuka/flyaway/blob/master/screenshots/flyaway5.jpg)

## Building

This project uses [Premake](https://premake.github.io/) to build project files. As for now, I have only compiled it under windows, but there shouldn't be any problems compiling for other platforms like Linux or MacOS.


## Implementation

The biome concept has been abstracted into a __Biome__ class that defines all the terrain properties, given the 3D-space coordinates.
The __BiomeInterpolator__ class extends Biome, and basically implements a smooth transition between 2 biomes.

Each particual biome extends the __Biome__ base class, therefore is rather easy to add new biomes to the world. Of course this also involves tweaking a few numbers (like the perlin noise parameters, the colors, etc) to make it look nice, but all the transitions are handled gracefully by the __BiomeInterpolator__ class.

Each biome can have objects (like trees, rocks, water). The space is subdivided into a uniform grid, and in each cell can have an object in it. The placement inside the randomized but kept whitin the bounds of the cell, so that objects do not overlap, but also the viewer can't notice the grid division. The object to place is selected by a probability tree where each node branches based on a random value. These values are injected for each cell, and they're calculated with different noise functions.

I also implemented a simple day/night cycle with dawn and sunset. Nothing too special here, the sky and the light changes over time.

I wasn't sure about the graphics style to use. I wanted to make a PBR scene with realitisc objects, but also I wanted to make the 3d models myself. So in the end, I developed my own cell shading, and since the models would be low poly, I was able to do it myself.

