#ifndef IMGUI_UTILS_HPP
#define IMGUI_UTILS_HPP

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#	define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_internal.h>

#include "await.hpp"

#include <lak/string.hpp>

#include <atomic>
#include <filesystem>
#include <thread>

namespace lak
{
	namespace fs = std::filesystem;

	bool input_text(const char *str_id,
	                char *buf,
	                size_t buf_size,
	                ImGuiInputTextFlags flags       = 0,
	                ImGuiInputTextCallback callback = nullptr,
	                void *user_data                 = nullptr);

	bool input_text(const char *str_id,
	                lak::astring *str,
	                ImGuiInputTextFlags flags       = 0,
	                ImGuiInputTextCallback callback = nullptr,
	                void *user_data                 = nullptr);

	bool path_navigator(fs::path &path,
	                    std::error_code &ec,
	                    ImVec2 size = {0, 0});

	enum struct file_open_error
	{
		INCOMPLETE,
		INVALID,
		CANCELED,
		VALID
	};

	lak::result<file_open_error, std::error_code> open_file(
	  fs::path &path, bool save, ImVec2 size = {0, 0});

	lak::result<file_open_error, std::error_code> open_folder(fs::path &path,
	                                                          ImVec2 size = {0,
	                                                                         0});

	lak::result<file_open_error, std::error_code> open_file_modal(fs::path &path,
	                                                              bool save);

	lak::result<file_open_error, std::error_code> open_folder_modal(
	  fs::path &path);

	template<typename R, typename... T, typename... D>
	bool await_popup(const char *str_id,
	                 bool &open,
	                 std::unique_ptr<std::thread> &staticThread,
	                 std::atomic<bool> &staticFinished,
	                 R (*callback)(T...),
	                 const std::tuple<D...> &callbackData)
	{
		if (ImGui::BeginPopup(str_id, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (lak::await(staticThread, staticFinished, callback, callbackData))
			{
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				open = false;
				return false;
			}
			open = true;
		}
		else
		{
			open = false;
			ImGui::OpenPopup(str_id);
		}

		return true;
	}

}

#endif
