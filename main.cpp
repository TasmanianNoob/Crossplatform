#include <iostream>
#include <fstream>
#include <bx/timer.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <algorithm>

#include "stb_image.h"

bgfx::ShaderHandle LoadShader(const std::string& shaderName)
{
	std::ifstream file("../" + std::string(shaderName) + ".bin", std::ios::binary | std::ios::ate); // TODO: FIX LOCATION

	if (!file.is_open())
	{
		return BGFX_INVALID_HANDLE;
	}

	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	char* data = new char[fileSize];
	file.read(data, fileSize);
	file.close();

	const bgfx::Memory* const mem = bgfx::copy(data, static_cast<uint32_t>(fileSize));
	delete[] data;

	auto handle = bgfx::createShader(mem);
	bgfx::setName(handle, shaderName.c_str());

	return handle;
}

bgfx::TextureHandle LoadTexture(const std::string& fileName, bool flip)
{
	int width;
	int height;
	int nrChannels;

	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(("../" + fileName).c_str(), &width, &height, &nrChannels, 0); // TODO: FIX LOCATION

	if (!data)
	{
		std::cout << "DATA FAILED TO LOAD: " << "textures/" + fileName << '\n';
		return BGFX_INVALID_HANDLE;
	}

	const size_t imageSize = width * height * nrChannels;
	const bgfx::Memory* memory = bgfx::copy(data, imageSize);

	stbi_image_free(data);

	return bgfx::createTexture2D(width, height, false, 1, nrChannels == 3 ? bgfx::TextureFormat::RGB8 : bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_UVW_MIRROR, memory);
}

int main(int argc, char **argv)
{
	int width = 800;
	int height = 600;

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		return 1;
	}

	//TODO: CONFIG IF FULLSCREEN glfwGetPrimaryMonitor() ELSE nullptr
	//WIDTH AND HEIGHT WOULD NEED TO BE SET TO ACTUAL SCREEN SIZE
	SDL_Window* window = SDL_CreateWindow("testing 123", width, height, SDL_WINDOW_RESIZABLE);

	if (window == nullptr)
	{
		return 1;
	}

	int fbWidth = 0, fbHeight = 0;

	bgfx::Init init;
#if defined(SDL_PLATFORM_WIN32)
	init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
#elif defined(SDL_PLATFORM_MACOS)
	init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
#elif defined(SDL_PLATFORM_LINUX)
	if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
	{
		init.platformData.ndt = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
		init.platformData.nwh = reinterpret_cast<void*>(SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
	}
	else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0)
	{
		init.platformData.ndt = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
		init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
	}
#elif defined(SDL_PLATFORM_IOS)
	init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, nullptr);
#else
#error Unsupported platform
#endif

	SDL_GetWindowSizeInPixels(window, &fbWidth, &fbHeight);
	init.resolution.width = static_cast<uint32_t>(fbWidth);
	init.resolution.height = static_cast<uint32_t>(fbHeight);
	init.resolution.reset = BGFX_RESET_VSYNC; //TODO: CONFIG SYSTEM

	bgfx::renderFrame();
	bgfx::init(init);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x42B0F5FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	std::string location = R"(..\)"; // TODO: FIX LOCATION

	std::string reverseSlashes = location;
	std::replace(reverseSlashes.begin(), reverseSlashes.end(), '\\', '/');

	std::string command;
#if WIN32
	std::string platformExtension = "bat";
#else
	std::string platformExtension = "sh";
#endif

	switch (bgfx::getRendererType())
	{
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12:
			command = location + "compile.bat s_5_0";
		break;
		case bgfx::RendererType::Metal:
			command = reverseSlashes + "compile.sh metal > /dev/null 2>&1";
		break;
		case bgfx::RendererType::OpenGL:
			command = location + "compile." + platformExtension + " 440";
		break;
		case bgfx::RendererType::Vulkan:
			command = location + "compile" + platformExtension + " s_5_0"; // TODO:
		break;
		default:
			throw std::runtime_error("Unsupported renderer type: RenderInterface_BGFX::CompileShaders");
	}

	std::cout << command << '\n';
	assert(std::system(command.c_str()) == 0);

	bgfx::VertexLayout layout;
	layout
		.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();

	const bgfx::VertexBufferHandle vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), layout);
	const bgfx::UniformHandle smileHandle = bgfx::createUniform("_texSmile", bgfx::UniformType::Sampler);
	const bgfx::UniformHandle containerHandle = bgfx::createUniform("_texContainer", bgfx::UniformType::Sampler);

	const bgfx::ShaderHandle vertShader = LoadShader("vs_main");
	const bgfx::ShaderHandle fragShader = LoadShader("fs_main");
	bgfx::ProgramHandle program = bgfx::createProgram(vertShader, fragShader, true);

	bgfx::TextureHandle smile = LoadTexture("awesomeface.png", true);
	bgfx::TextureHandle container = LoadTexture("container.jpg", true);

	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);

	int64_t m_timeOffset = bx::getHPCounter();

	bool quit = false;
	SDL_Event e;
	while(!quit)
	{
		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_EVENT_QUIT)
			{
				quit = true;
			}
		}

		float time = (float)( (bx::getHPCounter()-m_timeOffset)/double(bx::getHPFrequency() ) );
		float mtx[16];
		bx::mtxRotateXY(mtx, time, time);
		mtx[12] = 0.0f;
		mtx[13] = 0.0f;
		mtx[14] = 0.0f;

		bgfx::touch(0);
		bgfx::setTransform(mtx);
		bgfx::setViewTransform(0, glm::value_ptr(view), glm::value_ptr(projection));

		bgfx::setTexture(0, smileHandle, smile);
		bgfx::setTexture(1, containerHandle, container);

		bgfx::setVertexBuffer(0, vertexBuffer);
		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
		bgfx::submit(0, program);

		bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
		bgfx::frame();
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}