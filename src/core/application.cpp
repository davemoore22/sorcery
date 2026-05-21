// Copyright (C) 2026 Dave Moore
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
#include "core/animation.hpp"
#include "core/audioplayer.hpp"
#include "core/controller.hpp"
#include "core/debug.hpp"
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
#include "resources/filestore.hpp"
#include "resources/imagestore.hpp"
#include "resources/itemstore.hpp"
#include "resources/levelstore.hpp"
#include "resources/monsterstore.hpp"
#include "types/config.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

#include <fstream>

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

	install_signal_handlers();

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

	// And the Context object used for DI
	ctx = Context{};
	ctx.application = this;

	// Set up all the Core Modules (and populate the DI helper as we go)
	_system = std::make_unique<System>(argc, argv);
	ctx.system = _system.get();
	ctx.animation = _system->animation.get();
	ctx.audio = _system->audio.get();
	ctx.config = _system->config.get();
	ctx.database = _system->db.get();
	ctx.files = _system->files.get();
	ctx.random = _system->random.get();
	ctx.strings = _system->strings.get();
	_resources = std::make_unique<Resources>(ctx);
	ctx.resources = _resources.get();
	_display = std::make_unique<Display>(ctx);
	ctx.display = _display.get();
	_controller = std::make_unique<Controller>(ctx);
	ctx.controller = _controller.get();
	_ui = std::make_unique<UI>(ctx);
	ctx.ui = _ui.get();
	ctx.menubuilder = _ui->menubuilder.get();
	ctx.components = _ui->components.get();
	ctx.images = _ui->images.get();
	ctx.fonts = _ui->fontstore.get();

	_game = std::make_unique<Game>(ctx);
	ctx.game = _game.get();

	// Frontend Game Modules
	_main_menu = std::make_unique<MainMenu>(ctx);
	_splash = std::make_unique<Splash>(ctx);

	// Castle/Town/etc Modules
	_castle = std::make_unique<Castle>(ctx);
	_edge_of_town = std::make_unique<EdgeOfTown>(ctx);

	// Game Engine
	_engine = std::make_unique<Engine>(ctx);
}

auto Sorcery::Application::save_state_to_binary(const std::string &filename)
	-> bool {

	std::ofstream os(filename, std::ios::binary);
	if (!os.is_open()) {
		std::cerr << "Error: could not open " << filename << " for writing.\n";
		return false;
	}

	// Note we serialize FROM existing objects thus no need to reinject after
	cereal::BinaryOutputArchive archive(os);
	archive(*_game, *_controller);

	DEBUG_LOGF("Quicksave successfully written to {}!", filename);

	return true;
}

auto Sorcery::Application::load_state_from_binary(const std::string &filename)
	-> bool {

	std::ifstream is(filename, std::ios::binary);
	if (!is.is_open()) {
		std::cerr << "Error: could not open " << filename << " for reading.\n";
		return false;
	}

	// Note we serialize INTO existing objects thus no need to reinject
	cereal::BinaryInputArchive archive(is);
	archive(*_game, *_controller);

	DEBUG_LOGF("Quicksave successfully loaded from {}!", filename);

	return true;
}

// Default Destructor
Sorcery::Application::~Application() {}

auto Sorcery::Application::get_resources() const -> Resources * {

	return ctx.resources;
}

// Start the Game
auto Sorcery::Application::start() -> int {

	ctx.ui->start();

	_splash->start();
	_splash->stop();

	ctx.animation->refresh_colcyc();
	ctx.animation->start_colcycl_th();
	ctx.animation->refresh_wp();
	ctx.animation->start_wp_th();

	ctx.audio->load(ctx.files->get(MAINMENU_MUSIC));
	ctx.audio->set_volume(0.0f);

	const auto plan{_build_startup_plan()};
	auto flow{_flow_from_startup_plan(plan)};

	while (flow != AppFlow::QUIT && flow != AppFlow::ABORT) {
		switch (flow) {
		case AppFlow::MAIN_MENU:
			flow = _run_main_menu();
			break;

		case AppFlow::NEW_GAME:
			_start_new_game(true);
			flow = AppFlow::TOWN;
			break;

		case AppFlow::CONTINUE_GAME:
			if (ctx.controller->has_saved_game())
				_load_existing_game();
			flow = AppFlow::TOWN;
			break;

		case AppFlow::TOWN:
			flow = _run_town();
			break;

		case AppFlow::MAZE:
			flow = _run_maze(EXPEDITION_START);
			break;

		case AppFlow::RESTART_MAZE:
			flow = _run_restart_maze(EXPEDITION_RESTART);
			break;

		case AppFlow::LEAVE_GAME:
			ctx.game->save_game();
			ctx.controller->set_game(nullptr);
			flow = AppFlow::MAIN_MENU;
			break;

		default:
			flow = AppFlow::ABORT;
			break;
		}
	}

	ctx.audio->stop();
	ctx.ui->stop();

	return 0;
}

