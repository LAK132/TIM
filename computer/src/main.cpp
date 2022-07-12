#include "main.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_impl_lak.hpp"
#include "imgui_utils.hpp"

#include <GL/gl3w.h>
#include <SDL2/SDL.h>

#include <lak/file.hpp>
#include <lak/window.hpp>

#include <lak/basic_program.inl>

int opengl_major, opengl_minor;
ImGui::ImplContext imgui_context = nullptr;

lak::optional<int> basic_window_preinit(int argc, char **argv)
{
	if (argc == 2 && argv[1] == lak::astring("--version"))
	{
		std::cout << "Tyre Impact Monitor " APP_VERSION << "\n";
		return lak::optional<int>(0);
	}
	else if (argc == 2 && argv[1] == lak::astring("--full-version"))
	{
		std::cout << APP_NAME << "\n";
		return lak::optional<int>(0);
	}

	lak::debugger.std_out(u8"", u8"" APP_NAME "\n");

	for (int arg = 1; arg < argc; ++arg)
	{
		if (argv[arg] == lak::astring("--help"))
		{
			std::cout << "tim.exe [--version] [--full-version] [--help] [--onlyerr] "
			             "[--nogl] "
			             "[--listtests | --laktestall | --laktests \"test1;test2\"] "
			             "[<filepath>]\n";
			return lak::optional<int>(0);
		}
		else if (argv[arg] == lak::astring("--nogl"))
		{
			basic_window_force_software = true;
		}
		// else if (argv[arg] == lak::astring("--onlyerr"))
		// {
		// 	force_only_error = true;
		// }
		// else if (argv[arg] == lak::astring("--listtests"))
		// {
		// 	lak::debugger.std_out(lak::u8string(),
		// 	                      lak::u8string(u8"Available tests:\n"));
		// 	for (const auto &[name, func] : lak::registered_tests())
		// 	{
		// 		lak::debugger.std_out(lak::u8string(),
		// 		                      lak::to_u8string(name) + u8"\n");
		// 	}
		// }
		// else if (argv[arg] == lak::astring("--laktestall"))
		// {
		// 	return lak::optional<int>(lak::run_tests());
		// }
		// else if (argv[arg] == lak::astring("--laktests") ||
		//          argv[arg] == lak::astring("--laktest"))
		// {
		// 	++arg;
		// 	if (arg >= argc) FATAL("Missing tests");
		// 	return lak::optional<int>(lak::run_tests(
		// 	  lak::as_u8string(lak::astring_view::from_c_str(argv[arg]))));
		// }
	}

	basic_window_target_framerate      = 30;
	basic_window_opengl_settings.major = 3;
	basic_window_opengl_settings.minor = 2;
	basic_window_clear_colour          = {0.0f, 0.0f, 0.0f, 1.0f};

	return lak::nullopt;
}

void basic_window_init(lak::window &window)
{
	lak::debugger.crash_path = lak::fs::current_path() / "tim-crash-log.txt";

	lak::debugger.live_output_enabled = true;

	imgui_context = ImGui::ImplCreateContext(window.graphics());
	ImGui::ImplInit();
	ImGui::ImplInitContext(imgui_context, window);

	DEBUG("Graphics: ", window.graphics());
	if (!lak::debugger.live_output_enabled || lak::debugger.live_errors_only)
		std::cout << "Graphics: " << window.graphics() << "\n";

	switch (window.graphics())
	{
		case lak::graphics_mode::OpenGL:
		{
			opengl_major = lak::opengl::get_uint(GL_MAJOR_VERSION);
			opengl_minor = lak::opengl::get_uint(GL_MINOR_VERSION);
		}
		break;

		case lak::graphics_mode::Software:
		{
			ImGuiStyle &style      = ImGui::GetStyle();
			style.AntiAliasedLines = false;
			style.AntiAliasedFill  = false;
			style.WindowRounding   = 0.0f;
		}
		break;

		default: break;
	}

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowRounding = 0;
}

void basic_window_handle_event(lak::window &, lak::event &event)
{
	ImGui::ImplProcessEvent(imgui_context, event);

	switch (event.type)
	{
		case lak::event_type::dropfile: break;
		default: break;
	}
}

void basic_window_loop(lak::window &window, uint64_t counter_delta)
{
	const float frame_time = (float)counter_delta / lak::performance_frequency();
	ImGui::ImplNewFrame(imgui_context, window, frame_time);

	bool mainOpen = true;

	ImGuiStyle &style = ImGui::GetStyle();
	ImGuiIO &io       = ImGui::GetIO();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(io.DisplaySize);
	ImVec2 old_window_padding = style.WindowPadding;
	style.WindowPadding       = ImVec2(0.0f, 0.0f);
	if (ImGui::Begin(APP_NAME,
	                 &mainOpen,
	                 ImGuiWindowFlags_AlwaysAutoResize |
	                   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar |
	                   ImGuiWindowFlags_NoSavedSettings |
	                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
	{
		style.WindowPadding = old_window_padding;

		ImGui::End();
	}

	ImGui::ImplRender(imgui_context);
}

int basic_window_quit(lak::window &)
{
	ImGui::ImplShutdownContext(imgui_context);
	return 0;
}
