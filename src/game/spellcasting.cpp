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

#include "game/spellcasting.hpp"
#include "common/types.hpp"
#include "core/context.hpp"
#include "core/controller/controller.hpp"
#include "core/debug.hpp"
#include "core/random.hpp"
#include "core/resources.hpp"
#include "display/ui/popupmanager.hpp"
#include "display/ui/ui.hpp"
#include "game/game.hpp"
#include "magic/castcontext.hpp"
#include "resources/levelstore.hpp"
#include "resources/spellstore.hpp"
#include "types/meta.hpp"
#include "types/state.hpp"
#include <limits>
#include <magic/malor.hpp>
#include <string>
#include <string_view>
#include <tuple>

Sorcery::SpellCasting::SpellCasting(Context &ctx, Game &game)
	: _ctx{ctx},
	  _game{game} {}

Sorcery::SpellCasting::~SpellCasting() = default;

auto Sorcery::SpellCasting::begin(const Magic::CastRequest &request) -> Magic::CastPlan {

	const auto spell{_ctx.resources->spells->get(request.spell)};

	DEBUG_LOGF("Cast request: spell={} caster={} context={}", spell.name, request.caster_id,
			   enum_name(request.context));

	const auto requirement{_get_requirement(spell, request.context)};

	if (requirement == Magic::CastRequirement::NONE) {

		DEBUG_LOG("Resolving immediate spell");

		_resolve(request);

	} else {

		// MALOR commits the spell point when the  destination selector is opened
		using enum Enums::Magic::SpellID;
		using enum Magic::CastRequirement;
		if (request.spell == MALOR) {

			auto caster_it{_game.characters.find(request.caster_id)};

			if (caster_it == _game.characters.end())
				return {.request = request, .requirement = NONE};

			auto &caster{caster_it->second};

			if (!caster.magic().spend_spell_point(spell.type, spell.level))
				return {.request = request, .requirement = NONE};

			DEBUG_LOGF(
				"MALOR: spell point spent "
				"before destination selection");
		}

		_pending = request;
	}

	return {.request = request, .requirement = requirement};
}

auto Sorcery::SpellCasting::_get_requirement(const Spell &spell, const Enums::Magic::CastContext context) const
	-> Magic::CastRequirement {

	using enum Enums::Magic::CastContext;
	using enum Enums::Magic::SpellID;
	using enum Magic::CastRequirement;

	if (context != FIELD)
		return NONE;

	switch (spell.id) {

	case DIOS:
	case DIALKO:
	case DIAL:
	case LATUMOFIS:
	case DI:
	case DIALMA:
	case MADI:
	case KADORTO:
		return PARTY_MEMBER;

	case MALOR:
		return DESTINATION;

	case HAMAN:
	case MAHAMAN:
		return EFFECT;

	default:
		// Includess stuff like KANDI and DUMAPIC
		return NONE;
	}
}

auto Sorcery::SpellCasting::select_party_target(const unsigned int target_id) -> bool {

	if (!_pending)
		return false;

	auto request{*_pending};
	request.target_id = target_id;

	return _resolve(std::move(request));
}

