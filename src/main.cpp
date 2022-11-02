#include <glad/glad.h> // GLAD: https://github.com/Dav1dde/glad GLAD 2 also works via the web-service: https://gen.glad.sh/ (leaving all checkbox options unchecked)
#include <GLFW/glfw3.h>
 
#include <ft2build.h> // https://freetype.org/freetype2/docs/tutorial/step1.html#section-1
#include FT_FREETYPE_H
 
// OpenGL Mathematics(GLM) https://github.com/g-truc/glm/blob/master/manual.md
// ------------------------------------
// GLM Headers
// ------------------
#include <glm/glm.hpp> // Include all GLM core.	
#include <glm/gtc/matrix_transform.hpp> // Specific extensions.
#include <glm/gtc/type_ptr.hpp>
 
#include <vector>
#include <iostream>
#include <fstream> // Used in "shader_configure.h" to read the shader text files.
 
#include "shader_configure.h" // Used to create the shaders.
#include "text_fonts_glyphs.h"
 
int main()
{
	// (1) GLFW: Initialise & Configure
	// -----------------------------------------
	if (!glfwInit())
		exit(EXIT_FAILURE);
 
	glfwWindowHint(GLFW_SAMPLES, 4); // Anti-aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
 
	int monitor_width = mode->width; // Monitor's width and height.
	int monitor_height = mode->height;
 
	int window_width = (int)(monitor_width * 0.75f); // Example: monitor_width * 0.5f... will be 50% the monitor's size.
	int window_height = (int)(monitor_height * 0.75f); // Cast is simply to silence the compiler warning.
 
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "FreeType Fonts - Glyphs Text (3D Animation)", NULL, NULL);
	// GLFWwindow* window = glfwCreateWindow(window_width, window_height, "FreeType Fonts - Glyphs Text (3D Animation)", glfwGetPrimaryMonitor(), NULL); // Full Screen Mode ("Alt" + "F4" to Exit!)
 
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window); // Set the window to be used and then centre that window on the monitor. 
	glfwSetWindowPos(window, (monitor_width - window_width) / 2, (monitor_height - window_height) / 2);
 
	glfwSwapInterval(1); // Set VSync rate 1:1 with monitor's refresh rate.
	
	// (2) GLAD: Load OpenGL Function Pointers
	// -------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // For GLAD 2 use the following instead: gladLoadGL(glfwGetProcAddress)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glEnable(GL_DEPTH_TEST); // Enabling depth testing allows rear faces of 3D objects to be hidden behind front faces.
	glEnable(GL_MULTISAMPLE); // Anti-aliasing
	glEnable(GL_BLEND); // GL_BLEND for OpenGL transparency which is further set within the fragment shader. 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
 
	 // (3) Compile Shaders & Initialise Camera 
	// -----------------------------------------------------
	const char* vert_shader_text = "/home/ryan/Documents/tests/opengl-text/shaders/shader_text.vert";
	const char* frag_shader_text = "/home/ryan/Documents/tests/opengl-text/shaders/shader_text.frag";
 
	Shader text_shader(vert_shader_text, frag_shader_text);
	text_shader.use();
 
	/* glm::vec3 camera_position(0.0f, 0.0f, 3.0f); // -Z is into the screen.
	glm::vec3 camera_target(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_up(0.0f, 1.0f, 0.0f);
 
	glm::mat4 view = glm::lookAt(camera_position, camera_target, camera_up);
	glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float)window_width / (float)window_height, 0.1f, 100.0f); */
 
	// (4) Initialise FreeType & Declare Text Objects
	// -----------------------------------------------------------
	// "How is it possible that the HorizontalAdvance of a glyph is smaller than the glyph's width?"
	// https://stackoverflow.com/questions/45304838/how-is-it-possible-that-the-horizontaladvance-of-a-glyph-is-smaller-than-the-gly	
	
	FT_Library free_type;
	FT_Error error_code = FT_Init_FreeType(&free_type);
	if (error_code)
	{
		std::cout << "\n   Error code: " << error_code << " --- " << "An error occurred during initialising the FT_Library";
		int keep_console_open;
		std::cin >> keep_console_open;
	}
	/* Text text_object1(free_type, window_width, window_height, "1234567890&.-abcdefghijklmnopqrstuvwxyz:_ABCDEFGHIJKLMNOPQRSTUVWXYZ "); // Pass a specific alphabet to be used for this specific text object.
	text_object1.create_text_message("Using OpenGL and the FreeType library to render", 110, 60, "Text Fonts/cambriaz.ttf", 70, false);
	text_object1.create_text_message("and animate TrueType fonts.", 110, 180, "Text Fonts/cambriaz.ttf", 70, false); */
 
	Text text_object2(free_type, window_width, window_height, "01234567890Get Rady.Timr:owns&ClBgfb"); // Declare a new text object, passing in your chosen alphabet.	
	text_object2.create_text_message("Get Ready... Timer: 000", 150, 100, "/home/ryan/Documents/tests/opengl-text/font/arialbi.ttf", 130, true); // True indicates that the message will be modified.
 
	int num_replace = 3;
	size_t vec_size = text_object2.messages[0].characters_quads.size();
	float start_pos = text_object2.messages[0].start_x_current[vec_size - num_replace];
	
	int display_counting = 0;
	int get_ready = 0;
	bool running = false; 
	
	/* unsigned int view_mat_text_loc = glGetUniformLocation(text_shader.ID, "view");
	unsigned int proj_mat_text_loc = glGetUniformLocation(text_shader.ID, "projection");
	unsigned int anim_text_loc = glGetUniformLocation(text_shader.ID, "animate");
 
	glUniformMatrix4fv(view_mat_text_loc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(proj_mat_text_loc, 1, GL_FALSE, glm::value_ptr(projection)); */
 
	glUniform1i(glGetUniformLocation(text_shader.ID, "alphabet_texture"), 31);
 
	glm::vec3 RGB(10.0f, 120.0f, 105.0f);
	unsigned int font_colour_loc = glGetUniformLocation(text_shader.ID, "font_colour");
	glUniform3fv(font_colour_loc, 1, glm::value_ptr(RGB));	
 
	// (5) Enter the Main-Loop
	// --------------------------------
	 /* srand((unsigned)time(NULL)); // Initialise random seed.
 
	float x_spin = 1.0f / (rand() % 10 + 1); // Generate random number between 1 and 10
	float y_spin = 1.0f / (rand() % 10 + 1);
	float z_spin = 1.0f / (rand() % 10 + 1);
	float spin_speed = (float)(rand() % 5 + 1);
 
	float spin_vary = 0.0f;
	int spin_dir = 1;
 
	glm::mat4 spinning_mat(1.0f); */
 
	while (!glfwWindowShouldClose(window)) // Main-Loop
	{
		// (6) Randomise the Model's Spinning Speed & Axis
		// ------------------------------------------------------------------
		/* spin_vary += 0.05f * spin_dir;
 
		if (spin_vary > 6 || spin_vary < 0)
		{
			spin_dir = -spin_dir; // Reverse the spinning direction.
 
			x_spin = 1.0f / (rand() % 10 + 1);
			y_spin = 1.0f / (rand() % 10 + 1);
			z_spin = 1.0f / (rand() % 10 + 1);
			spin_speed = (float)(rand() % 50 + 1) / 40;			
		}		
		spinning_mat = glm::rotate(spinning_mat, glm::radians(spin_speed), glm::normalize(glm::vec3(x_spin, y_spin, z_spin))); */
 
		// (7) Clear the Screen & Depth Buffer
		// ----------------------------------------------
		glClearColor(210.0f / 255, 240.0f / 255, 250.0f / 255, 1.0f); // This line can be moved to before the while loop.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
		
		// (8) Draw the Alphabets & Messages
		// -----------------------------------------------
		// text_object1.draw_messages();
		// text_object1.draw_alphabets();
 
		 /* glUniformMatrix4fv(anim_text_loc, 1, GL_FALSE, glm::value_ptr(spinning_mat)); */
 
		if (!running)
			++get_ready;
		if (get_ready > 125)
			running = true;
 
		if (running)
		{
			++display_counting;
			if (display_counting == 300)
				display_counting = 0;
 
			// std::cout << "\n   display_counting: " << display_counting << " ---  display_counting % 10: " << display_counting % 10 << " ---  display_counting / 10 % 10: "
				// << display_counting / 10 % 10 << " --- display_counting / 100 % 10: " << display_counting / 100 % 10;
 
			unsigned num1 = display_counting / 100 % 10; // Left digit.
			unsigned num2 = display_counting / 10 % 10;
			unsigned num3 = display_counting % 10;
 
			float advance1 = text_object2.messages[0].alphabet_vec[num1].glyph_advance_x;
			float advance2 = advance1 + (text_object2.messages[0].alphabet_vec[num2].glyph_advance_x);
			
			text_object2.messages[0].characters_quads.resize(vec_size - num_replace);
			text_object2.messages[0].text_start_x = start_pos;
 
			text_object2.process_text_index(text_object2.messages[0], num1, 0); // Important: the number of calls to: process_text_index(...) must = "num_replace_characters"
			text_object2.process_text_index(text_object2.messages[0], num2, advance1);
			text_object2.process_text_index(text_object2.messages[0], num3, advance2);
			
			text_object2.update_buffer_data_message(text_object2.messages[0], (int)(vec_size - num_replace));
		}	
		text_object2.draw_messages(0);		
		text_object2.draw_alphabets();  
 
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
 
	// (9) Exit the Application
	// ------------------------------
	// FT_Done_Face(text_object1.face);
	FT_Done_Face(text_object2.face);
	FT_Done_FreeType(free_type);		
	glDeleteProgram(text_shader.ID);
 
	/* glfwDestroyWindow(window) // Call this function to destroy a specific window */	
	glfwTerminate(); // Destroys all remaining windows and cursors, restores modified gamma ramps, and frees resources.
 
	exit(EXIT_SUCCESS); // Function call: exit() is a C/C++ function that performs various tasks to help clean up resources.
}
