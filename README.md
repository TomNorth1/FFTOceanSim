# FFT Ocean Simulation 

## A FFT Ocean simulation I made for my university dissertation using OpenGL and C++
I was inspired to create this project as part of my dissertation in order to explore how oceans are simulated in games and realtime applications such as Sea of Thieves and War Thunder. This simulation is based on J. Tessendorf's paper "Simulating Ocean Water" which the previously mentioned games based their implementation on aswell.

This project utilizes the properties of the discrete Fourier transform in
order to create a height field that, using statistical methods from the field of oceanography,
resembles an ocean. The project is made using C++ and OpenGL and the resulting simulation
contains a GUI with many parameters for the viewer to change as well as performance information such as framerate, memory usage and cpu usage. The project includes the ability to adjust certain parameters such as:
* Wind Speed
* Amplitude (Depth of ocean)
* Length of each grid
* Grid Size (Resolution of the 2D height field)
* Tiling Ammount

## Demonstration
![](https://media4.giphy.com/media/v1.Y2lkPTc5MGI3NjExaGF0ZWJ2MWFtb3h3OHM2N2RxdXozdGo3ZzN2bDMwOTVjNW0zbXN5bCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/Xs8BNX8vMjWVCVzfEg/giphy.gif)

Ocean simulation with the default set of parameters.

![](https://media0.giphy.com/media/v1.Y2lkPTc5MGI3NjExbnd6OGlleWhpOHd4aXh1NDBrbWMydWpmeTlmbG1ubjdwd3lwbW9qZiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/kbT0SPMkTt1wSkV7aO/giphy.gif)

Ocean simulation with amplitude and windspeed adjusted to produce a "rougher" looking ocean.

## Instructions on how to run
1. Clone this project
2. Navigate to the Release folder
3. Run the OceanSim.exe

## Instructions on how to use
Use W, A, S, D 

## Technical Information
The project was built and tested on Windows 11 and uses the following libraries (which are all found in the OceanSim/contrib folder): 
* glew32
* SDL2
* imgui
* stb_image
* glm
* wavefront
  
## Areas of improvement
Although the project succesfully implements the FFT to create a realistically animated ocean, the shading part of the ocean is rudiementary and in the future, improvements to the surface shader could be made to produce a more visually appealing result i.e adding effects such as wave foam at the crests of the waves and using PBR for the water shader to produce physically accurate results. 

The performance is also an area to be worked on as the program suffers when increasing the grid size above 128x128 which is a problem if this simulation is to be used in realtime applications.

Another area of improvement would be to make this simulation produce an "infinite" ocean as this project was originally made with games and other realtime applications in mind which might require the need for an infinite ocean. The tiling of the ocean height field would also need to be addressed if this idea was explored as it is very noticeable at certain angles and becomes more obvious the bigger the ocean gets. This could be done by implementing some sort of noise to the height field to break up the tiling.
