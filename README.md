# FFT Ocean Simulation 

## A FFT Ocean simulation I made for my university dissertation using OpenGL and C++
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
Although the project succesfully implements the FFT to create a realistically animated ocean the shading part of the ocean is rudiementary and in the future improvements to the surface shader could be made to produce a more visually appealing result. 
The performance is also an area to be worked on as the program suffers when increasing the grid size above 128x128 which is a problem.
