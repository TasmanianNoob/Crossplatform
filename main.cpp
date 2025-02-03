// #define SDL_PLATFORM_ANDROID 1

#include <iostream>
#include <vector>
#include <fstream>
#include <bx/timer.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined(SDL_PLATFORM_IOS)
#include "sdl_bgfx_ios.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <algorithm>

#include "stb_image.h"

#if defined(SDL_PLATFORM_ANDROID)
std::string filePath = "";
#elif defined(SDL_PLATFORM_WIN32)
std::string filePath = "..\\assets\\";
#elif defined(SDL_PLATFORM_MACOS)
#if defined(XCODE_BUILD)
std::string filePath = "../../assets/";
#else
std::string filePath = "../assets/";
#endif
#elif defined(SDL_PLATFORM_LINUX)
std::string filePath = "../assets/";
#elif defined(SDL_PLATFORM_IOS)
std::string filePath = "";
#else
#error Unsupported platform
#endif

std::vector<char> ReadFile( const std::string& fileName)
{
	const std::string fullPath = filePath + fileName;
    SDL_IOStream* file = SDL_IOFromFile(fullPath.c_str(), "rb");
    std::vector<char> data;

    if (!file)
    {
	    SDL_Log("File (%s) failed to load: %s", fullPath.c_str(), SDL_GetError());
	    SDL_CloseIO(file);
	    return data;
    }

    const int64_t size = SDL_SeekIO(file, 0, SDL_IO_SEEK_END);
	data.resize(size);

    // go back to the start of the file. TODO: Is this right?
    SDL_SeekIO(file, -size, SDL_IO_SEEK_END);
    if (SDL_ReadIO(file, data.data(), size) == 0 && SDL_GetIOStatus(file) != SDL_IO_STATUS_EOF)
    {
        SDL_Log("File (%s) failed to read: %s", fullPath.c_str(), SDL_GetError());
		SDL_CloseIO(file);
        return data;
    }

    SDL_CloseIO(file);
    return data;
}

bgfx::ShaderHandle LoadShader(const std::string& shaderName)
{
	std::string shaderType;
	switch (bgfx::getRendererType())
	{
		case bgfx::RendererType::Direct3D11:
			shaderType = "dx10_";
        break;
		case bgfx::RendererType::Direct3D12:
			shaderType = "dx11_";
		break;
		case bgfx::RendererType::Metal:
			shaderType = "msl_";
		break;
		case bgfx::RendererType::OpenGL:
			shaderType = "glsl_";
		break;
		case bgfx::RendererType::OpenGLES:
			shaderType = "essl_";
		break;
		case bgfx::RendererType::Vulkan:
			shaderType = "spirv_";
		break;
		default:
			return BGFX_INVALID_HANDLE;
	}

    const std::string shaderFile = shaderType + shaderName + ".bin";
    const std::vector<char> data = ReadFile(shaderFile);

	if (data.empty())
	{
		return BGFX_INVALID_HANDLE;
	}

	const bgfx::Memory* const mem = bgfx::copy(data.data(), static_cast<uint32_t>(data.size()));

	const bgfx::ShaderHandle handle = bgfx::createShader(mem);
	bgfx::setName(handle, shaderName.c_str());

	return handle;
}

bgfx::TextureHandle LoadTexture(const std::string& fileName, bool flip)
{
	const std::vector<char> data = ReadFile(fileName);

	if (data.empty())
	{
		return BGFX_INVALID_HANDLE;
	}

	int width;
	int height;
	int nrChannels;

	stbi_set_flip_vertically_on_load(flip);
	unsigned char* img = stbi_load_from_memory(reinterpret_cast<stbi_uc const*>(data.data()), data.size(), &width, &height, &nrChannels, 0);

	if (!img)
	{
        SDL_Log("Invalid image data for %s", fileName.c_str());
		return BGFX_INVALID_HANDLE;
	}
	const size_t imageSize = width * height * nrChannels;
	const bgfx::Memory* memory = bgfx::copy(img, imageSize);
	stbi_image_free(img);

	return bgfx::createTexture2D(width, height, false, 1, nrChannels == 3 ? bgfx::TextureFormat::RGB8 : bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_UVW_MIRROR, memory);
}

int main(int argc, char* args[])
{
	int width = 800;
	int height = 600;
    
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
    SDL_SetHint(SDL_HINT_IOS_HIDE_HOME_INDICATOR, "2");
    
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		return 1;
	}

	//TODO: CONFIG IF FULLSCREEN glfwGetPrimaryMonitor() ELSE nullptr
	//WIDTH AND HEIGHT WOULD NEED TO BE SET TO ACTUAL SCREEN SIZE
	SDL_Window* window = SDL_CreateWindow("testing 123", width, height, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_BORDERLESS);

	if (window == nullptr)
	{
		return 1;
	}

	int fbWidth = 0, fbHeight = 0;

	SDL_Log("Hello LogCat!");

	bgfx::Init init;
#if defined(SDL_PLATFORM_ANDROID)
	init.platformData.ndt = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_ANDROID_SURFACE_POINTER, nullptr);
	init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);
#elif defined(SDL_PLATFORM_WIN32)
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
    init.platformData.nwh = CreateMetalLayer(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, nullptr));
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

	bgfx::TextureHandle smile = LoadTexture("Test/awesomeface.png", true);
	bgfx::TextureHandle container = LoadTexture("Test/container.jpg", true);

	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);

	const int64_t m_timeOffset = bx::getHPCounter();

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

		auto time = static_cast<float>((bx::getHPCounter() - m_timeOffset) / static_cast<double>(bx::getHPFrequency()));
		float mtx[16];
		bx::mtxRotateXY(mtx, time, time);
		mtx[12] = 0.0f;
		mtx[13] = 0.0f;
		mtx[14] = 0.0f;

		bgfx::touch(0);
		bgfx::setViewTransform(0, glm::value_ptr(view), glm::value_ptr(projection));
		bgfx::setTransform(mtx);

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
