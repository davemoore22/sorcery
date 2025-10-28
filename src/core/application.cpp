// Copyright (C) 2025 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "core/application.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "engine/define.hpp"
#include "engine/engine.hpp"
#include "frontend/mainmenu.hpp"
#include "frontend/splash.hpp"
#include "gui/define.hpp"
#include "modules/castle.hpp"
#include "modules/edgeoftown.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

// Standard Constructor
Sorcery::Application::Application(int argc, char **argv) {

	// Setup a Global Exception Handler as early as possible
	std::set_terminate([]() -> void {
		std::cerr << "Terminate called after throwing an instance of ";
		try {
			std::rethrow_exception(std::current_exception());
		} catch (const std::exception &ex) {
			std::cerr << typeid(ex).name() << std::endl;
			std::cerr << "  what(): " << ex.what() << std::endl;
		} catch (...) {
			std::cerr << typeid(std::current_exception()).name() << std::endl;
			std::cerr << " ...something else!" << std::endl;
		}
		std::cerr << "errno: " << errno << ": " << std::strerror(errno)
				  << std::endl;
		std::abort();
	});

	// Get any command line arguments
	_args.clear();
	for (auto loop = 0; loop < argc; ++loop) {
		std::string arg{argv[loop]};
		std::ranges::transform(arg.begin(), arg.end(), arg.begin(),
							   [](unsigned char c) -> unsigned char {
								   return std::tolower(c);
							   });
		_args.push_back(arg);
	}

	// Set up all the Core Modules
	_system = std::make_unique<System>(argc, argv);
	_display = std::make_unique<Display>(_system.get());
	_resources = std::make_unique<Resources>(_system.get());
	_controller = std::make_unique<Controller>(_system.get(), _display.get(),
											   _resources.get());
	_ui = std::make_unique<UI>(_system.get(), _display.get(), _resources.get(),
							   _controller.get());
	_game = std::make_unique<Game>(_system.get(), _resources.get());

	// Frontend Game Modules
	_main_menu = std::make_unique<MainMenu>(_system.get(), _display.get(),
											_ui.get(), _controller.get());
	_splash = std::make_unique<Splash>(_system.get(), _display.get(), _ui.get(),
									   _controller.get());

	// Castle/Town/etc Modules
	_castle = std::make_unique<Castle>(_system.get(), _display.get(), _ui.get(),
									   _controller.get());
	_edge_of_town = std::make_unique<EdgeOfTown>(_system.get(), _display.get(),
												 _ui.get(), _controller.get());

	// Game Engine
	_engine = std::make_unique<Engine>(this, _system.get(), _display.get(),
									   _ui.get(), _controller.get());
}

auto Sorcery::Application::get_state() -> std::string {

	std::stringstream ss(std::ios::binary | std::ios::in | std::ios::out);
	{
		cereal::BinaryOutputArchive out_archive(ss);
		out_archive(_game, _controller);
	}

	std::string bin{ss.str()};
	std::vector<uint8_t> buffer(bin.begin(), bin.end());

	return _encode_base64(buffer);
}

auto Sorcery::Application::set_state(const std::string &state) -> void {

	auto buf{_decode_base64(state)};
	std::stringstream in(std::string(buf.begin(), buf.end()));
	cereal::BinaryInputArchive in_archive(in);
	in_archive(_game, _controller);

	_game->post_construct(_system.get(), _resources.get());
	_controller->post_construct(_system.get(), _display.get(),
								_resources.get());
}