auto Sorcery::SpellCasting::_resolve(Magic::CastRequest request) -> bool {

	using enum Enums::Magic::SpellID;

	const auto spell{_ctx.resources->spells->get(request.spell)};

	// Every spell needs a valid caster
	auto caster_it{_game.characters.find(request.caster_id)};
	if (caster_it == _game.characters.end())
		return false;

	auto &caster{caster_it->second};

	if (!caster.magic().can_cast(spell.type, static_cast<int>(spell.level)))
		return false;

	const auto can_continue = [&]() {
		return caster.magic().can_cast(spell.type, static_cast<int>(spell.level));
	};

	const auto spend = [&]() {
		return caster.magic().spend_spell_point(spell.type, spell.level);
	};

	const auto get_target = [&]() -> Character * {
		if (!request.target_id)
			return nullptr;

		auto it{_game.characters.find(*request.target_id)};

		return it != _game.characters.end() ? &it->second : nullptr;
	};

	DEBUG_LOGF("Resolving cast: spell={} caster={} context={}", spell.name, request.caster_id,
			   enum_name(request.context));

	switch (spell.id) {

	case DIOS: {

		auto *target{get_target()};
		if (!target)
			return false;

		if (!spend())
			return false;

		const auto before{target->get_current_hp()};
		const auto healing{_ctx.random->get(Enums::System::Random::D8)};

		target->heal(healing);

		const auto healed{target->get_current_hp() - before};

		DEBUG_LOGF("DIOS: caster={} target={} healed={} remaining={}", request.caster_id, *request.target_id, healed,
				   caster.magic().priest_current_spellpoints().at(spell.level));

		return can_continue();
	}

	case DIAL: {

		auto *target{get_target()};
		if (!target)
			return false;

		if (!spend())
			return false;

		const auto before{target->get_current_hp()};
		const auto healing{_ctx.random->get(Enums::System::Random::D8) * 2};

		target->heal(healing);

		const auto healed{target->get_current_hp() - before};

		DEBUG_LOGF("DIAL: caster={} target={} healed={} remaining={}", request.caster_id, *request.target_id, healed,
				   caster.magic().priest_current_spellpoints().at(spell.level));

		return can_continue();
	}

	case DIALMA: {

		auto *target{get_target()};
		if (!target)
			return false;

		if (!spend())
			return false;

		const auto before{target->get_current_hp()};
		const auto healing{_ctx.random->get(Enums::System::Random::D8) * 3};

		target->heal(healing);

		const auto healed{target->get_current_hp() - before};

		DEBUG_LOGF("DIALMA: caster={} target={} healed={} remaining={}", request.caster_id, *request.target_id, healed,
				   caster.magic().priest_current_spellpoints().at(spell.level));

		return can_continue();
	}

	case MADI: {

		using enum Enums::Character::Status;

		auto *target{get_target()};
		if (!target)
			return false;

		const auto status{target->get_status()};

		// Invalid target, but keep the selection modal open
		if (status == DEAD || status == ASHES || status == LOST)
			return true;

		if (!spend())
			return false;

		const auto before{target->get_current_hp()};
		const auto healing{target->get_max_hp() - target->get_current_hp()};

		target->heal(healing);
		target->set_status(OK);
		target->set_poisoned_rate(0);

		const auto healed{target->get_current_hp() - before};

		DEBUG_LOGF("MADI: caster={} target={} healed={} remaining={}", request.caster_id, *request.target_id, healed,
				   caster.magic().priest_current_spellpoints().at(spell.level));

		return can_continue();
	}

	case DIALKO: {

		auto *target{get_target()};
		if (!target)
			return false;

		if (target->get_status() != Enums::Character::Status::HELD)
			return true;

		if (!spend())
			return false;

		target->set_status(Enums::Character::Status::OK);

		return can_continue();
	}

	case LATUMOFIS: {

		auto *target{get_target()};
		if (!target)
			return false;

		if (!target->is_poisoned())
			return true;

		if (!spend())
			return false;

		target->set_poisoned_rate(0);

		return can_continue();
	}

	case DI: {

		using enum Enums::Character::Attribute;
		using enum Enums::Character::Status;
		using enum Enums::System::Random;

		auto *target{get_target()};
		if (!target)
			return false;

		if (target->get_status() != DEAD)
			return true;

		if (!spend())
			return false;

		const auto chance{target->get_spell_resurrection_chance()};

		const auto roll{_ctx.get_random(D100)};

		if (roll < chance) {

			target->set_status(OK);
			target->set_current_hp(1);
			target->adjust_attribute(VITALITY, -1);

			DEBUG_LOGF("DI succeeded: target={} roll={} chance={}", *request.target_id, roll, chance);

		} else {

			target->set_status(ASHES);

			DEBUG_LOGF("DI failed: target={} roll={} chance={}", *request.target_id, roll, chance);
		}

		return can_continue();
	}

	case KADORTO: {

		using enum Enums::Character::Status;
		using enum Enums::System::Random;

		auto *target{get_target()};
		if (!target)
			return false;

		const auto status{target->get_status()};

		if (status != DEAD && status != ASHES)
			return true;

		if (!spend())
			return false;

		const auto chance{target->get_spell_resurrection_chance()};

		const auto roll{_ctx.get_random(D100)};

		if (roll < chance) {

			target->set_status(OK);
			target->set_current_hp(target->get_max_hp());

			DEBUG_LOGF("KADORTO succeeded: target={} roll={} chance={}", *request.target_id, roll, chance);

		} else {

			target->set_status(LOST);
			target->set_current_hp(0);

			DEBUG_LOGF("KADORTO failed: target={} roll={} chance={}", *request.target_id, roll, chance);
		}

		return can_continue();
	}

		// No target required from here onwards

	case MILWA: {

		if (!spend())
			return false;

		const auto turns{15 + _ctx.get_random(Enums::System::Random::D15)};

		_game.state->set_lit(turns);

		DEBUG_LOGF("MILWA: light set for {} turns", turns);

		_game.save_game();

		return false;
	}

	case LOMILWA: {

		if (!spend())
			return false;

		_game.state->set_lit(32000);

		DEBUG_LOG("LOMILWA: light set for 32000 turns");

		_game.save_game();

		return false;
	}

	case LATUMAPIC: {

		if (!spend())
			return false;

		_game.state->set_latumapic(true);

		DEBUG_LOG("LATUMAPIC: enemy identification enabled");

		_game.save_game();

		return false;
	}

	case MAPORFIC: {

		if (!spend())
			return false;

		_game.state->set_maporfic(true);

		DEBUG_LOG("MAPORFIC: armour protection enabled");

		_game.save_game();

		return false;
	}

	case DUMAPIC: {
		const auto loc{_game.state->get_player_pos()};

		const auto facing{enum_name(_game.state->get_player_facing())};

		const auto depth{std::abs(_game.state->get_depth())};

		// TODO: put these into strings
		const auto text{
			std::format("PARTY LOCATION:\n\n"
						"THE PARTY IS FACING {}.\n\n"
						"YOU ARE {} SQUARES EAST AND\n"
						"{} SQUARES NORTH OF THE STAIRS,\n"
						"AND {} LEVELS BELOW IT.",
						facing, loc.x, loc.y, depth)};

		_ctx.ui->popup_manager->open_dialog("global:dialog_dumapic", Enums::Layout::DialogType::OK, text);

		return false;
	}

	case LOKTOFEIT: {

		using enum Enums::Character::Ability;
		using enum Enums::System::Random;

		if (!spend())
			return false;

		const auto chance{caster.abilities().at(LOKTOFELT_SUCCESS)};
		const int roll{_ctx.get_random(D100)};

		// Forget LOKTOFEIT here regardless of success
		caster.magic().forget_spell(LOKTOFEIT);

		if (roll >= chance) {

			DEBUG_LOGF("LOKTOFEIT failed: caster={} roll={} chance={}", request.caster_id, roll, chance);

			_ctx.ui->popup_manager->open_dialog("global:notice_oops", Enums::Layout::DialogType::OK,
												"LOKTOFEIT FAILS!");

			_game.save_game();

			return false;
		}

		DEBUG_LOGF("LOKTOFEIT succeeded: caster={} roll={} chance={}", request.caster_id, roll, chance);

		_ctx.ui->popup_manager->close();
		_ctx.controller->clear_modal_flags();
		_ctx.controller->request_back();
		_ctx.controller->set_flag("want_return_to_town");

		_game.save_game();

		return false;
	}

	case KANDI: {

		if (!spend())
			return false;

		_ctx.ui->popup_manager->open_dialog("global:dialog_kandi", Enums::Layout::DialogType::OK, _kandi_report());

		_game.save_game();

		return false;
	}

	default:

		DEBUG_LOGF("Spell resolution not implemented: {}", spell.name);

		return false;
	}
}

