# Terrain-generation
OpenGL terrain generation and hydraulic erosion simulation using OpenGL 3.3.

This was my first major graphics project with OpenGL. It began with generating a terrain mesh using my own custom Perlin noise implementation, and progressed to feature Phong lighting, animated water, and shadows. I'm currently attempting to achieve hydraulic erosion simulation on the generated terrain. Below are progress photos.


There was a lot of experimentation getting the function right for the water animation. This had to be handled in the vertex shader.
![early-water-animation](Game1/screenshots/early-water-animation.png)
![early-water](Game1/screenshots/early-water.png)
![water-high-amplitude](Game1/screenshots/water-high-amplitude.png)
![water-high-frequency](Game1/screenshots/water-high-frequency.png)
The water began to improve with fine tuned specular lighting and better coloring.
![water-specular](Game1/screenshots/water-specular.png)
![early-water-animation](Game1/screenshots/early-water-animation.gif)
![early-water-animation2](Game1/screenshots/early-water-animation2.gif)

This water satisfied me so I moved on to erosion simulation.
![water-final](Game1/screenshots/water-final.gif)

Erosion simulation began with checking to make sure the gradient was calculated properly. I did this with visualizations shown below.
![gradient](Game1/screenshots/gradient.png)
![gradient](Game1/screenshots/gradient2.png)

I then began attempting to simulate water droplets rolling down the terrain with momentum and a sedimentation coefficient. These droplets pick up sedimentation the faster they roll and deposit it when they slow down.

This image shows early signs the sedimentation was starting to work, although enevenly.
![sedimentation](Game1/screenshots/sedimentation.png)

This image shows the beginnings of successful hydraulic erosion.
![early-erosion](Game1/screenshots/early-erosion.png)