auto Sorcery::Application::save_state(const std::string &screen,
									  const std::string &encoded_data,
									  const std::string &filename) -> bool {

	try {

		// Lambda for generating current UTC timestamp in ISO 8601 format
		auto make_timestamp_iso8601 = []() -> std::string {
			const auto now{std::chrono::system_clock::now()};
			const std::time_t t{std::chrono::system_clock::to_time_t(now)};
			const std::tm tm{*std::gmtime(&t)};
			std::ostringstream oss;
			oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
			return oss.str();
		};

		std::filesystem::path file_path{std::filesystem::current_path() /
										filename};
		std::ofstream file(file_path, std::ios::out | std::ios::trunc);

		if (!file.is_open()) {
			std::cerr << "Error: Could not open file for writing: " << file_path
					  << "\n";
			return false;
		}

		file << "# Sorcery Save File\n";
		file << "screen=" << screen << '\n';
		file << "version=" << SAVE_STATE_VERSION << '\n';
		file << "timestamp=" << make_timestamp_iso8601() << '\n';
		file << "---\n";
		file << encoded_data;
		file.close();

		if (!file) {
			std::cerr << "Error: Write failed for: " << file_path << "\n";
			return false;
		}

		std::cout << "✅ Saved encoded data to: " << file_path << "\n";
		return true;
	} catch (const std::exception &e) {

		std::cerr << "Exception while writing file: " << e.what() << "\n";
		return false;
	}
}
auto Sorcery::Application::load_state(const std::string &filename)
	-> std::pair<SaveHeader, std::string> {

	try {

		std::filesystem::path file_path{std::filesystem::current_path() /
										filename};

		if (!std::filesystem::exists(file_path)) {
			std::cerr << "Error: File does not exist: " << file_path << "\n";
			return {};
		}

		std::ifstream file(file_path, std::ios::in);
		if (!file.is_open()) {
			std::cerr << "Error: Could not open file for reading: " << file_path
					  << "\n";
			return {};
		}

		SaveHeader header{};
		std::string line;
		std::ostringstream data;

		// Parse header
		while (std::getline(file, line)) {
			if (line == "---")
				break;
			if (line.starts_with("version="))
				header.version = std::stoi(line.substr(8));
			else if (line.starts_with("screen="))
				header.screen = line.substr(7);
			else if (line.starts_with("timestamp="))
				header.timestamp = line.substr(10);
		}

		// Read Base64 data
		data << file.rdbuf();

		std::cout << "✅ Loaded encoded data from: " << file_path << '\n';

		return {header, data.str()};

	} catch (const std::exception &e) {
		std::cerr << "Exception while reading file: " << e.what() << "\n";
		return {};
	}
}

// Default Destructor
Sorcery::Application::~Application() {}

// Start the Game
auto Sorcery::Application::start() -> int {

	_ui->start();

	// Display Splash Screen (this will start loading resources)
	_splash->start();
	_splash->stop();

	// Start relevant animation worker threads
	_system->animation->refresh_colcyc();
	_system->animation->start_colcycl_th();
	_system->animation->refresh_wp();
	_system->animation->start_wp_th();

	// Now check for any command line parameters
	auto done{false};
	auto front_result{0};

	// Optionally skip images for speed to avoid loading them
	if (_check_param(NO_IMAGES_PARAM))
		_ui->images->show_images = false;

	if (_check_param(CONTINUE_GAME_PARAM) && _controller->has_saved_game()) {

		// _game is set by this
		_load_existing_game();

		// Castle
		front_result = _do_town(DEST_NONE);
		if (front_result == GO_TO_MAZE) {

			// Start the Maze
			front_result = _do_start_expedition(EXPEDITION_RESTART);
			done = true;
		}

	} else if (_check_param(QUICKSTART_PARAM)) {

		// _game is set by this
		_start_new_game(true);
		front_result = _do_town(DEST_NONE);
	} else if (_check_param(NEW_GAME_PARAM)) {

		// _game is set by this
		_start_new_game(false);
		front_result = _do_town(DEST_NONE);
	} else {

		const auto go_to_maze{_check_param(GO_TO_MAZE_PARAM)};
		const auto go_to_training{_check_param(GO_TO_TRAINING_PARAM)};

		int mm_what{};
		auto first_time{true};

		// Main Frontend Loop
		while (!done) {

			if (first_time && go_to_maze)
				mm_what = MAIN_MENU_NEW_GAME;
			else if (first_time && go_to_training) {
				if (_controller->has_saved_game())
					_load_existing_game();
				else
					_start_new_game(false);

			} else {
				mm_what = _main_menu->start();
				_main_menu->stop();
			}

			// If we want to load a game or start a new game
			if (mm_what == MAIN_MENU_NEW_GAME ||
				mm_what == MAIN_MENU_CONTINUE_GAME) {

				// Do the town/maze loop til we want to exit
				if (mm_what == MAIN_MENU_NEW_GAME)
					_start_new_game(true);
				else if (mm_what == MAIN_MENU_CONTINUE_GAME &&
						 _controller->has_saved_game())
					_load_existing_game();
				else if (mm_what == MAIN_MENU_EXIT_GAME)
					done = true;
				else if (mm_what == ABORT_GAME)
					done = true;
				else
					_start_new_game(false);

				if (!done) {

					int inner_what;
					while (!(inner_what == ABORT_GAME ||
							 inner_what == LEAVE_GAME)) {

						if (_check_param(GO_TO_MAZE_PARAM) && first_time) {
							inner_what = _do_start_expedition(EXPEDITION_START |
															  EXPEDITION_GOTO);
							first_time = false;
							continue;
						} else if (_check_param(GO_TO_TRAINING_PARAM) &&
								   first_time) {

							inner_what = _do_edge(GO_TO_TRAINING);
							first_time = false;
							continue;
						}

						inner_what = _do_town(DEST_NONE);
						if (_controller->has_flag("want_enter_maze"))
							inner_what = _do_start_expedition(EXPEDITION_START);
						else if (_controller->has_flag(
									 "want_restart_expedition"))
							inner_what =
								_do_restart_expedition(EXPEDITION_RESTART);
					}

					done = true;
				}
			} else if (mm_what == MAIN_MENU_EXIT_GAME || mm_what == ABORT_GAME)
				done = true;
		}
	}

	_ui->stop();
	return 0;
};

