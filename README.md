# Terrain-generation
OpenGL terrain generation and hydraulic erosion simulation using OpenGL 3.3.

This was my first major graphics project with OpenGL. It began with generating a terrain mesh using my own custom Perlin noise implementation, and progressed to feature Phong lighting, animated water, and shadows. I'm currently attempting to achieve hydraulic erosion simulation on the generated terrain. Below are progress photos.

This is right after I figured out shadows.
![shadow-parallel-light](Game1/screenshots/shadow-parallel-light.png).
There was a lot of experimentation getting the function right for the water animation. This had to be handled in the vertex shader.
![early-water-animation](Game1/screenshots/early-water-animation.png).
![early-water](Game1/screenshots/early-water.png).
![water-high-amplitude](Game1/screenshots/water-high-amplitude.png).
![water-high-frequency](Game1/screenshots/water-high-frequency.png).
The water began to improve with fine tuned specular lighting and better coloring.
![water-specular](Game1/screenshots/water-specular.png).