auto Sorcery::SpellCasting::cancel() -> void {

	_pending.reset();
}

auto Sorcery::SpellCasting::_kandi_report() const -> std::string {

	struct LocatedCharacter {
			const Character *character;
			int level_distance;
			int tile_distance;
	};

	const auto current_depth{_game.state->get_depth()};

	const auto current_loc{_game.state->get_player_pos()};

	std::vector<LocatedCharacter> characters;

	for (const auto &[id, character] : _game.characters) {

		if (character.get_location() != Enums::Character::Location::MAZE)
			continue;

		if (!character.depth || !character.coordinate)
			continue;

		const auto level_distance{std::abs(*character.depth - current_depth)};

		auto tile_distance{std::numeric_limits<int>::max()};

		if (*character.depth == current_depth) {

			tile_distance =
				std::abs(character.coordinate->x - current_loc.x) + std::abs(character.coordinate->y - current_loc.y);
		}

		characters.emplace_back(&character, level_distance, tile_distance);
	}

	std::ranges::sort(characters, {}, [](const LocatedCharacter &entry) {
		return std::tuple{entry.level_distance, entry.tile_distance, entry.character->get_name()};
	});

	// TODO: put these into strings
	if (characters.empty())
		return "NO OTHER CHARACTERS ARE IN THE MAZE";

	std::string result{"CHARACTERS IN THE MAZE\n\n"};

	for (const auto &entry : characters) {

		result += entry.character->get_name_status_and_loc_in_maze();

		result += '\n';
	}

	return result;
}