auto Sorcery::Application::_do_restart_expedition(const int mode) -> int {

	_game->restart_maze(_controller->get_character("restart"));
	auto what{_engine->start(_game.get(), mode)};
	_engine->stop();

	return what;
}

// Do Maze
auto Sorcery::Application::_do_start_expedition(const int mode) -> int {

	_game->enter_maze();
	auto what{_engine->start(_game.get(), mode)};
	_engine->stop();

	return what;
}

auto Sorcery::Application::_do_edge(const int mode) -> int {

	// Go to the Edge of Town, and optionally wait to do something
	auto done{false};
	while (!done) {

		// Limited selection here of choices here (for now) to cover just
		// the additional command line parameters used for shortcuts
		auto edge_what{_edge_of_town->start(_game.get(), mode)};
		_edge_of_town->stop();

		if (edge_what == EDGE_OF_TOWN_GO_TO_TRAINING)
			return EDGE_OF_TOWN_GO_TO_TRAINING;
		else if (_controller->has_flag("want_leave_game")) {
			_game->save_game();
			_controller->set_game(nullptr);
			return LEAVE_GAME;
		} else if (edge_what == RETURN_TO_TOWN) {
			continue;
		} else if (edge_what == ABORT_GAME)
			return ABORT_GAME;
	};

	return 0;
}

auto Sorcery::Application::_do_town(const int mode) -> int {

	// Go to the Castle, and then wait until we've got exit back
	auto done{false};
	while (!done) {

		auto castle_what{_castle->start(_game.get())};
		_castle->stop();

		if (castle_what == CASTLE_GO_TO_EDGE_OF_TOWN) {

			auto edge_what{_edge_of_town->start(_game.get(), mode)};
			_edge_of_town->stop();

			if (edge_what == EDGE_OF_TOWN_GO_TO_CASTLE)
				continue;
			else if (edge_what == EDGE_OF_TOWN_GO_TO_MAZE)
				return START_MAZE;
			else if (edge_what == RESTART_MAZE)
				return RESTART_MAZE;
			else if (edge_what == ABORT_GAME)
				return ABORT_GAME;
			else if (edge_what == EDGE_OF_TOWN_GO_TO_TRAINING)
				return EDGE_OF_TOWN_GO_TO_TRAINING;
			else if (_controller->has_flag("want_leave_game")) {
				_game->save_game();
				_controller->set_game(nullptr);
				return LEAVE_GAME;
			} else if (edge_what == RETURN_TO_TOWN) {
				continue;
			}

		} else if (castle_what == ABORT_GAME)
			return ABORT_GAME;
		else if (_controller->has_flag("want_leave_game")) {
			_game->save_game();
			return LEAVE_GAME;
		}

		return 0;
	}

	return 0;
}

auto Sorcery::Application::_load_existing_game() -> void {

	_controller->set_busy(true);
	_ui->display_refresh();
	_game->load_game();
	_controller->set_busy(false);
	_controller->set_game(_game.get());
}

auto Sorcery::Application::_start_new_game(const bool quickstart) -> void {

	_controller->set_busy(true);
	_ui->display_refresh();
	_game->wipe_data();
	_game->create_game();
	_game->save_game();
	_controller->set_busy(false);
	_controller->set_game(_game.get());

	if (quickstart)
		_add_quickstart_party();
}

