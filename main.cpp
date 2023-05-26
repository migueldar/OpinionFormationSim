#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/imgui.h"
#include "model.hpp"
#include <chrono>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

static void glfw_error_callback(int error, const char *description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main() {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create window with graphics context
	GLFWwindow *window = glfwCreateWindow(1280, 720, "Opinion formation model", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// state
	Model model = Model();
	ModelConfig modelConf;
	bool generated = false;
	bool simulating = false;
	bool paused = false;
	int n_pixels = 10;

	//texture creation
	GLuint modelTexture;
    glGenTextures(1, &modelTexture);
    glBindTexture(GL_TEXTURE_2D, modelTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Controls window
		ImGui::Begin("Controls");
		ImGui::BeginDisabled(simulating);
		ImGui::Text("Set the parameters of the simulation here");
		ImGui::SliderInt("Number of rows", &(modelConf.n_rows), 1, 100);
		ImGui::SliderInt("Number of columns", &(modelConf.n_cols), 1, 100);
		ImGui::SliderFloat("Probability of interaction", &(modelConf.p_interaction), 0.0f, 1.0f);
		ImGui::SliderFloat("Interaction threshold", &(modelConf.interaction_max), 0.0f, 1.0f);
		ImGui::SliderFloat("Convergence parameter", &(modelConf.convergence_parameter), 0.0f, 0.5f);
		ImGui::EndDisabled();
		ImGui::SliderInt("Zoom", &n_pixels, 1, 30);
		ImGui::BeginDisabled(simulating);

		if (ImGui::Button("Generate opinions")) {
			generated = true;
			model.setOptions(modelConf);
			model.generateOpinions();
			model.generateConnections();
			model.updateImage(modelTexture);
		} 
		ImGui::SameLine();
		ImGui::EndDisabled();
		ImGui::BeginDisabled((simulating && !paused) || !generated);
		if (ImGui::Button("Start/Continue") && generated)
		{
			paused = false;
			simulating = true;
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::BeginDisabled(!simulating);
		if (ImGui::Button("Pause"))
			paused = true;
		ImGui::SameLine();
		if (ImGui::Button("End"))
		{
			generated = false;
			paused = false;
			simulating = false;
		}
		ImGui::EndDisabled();
		if (simulating) {
			model.interact();
			model.updateImage(modelTexture);
		}
		ImGui::End();

		// Simulation window
		ImGui::Begin("Simulation");
		if (generated)
			ImGui::Image(reinterpret_cast<ImTextureID>(modelTexture), ImVec2(model.getCols() * n_pixels, model.getRows() * n_pixels));
		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.45f, 0.55f, 0.60f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}