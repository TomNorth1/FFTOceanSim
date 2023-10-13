#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"


#include "common.h"
#include "SDL2/SDL.h"
#include "Ocean.h"
#include "Shader.h"
#include "Camera.h"
#include "Skybox.h"
#include <iostream>
#include <string>
#include "windows.h"
#include "psapi.h"



#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720


static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

int main(int argc, char* argv[])
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	
	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	self = GetCurrentProcess();
	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

	SDL_Window* window = SDL_CreateWindow("My Game",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!context)
	{
		throw std::runtime_error("Failed to create OpenGL context");
	}

	if (glewInit() != GLEW_OK)
	{
		throw std::runtime_error("Failed to initialize GLEW");
	}


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init("#version 330");

	//simulation settings
	int N = 64;
	float A_multiplied = 5.0f;
	float A = A_multiplied / 100000.0f;
	float windspeed = 32.0f;
	float length = 64;
	int tiling = 10;


	Ocean* ocean = new Ocean(N, A, glm::vec2{ 0.0f,windspeed }, length);
	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(1500.0f, 150.0f, -1500.0f));
	Skybox skybox;

	Uint32 fps_lastime = SDL_GetTicks();
	Uint32 fps_current = 0;
	Uint32 fps_frames = 0;

	bool quit = false;	
	
	float elapsedTime = 0;
	
	glEnable(GL_DEPTH_TEST);
	float cpu_percent = 0;

	while (!quit) 
	{
		
		//ram usage
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		SIZE_T ram_usage = pmc.WorkingSetSize;
		float actual_usage = (float)ram_usage / 1000000.f;

		//CPU usage
		FILETIME ftime, fsys, fuser;
		ULARGE_INTEGER now, sys, user;
		

		
		
		SDL_Event event = { 0 };

		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			camera.handleInput(&event);
		}

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		glm::mat4 projectionMatrix = camera.getProjectionMatrix(45.f, 0.1f, 10000.0f);
		glm::mat4 viewMatrix = camera.getViewMatrix();
		glm::mat4 modelMatrix = glm::mat4(1.0f);


		elapsedTime = float(SDL_GetTicks()) /1000;

		skybox.render(&camera, projectionMatrix);
		ocean->render(elapsedTime, glm::vec3(-165000.0f, 19000.0f, -100000.0f), camera.position,projectionMatrix, viewMatrix, modelMatrix, tiling);

		ImGui::Begin("FFT Ocean simulation");
		ImGui::Text("Controls: WASD to Move\n Left Ctrl and Space to ascend/descend\n Right Click to rotate camera!\n");
		ImGui::Text("AVG FPS: %d", fps_current);
		ImGui::Text("RAM USAGE: %.2fMB", actual_usage);
		ImGui::Text("CPU USAGE: %.2f%%", cpu_percent);
		ImGui::SliderInt("Tiling", &tiling, 1, 50);
		ImGui::SliderFloat("Camera Speed", &camera.speed, 1.0f, 100.0f);

		ImGui::SliderFloat("Wind Speed", &windspeed, 1.0f, 100.0f);
		ImGui::SliderFloat("Amplitude", &A_multiplied, 1.0f, 30.0f);
		ImGui::SliderFloat("Length", &length, 10.0f, 250.0f);
		const char* options[] = { "16","32","64","128","256","512","1024" };
		static const char* current_option = "64";
		if(ImGui::BeginCombo("Grid Size(N)", current_option)) 
		{
			for (int n = 0; n < IM_ARRAYSIZE(options); n++)
			{
				bool is_selected = (current_option == options[n]); 
				
				if (ImGui::Selectable(options[n], is_selected))
				{
					current_option = options[n];
					N = atoi(current_option);
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
				
			}
			ImGui::EndCombo();
		}


		if (ImGui::Button("Apply")) 
		{
			Ocean* oldOcean = ocean;
			A = A_multiplied / 100000.0f;
			ocean = new Ocean(N, A, glm::vec2{ 0.0f,windspeed }, length);
			oldOcean->cleanup();
			delete oldOcean;
		}
		ImGui::End();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//avg fps and cpu usage
		SDL_GL_SwapWindow(window);
		fps_frames++;
		if(fps_lastime < SDL_GetTicks() - 1000)
		{
			//fps
			fps_lastime = SDL_GetTicks();
			fps_current = fps_frames;
			fps_frames = 0;
			//cpu usage
			GetSystemTimeAsFileTime(&ftime);
			memcpy(&now, &ftime, sizeof(FILETIME));

			GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
			memcpy(&sys, &fsys, sizeof(FILETIME));
			memcpy(&user, &fuser, sizeof(FILETIME));
			cpu_percent = (sys.QuadPart - lastSysCPU.QuadPart) +
				(user.QuadPart - lastUserCPU.QuadPart);
			cpu_percent /= (now.QuadPart - lastCPU.QuadPart);
			cpu_percent /= numProcessors;
			lastCPU = now;
			lastUserCPU = user;
			lastSysCPU = sys;

			cpu_percent *= 100;
		}

		

	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	ocean->cleanup();
	delete ocean;

	SDL_Quit();

	return 0;
}

