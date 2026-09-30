#include "common/cereal.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "game/game.hpp"
#include "magic/enum.hpp"
#include "resources/define.hpp"
#include "resources/filestore.hpp"
#include "resources/savestore.hpp"
#include "resources/spellstore.hpp"
#include "types/character/create.hpp"
#include "types/character/magic.hpp"
#include "types/state.hpp"
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <system_error>

namespace {

using namespace Sorcery;

auto require(const bool condition, const char *message) -> void {
	if (!condition)
		throw std::runtime_error{message};
}

class TemporarySaves {
	public:
		TemporarySaves() {
			const auto id{GUID()};
			require(!id.empty(), "Could not generate a test directory ID");
			path = std::filesystem::temp_directory_path() / ("sorcery-save-test-" + id);
			require(std::filesystem::create_directory(path), "Test directory already exists");
		}
		~TemporarySaves() {
			std::error_code error;
			std::filesystem::remove_all(path, error);
		}
		TemporarySaves(const TemporarySaves &) = delete;
		auto operator=(const TemporarySaves &) -> TemporarySaves & = delete;
		std::filesystem::path path;
};

auto known_spells(const Sorcery::Character &character) -> std::map<Enums::Magic::SpellID, bool> {
	std::map<Enums::Magic::SpellID, bool> result;
	for (const auto &spell : character.magic().get_spells())
		result.emplace(spell.id, spell.known);
	return result;
}

auto snapshot(Game &game) -> std::string {
	std::ostringstream stream{std::ios::binary};
	{
		cereal::BinaryOutputArchive archive{stream};
		archive(game);
	}
	return stream.str();
}

auto check_restoration(Context &ctx, Game &game) -> void {
	game.hide_console();
	game.state->set_player_prev_depth(-1);
	game.state->set_player_pos({2, 3});
	game.state->set_lit(12);

	Sorcery::Character priest{&ctx};
	priest.create().create_class_alignment(Enums::Character::Class::PRIEST, Enums::Character::Align::GOOD);
	priest.create().finalise();
	priest.create().set_name("Restore test");
	priest.magic().forget_spell(Enums::Magic::SpellID::BADIOS);
	require(priest.magic().spend_spell_point(Enums::Magic::SpellType::DIVINE, 1),
			"Fixture priest cannot spend a point");
	const auto expected_spells{known_spells(priest)};
	const auto expected_points{priest.magic().priest_current_spellpoints()};
	require(expected_spells.at(Enums::Magic::SpellID::DIOS), "Fixture priest has not learned DIOS");
	require(!expected_spells.at(Enums::Magic::SpellID::BADIOS), "Fixture priest did not forget BADIOS");
	require(expected_spells.size() == ctx.resources->spells->get_all().size(), "Incomplete fixture catalog");

	const auto id{game.save_character(priest)};
	game.characters.insert_or_assign(id, priest);
	game.state->set_party({id});
	game.creation_candidate = std::make_shared<Sorcery::Character>(priest);
	const auto saved{snapshot(game)};
	game.characters.clear();
	game.creation_candidate.reset();
	game.state->set_lit(0);
	{
		std::istringstream stream{saved, std::ios::binary};
		cereal::BinaryInputArchive archive{stream};
		archive(game); // The production load boundary must restore runtime state itself.
	}

	require(snapshot(game) == saved, "Loading changed serialized data or the binary layout");
	require(game.state->get_party_characters() == std::vector<unsigned int>{id}, "Party was not restored");
	require(game.state->get_lit_turns() == 12, "Light duration was not restored");
	const auto &restored{game.characters.at(id)};
	require(known_spells(restored) == expected_spells, "Roster spell definitions/learned flags were not restored");
	require(restored.magic().priest_current_spellpoints() == expected_points, "Loading replenished spent points");
	require(known_spells(*game.creation_candidate) == expected_spells, "Creation candidate spells were not restored");
	require(restored.get_condition() == priest.get_condition(), "Restored character context is unusable");
	game.state->add_log_dice_roll("Restored context", 6, 2, 4);
	require(game.state->get_log_messages(1).back().text.find("Restored context") != std::string::npos,
			"Restored state context is unusable");

	// Rehydration is safe more than once and must not change the learned flags or points.
	game.post_construct(ctx);
	require(known_spells(restored) == expected_spells, "Repeated restoration changed learned spells");
	require(restored.magic().priest_current_spellpoints() == expected_points, "Repeated restoration changed points");

	// Normal XML-backed character loading uses the same restoration lifecycle.
	game.load_game();
	require(known_spells(game.characters.at(id)) == expected_spells, "Database loading lost spell definitions");
	require(game.characters.at(id).magic().priest_current_spellpoints() == expected_points,
			"Database loading changed remaining spell points");
}

}

int main() {
	try {
		TemporarySaves temporary;
		System system{0, nullptr}; // CTest selects SDL's dummy audio backend; no graphics are initialized.
		require(system.files != nullptr, "Headless system initialization failed");
		Context ctx{};
		ctx.system = &system;
		ctx.files = system.files.get();
		ctx.strings = system.strings.get();
		ctx.random = system.random.get();
		ctx.config = system.config.get();
		Resources resources{ctx};
		ctx.resources = &resources;
		// Runtime assets are read-only inputs. Every game/character write uses this unique directory.
		SaveStore saves{temporary.path / "game.json", temporary.path / "characters"};
		ctx.saves = &saves;
		Game game{ctx};
		ctx.game = &game;
		check_restoration(ctx, game);
		std::cout << "Binary quickload and XML character restoration checks passed\n";
	} catch (const std::exception &error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}