auto Sorcery::Application::_run_town() -> AppFlow {

	ctx.audio->load(ctx.files->get(TOWN_MUSIC));
	ctx.audio->set_volume(0.0f);
	ctx.audio->play();

	while (true) {
		const auto castle_result{_castle->start()};
		_castle->stop();

		if (castle_result == ABORT_GAME)
			return AppFlow::ABORT;

		if (ctx.controller->want_to_leave_game())
			return AppFlow::LEAVE_GAME;

		if (castle_result != CASTLE_GO_TO_EDGE_OF_TOWN)
			continue;

		const auto edge_result{_edge_of_town->start(DEST_NONE)};
		_edge_of_town->stop();

		switch (edge_result) {
		case EDGE_OF_TOWN_GO_TO_CASTLE:
		case RETURN_TO_TOWN:
			continue;

		case EDGE_OF_TOWN_GO_TO_MAZE:
			return AppFlow::MAZE;

		case RESTART_MAZE:
			return AppFlow::RESTART_MAZE;

		case EDGE_OF_TOWN_GO_TO_TRAINING:
			return AppFlow::TRAINING;

		case ABORT_GAME:
			return AppFlow::ABORT;

		default:
			if (ctx.controller->want_to_leave_game())
				return AppFlow::LEAVE_GAME;

			continue;
		}
	}
}

auto Sorcery::Application::_flow_from_startup_plan(const StartupPlan &plan)
	-> AppFlow {

	if (!plan.bypass_menu)
		return AppFlow::MAIN_MENU;

	switch (plan.bootstrap) {
	case GameBootstrap::LOAD_GAME:
		if (!ctx.controller->has_saved_game())
			return AppFlow::MAIN_MENU;

		_load_existing_game();
		break;

	case GameBootstrap::NEW_GAME:
		_start_new_game(plan.party == PartyMode::QUICKSTART);
		break;

	default:
		return AppFlow::MAIN_MENU;
	}

	if (plan.location == StartLocation::MAZE)
		return AppFlow::MAZE;

	return AppFlow::TOWN;
}

auto Sorcery::Application::_run_maze(const int mode) -> AppFlow {

	ctx.game->enter_maze();

	ctx.audio->load(ctx.files->get(ENGINE_MUSIC));
	ctx.audio->set_volume(0.0f);
	ctx.audio->play();

	const auto result{_engine->start(mode)};
	_engine->stop();

	if (result == ABORT_GAME)
		return AppFlow::ABORT;

	if (result == LEAVE_GAME)
		return AppFlow::LEAVE_GAME;

	return AppFlow::TOWN;
}

auto Sorcery::Application::_run_restart_maze(const int mode) -> AppFlow {

	ctx.game->restart_maze(ctx.controller->get_character("restart"));

	ctx.audio->load(ctx.files->get(ENGINE_MUSIC));
	ctx.audio->set_volume(0.0f);
	ctx.audio->play();

	const auto result{_engine->start(mode)};
	_engine->stop();

	if (result == ABORT_GAME)
		return AppFlow::ABORT;

	if (result == LEAVE_GAME)
		return AppFlow::LEAVE_GAME;

	return AppFlow::TOWN;
}

auto Sorcery::Application::_build_startup_plan() -> StartupPlan {

	// Command-line parameters (debug-only)
	constexpr auto PARAM_NO_IMAGES{"--no-images"sv};
	constexpr auto PARAM_LOAD_GAME{"--load"sv};
	constexpr auto PARAM_NEW_GAME{"--new"sv};
	constexpr auto PARAM_QUICKSTART{"--quickstart"sv};
	constexpr auto PARAM_GO_TO_MAZE{"--go-to-maze"sv};

	const bool load_game{_check_param(PARAM_LOAD_GAME)};
	const bool new_game{_check_param(PARAM_NEW_GAME)};
	const bool quickstart{_check_param(PARAM_QUICKSTART)};
	const bool go_to_maze{_check_param(PARAM_GO_TO_MAZE)};

	// Independent global modifier
	if (_check_param(PARAM_NO_IMAGES))
		ctx.images->show_images = false;

	// Validate mutually exclusive bootstrap options
	const int bootstrap_count =
		(load_game ? 1 : 0) + (new_game ? 1 : 0) + (quickstart ? 1 : 0);

	if (bootstrap_count > 1)
		throw std::runtime_error("Invalid startup parameters");

	if (load_game && go_to_maze)
		throw std::runtime_error("--load cannot be combined with --go-to-maze");

	// Build the startup plan
	StartupPlan plan{};

	// Aliases / shortcuts (highest priority first)
	if (go_to_maze) {
		// Alias: new game + quickstart + go to maze
		plan.bypass_menu = true;
		plan.bootstrap = GameBootstrap::NEW_GAME;
		plan.party = PartyMode::QUICKSTART;
		plan.location = StartLocation::MAZE;

	} else if (quickstart) {
		// Alias: new game + quickstart party
		plan.bypass_menu = true;
		plan.bootstrap = GameBootstrap::NEW_GAME;
		plan.party = PartyMode::QUICKSTART;

	} else if (new_game) {
		plan.bypass_menu = true;
		plan.bootstrap = GameBootstrap::NEW_GAME;

	} else if (load_game) {
		plan.bypass_menu = true;
		plan.bootstrap = GameBootstrap::LOAD_GAME;
	}

	return plan;
}