auto Sorcery::Application::_add_quickstart_party() -> void {

	_game->state->clear_party();

	// Create a new random party of a random alignment
	using enum Enums::Character::Align;
	auto first{NO_ALIGN}, second{NO_ALIGN}, align{NO_ALIGN};
	if ((*_system->random)[Enums::System::Random::D2] == 1) {
		first = GOOD;
		second = EVIL;
	} else {
		first = EVIL;
		second = GOOD;
	}

	for (int i = 0; i < 12; i++) {
		auto pc{Character(_system.get(), _resources.get())};
		align = i > 5 ? first : second;

		switch (i % 6) {
			using enum Enums::Character::Align;
			using enum Enums::Character::Class;
		case 0:
			pc.create_class_alignment(FIGHTER, align);
			break;
		case 1:
			pc.create_class_alignment(FIGHTER, NEUTRAL);
			break;
		case 2:
			pc.create_class_alignment(THIEF, NEUTRAL);
			break;
		case 3:
			pc.create_class_alignment(PRIEST, align);
			break;
		case 4:
			pc.create_class_alignment(BISHOP, align);
			break;
		case 5:
			pc.create_class_alignment(MAGE, NEUTRAL);
			break;
		default:
			break;
		}

		pc.finalise();
		pc.set_stage(Enums::Character::Stage::COMPLETED);
		pc.inventory.clear();

		switch (pc.get_class()) { // NOLINT(clang-diagnostic-switch)
			using enum Enums::Character::Class;
			using enum Enums::Items::TypeID;
		case FIGHTER:
		case LORD:
		case SAMURAI:
			pc.inventory.add_type((*_resources->items)[LEATHER_ARMOR], true);
			pc.inventory.add_type((*_resources->items)[LONG_SWORD], true);
			break;
		case MAGE:
			pc.inventory.add_type((*_resources->items)[ROBES], true);
			pc.inventory.add_type((*_resources->items)[DAGGER], true);
			break;
		case PRIEST:
		case BISHOP:
			pc.inventory.add_type((*_resources->items)[ROBES], true);
			pc.inventory.add_type((*_resources->items)[STAFF], true);
			pc.inventory.add_type((*_resources->items)[AMULET_OF_WERDNA], true);
			break;
		case THIEF:
		case NINJA:
			pc.inventory.add_type((*_resources->items)[LEATHER_ARMOR], true);
			pc.inventory.add_type((*_resources->items)[SHORT_SWORD], true);
		default:
			break;
		}

		if (i < 6)
			pc.set_location(Enums::Character::Location::PARTY);
		else
			pc.set_location(Enums::Character::Location::TAVERN);
		auto char_id{_game->save_character(pc)};

		_game->characters[char_id] = pc;
		if (i < 6)
			_game->state->add_character_to_party(char_id);
	}

	_game->save_game();
}

auto Sorcery::Application::_continue_existing_game() -> int {

	// Placeholder
	return 0;
}

// Stop the Game
auto Sorcery::Application::stop() -> void {

	// Stop relevant animation worker threads
	_system->animation->stop_colcyc_th();
	_system->animation->stop_wp_th();
};

// Check for a command line parameter
auto Sorcery::Application::_check_param(std::string_view param) const -> bool {

	return std::ranges::any_of(_args.begin(), _args.end(),
							   [&](std::string_view arg) {
								   return arg == param;
							   });
}

// Get the Application Executable Path
auto Sorcery::Application::_get_exe_path() const -> std::string_view {

#ifdef __linux__

	// On Linux
	{
		char result[PATH_MAX];
		if (const ssize_t count{readlink("/proc/self/exe", result, PATH_MAX)};
			count != -1) {
			const char *path{dirname(result)};
			std::string_view base_path{path};
			return base_path;
		} else
			return "";
	}

#elif _WIN32

	// On Windows
	{
		std::vector<wchar_t> pathBuf;
		unsigned long copied{0};
		do {
			pathBuf.resize(pathBuf.size() + MAX_PATH);
			copied = GetModuleFileName(0, &pathBuf.at(0), pathBuf.size());
		} while (copied >= pathBuf.size());

		pathBuf.resize(copied);

		std::wstring path{pathBuf.begin(), pathBuf.end()};
		std::string base_path;
		std::transform(wide.begin(), wide.end(), std::back_inserter(base_path),
					   [](wchar_t c) {
						   return (char)c;
					   });

		return base_path;
	}
#endif
}

auto Sorcery::Application::_encode_base64(const std::vector<uint8_t> &data)
	-> std::string {

	const char base64_chars[]{
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/"};

	std::string out;
	int val{0};
	int valb{-6};
	for (uint8_t c : data) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back(base64_chars[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6)
		out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4)
		out.push_back('=');
	return out;
}

auto Sorcery::Application::_decode_base64(const std::string &s)
	-> std::vector<uint8_t> {

	const char base64_chars[]{
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/"};

	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++)
		T[base64_chars[i]] = i;

	std::vector<uint8_t> out;
	int val{0}, valb{-8};
	for (uint8_t c : s) {
		if (T[c] == -1)
			break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back(uint8_t((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}