auto Sorcery::SpellCasting::resolve_malor(const Magic::MalorDestination &destination) -> Magic::MalorOutcome {

	using enum Magic::MalorOutcome;
	using enum Enums::Tile::Properties;

	_malor_outcome.reset();
	_malor_teleport.reset();

	if (!_pending || _pending->spell != Enums::Magic::SpellID::MALOR)
		return NONE;

	const auto current{_game.state->get_player_pos()};

	const auto current_depth{_game.state->get_depth()};

	const auto finish = [&](const Magic::MalorOutcome outcome) {
		_pending.reset();
		_malor_outcome = outcome;

		return outcome;
	};

	const auto wrap = [](const int value) {
		return ((value % 20) + 20) % 20;
	};

	const Coordinate target{wrap(current.x + destination.east), wrap(current.y + destination.north)};

	const auto target_depth{current_depth - destination.down};

	// Source anti-magic prevents escape
	const auto &current_tile{_game.state->level->at(current)};

	if (current_tile.is(ANTIMAGIC))
		return finish(BLOCKED);

	// Above castle
	if (target_depth > 0)
		return finish(MID_AIR);

	// TODO: will be Volcano in Wiz 3

	// Castle level
	if (target_depth == 0) {

		if (target == Coordinate{0, 0})
			return finish(CASTLE);

		return finish(MOAT);
	}

	// Beyond the dungeon entirely
	const auto target_level{_ctx.resources->levels->get(target_depth)};

	if (!target_level)
		return finish(INTO_ROCK);

	// Wizardry I B10 cannot be entered using MALOR
	if (target_depth == -10)
		return finish(BOUNCED);

	// Anti-magic destination
	const auto &target_tile{target_level->at(target)};

	if (target_tile.is(ANTIMAGIC))
		return finish(BOUNCED);

	// Valid dungeon teleport
	_malor_teleport = Magic::MalorTeleport{.depth = target_depth, .coordinate = target};

	return finish(DUNGEON);
}

auto Sorcery::SpellCasting::take_malor_outcome() -> std::optional<Magic::MalorOutcome> {

	auto result{std::move(_malor_outcome)};

	_malor_outcome.reset();

	return result;
}

auto Sorcery::SpellCasting::take_malor_teleport() -> std::optional<Magic::MalorTeleport> {

	auto result{std::move(_malor_teleport)};

	_malor_teleport.reset();

	return result;
}