auto Sorcery::Application::update() -> void {

	if (signal_shutdown_requested()) {
		ctx.controller->set_flag("want_abort_game");
		ctx.controller->set_flag("want_exit_game");
	}

	ctx.audio->update();
}

auto Sorcery::Application::_run_main_menu() -> AppFlow {

	ctx.audio->load(ctx.files->get(MAINMENU_MUSIC));
	ctx.audio->set_volume(0.0f);
	ctx.audio->play();

	const auto result{_main_menu->start()};
	_main_menu->stop();

	switch (result) {
	case MAIN_MENU_NEW_GAME:
		return AppFlow::NEW_GAME;

	case MAIN_MENU_CONTINUE_GAME:
		return AppFlow::CONTINUE_GAME;

	case MAIN_MENU_EXIT_GAME:
		return AppFlow::QUIT;

	case ABORT_GAME:
		return AppFlow::ABORT;

	default:
		return AppFlow::MAIN_MENU;
	}
}

auto Sorcery::Application::_do_restart_expedition(const int mode) -> int {

	ctx.game->restart_maze(ctx.controller->get_character("restart"));

	ctx.audio->load(ctx.files->get(ENGINE_MUSIC));
	ctx.audio->set_volume(0.0f);

	auto what{_engine->start(mode)};
	_engine->stop();

	return what;
}

// Do Maze
auto Sorcery::Application::_do_start_expedition(const int mode) -> int {

	ctx.game->enter_maze();
	ctx.audio->load(ctx.files->get(ENGINE_MUSIC));
	ctx.audio->set_volume(0.0f);
	auto what{_engine->start(mode)};
	_engine->stop();

	return what;
}

auto Sorcery::Application::_load_existing_game() -> void {

	ctx.controller->set_busy(true);
	ctx.ui->display_refresh();
	ctx.game->load_game();
	ctx.controller->set_busy(false);
	ctx.controller->set_game(ctx.game);
}

auto Sorcery::Application::_start_new_game(const bool quickstart) -> void {

	ctx.controller->set_busy(true);
	ctx.ui->display_refresh();
	ctx.game->wipe_data();
	ctx.game->create_game();
	ctx.game->save_game();
	ctx.controller->set_busy(false);
	ctx.controller->set_game(ctx.game);

	if (quickstart)
		_add_quickstart_party();
}

auto Sorcery::Application::_add_quickstart_party() -> void {

	ctx.game->state->clear_party();

	// Create a new random party of a random alignment
	using enum Enums::Character::Align;
	auto first{NO_ALIGN}, second{NO_ALIGN}, align{NO_ALIGN};
	if (ctx.get_random(Enums::System::Random::D2) == 1) {
		first = GOOD;
		second = EVIL;
	} else {
		first = EVIL;
		second = GOOD;
	}

	for (int i = 0; i < 12; i++) {
		auto pc{Character(&ctx)};
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
			pc.inventory.add_type(ctx.resources->items->get(LEATHER_ARMOR),
								  true);
			pc.inventory.add_type(ctx.resources->items->get(LONG_SWORD), true);
			break;
		case MAGE:
			pc.inventory.add_type(ctx.resources->items->get(ROBES), true);
			pc.inventory.add_type(ctx.resources->items->get(DAGGER), true);
			break;
		case PRIEST:
		case BISHOP:
			pc.inventory.add_type(ctx.resources->items->get(ROBES), true);
			pc.inventory.add_type(ctx.resources->items->get(STAFF), true);
			break;
		case THIEF:
		case NINJA:
			pc.inventory.add_type(ctx.resources->items->get(LEATHER_ARMOR),
								  true);
			pc.inventory.add_type(ctx.resources->items->get(SHORT_SWORD), true);
		default:
			break;
		}

		if (i < 6)
			pc.set_location(Enums::Character::Location::PARTY);
		else
			pc.set_location(Enums::Character::Location::TAVERN);
		auto char_id{ctx.game->save_character(pc)};

		ctx.game->characters.emplace(char_id, std::move(pc));
		if (i < 6)
			ctx.game->state->add_character_to_party(char_id);
	}

	ctx.game->save_game();
}

auto Sorcery::Application::_continue_existing_game() -> int {

	// Placeholder
	return 0;
}

// Stop the Game
auto Sorcery::Application::stop() -> void {

	// Stop relevant animation worker threads
	ctx.animation->stop_colcyc_th();
	ctx.animation->stop_wp_th();
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

auto Sorcery::Application::install_signal_handlers() -> void {

	std::signal(SIGTERM, _handle_signal);
	std::signal(SIGINT, _handle_signal);
}

auto Sorcery::Application::_handle_signal([[maybe_unused]] int signal) -> void {

	_signal_shutdown_requested.store(true, std::memory_order_relaxed);
}

auto Sorcery::Application::signal_shutdown_requested() -> bool {

	return _signal_shutdown_requested.load(std::memory_order_relaxed);
}