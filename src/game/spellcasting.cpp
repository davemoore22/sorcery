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
#include "core/debug.hpp"
#include "core/random.hpp"
#include "core/resources.hpp"
#include "game/game.hpp"
#include "magic/castcontext.hpp"
#include "resources/spellstore.hpp"
#include "types/meta.hpp"

Sorcery::SpellCasting::SpellCasting(Context &ctx, Game &game)
	: _ctx{ctx},
	  _game{game} {}

Sorcery::SpellCasting::~SpellCasting() = default;
auto Sorcery::SpellCasting::begin(const Magic::CastRequest &request) -> Magic::CastPlan {

	const auto spell{_ctx.resources->spells->get(request.spell)};

	DEBUG_LOGF("Cast request: spell={} caster={} context={}", spell.name, request.caster_id,
			   enum_name(request.context));

	const auto requirement{_get_requirement(spell, request.context)};

	if (requirement != Magic::CastRequirement::NONE)
		_pending = request;

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

	if (!request.target_id)
		return false;

	auto caster_it{_game.characters.find(request.caster_id)};

	auto target_it{_game.characters.find(*request.target_id)};

	if (caster_it == _game.characters.end() || target_it == _game.characters.end())
		return false;

	auto &caster{caster_it->second};
	auto &target{target_it->second};

	if (!caster.magic().can_cast(spell.type, static_cast<int>(spell.level)))
		return false;

	switch (spell.id) {
	case DIOS: {
		if (!caster.magic().spend_spell_point(spell.type, spell.level))
			return false;

		const auto before{target.get_current_hp()};

		const auto healing{_ctx.random->get(Enums::System::Random::D8)};
		target.heal(healing);

		const auto healed{target.get_current_hp() - before};
		DEBUG_LOGF("DIOS: caster={} target={} healed={} remaining={}", request.caster_id, *request.target_id, healed,
				   caster.magic().priest_current_spellpoints().at(spell.level));

		return caster.magic().can_cast(spell.type, static_cast<int>(spell.level));
	} break;
	case DIAL: {

		if (!caster.magic().spend_spell_point(spell.type, spell.level))
			return false;

		const auto before{target.get_current_hp()};

		const auto healing{_ctx.random->get(Enums::System::Random::D8) * 2};
		target.heal(healing);

		const auto healed{target.get_current_hp() - before};
		DEBUG_LOGF("DIAL: caster={} target={} healed={} remaining={}", request.caster_id, *request.target_id, healed,
				   caster.magic().priest_current_spellpoints().at(spell.level));

		return caster.magic().can_cast(spell.type, static_cast<int>(spell.level));
	} break;
	case DIALMA: {
		if (!caster.magic().spend_spell_point(spell.type, spell.level))
			return false;

		const auto before{target.get_current_hp()};

		const auto healing{_ctx.random->get(Enums::System::Random::D8) * 3};
		target.heal(healing);

		const auto healed{target.get_current_hp() - before};
		DEBUG_LOGF("DIALMA: caster={} target={} healed={} remaining={}", request.caster_id, *request.target_id, healed,
				   caster.magic().priest_current_spellpoints().at(spell.level));

		return caster.magic().can_cast(spell.type, static_cast<int>(spell.level));
	} break;
	case MADI: {
		using enum Enums::Character::Status;
		const auto status{target.get_status()};
		if (status == DEAD || status == ASHES || status == LOST)
			return false;

		if (!caster.magic().spend_spell_point(spell.type, spell.level))
			return false;

		const auto before{target.get_current_hp()};

		const auto healing{target.get_max_hp() - target.get_current_hp()};
		target.heal(healing);

		const auto healed{target.get_current_hp() - before};

		target.set_current_hp(target.get_max_hp());
		target.set_status(OK);
		target.set_poisoned_rate(0);

		DEBUG_LOGF("MADI: caster={} target={} healed={} remaining={}", request.caster_id, *request.target_id, healed,
				   caster.magic().priest_current_spellpoints().at(spell.level));

		return caster.magic().can_cast(spell.type, static_cast<int>(spell.level));
	} break;
	case DIALKO:
		if (target.get_status() != Enums::Character::Status::HELD)
			return true;

		if (!caster.magic().spend_spell_point(spell.type, spell.level))
			return false;

		target.set_status(Enums::Character::Status::OK);
		return true;
		break;
	case LATUMOFIS:
		if (!target.is_poisoned())
			return true;

		if (!caster.magic().spend_spell_point(spell.type, spell.level))
			return false;

		target.set_poisoned_rate(0);
		return true;
		break;
	case DI: {

		using enum Enums::Character::Attribute;
		using enum Enums::Character::Status;
		using enum Enums::System::Random;

		if (target.get_status() != DEAD)
			return true;

		if (!caster.magic().spend_spell_point(spell.type, spell.level))
			return false;

		const auto chance{target.get_spell_resurrection_chance()};
		const auto roll{_ctx.get_random(D100)};

		if (roll < chance) {

			target.set_status(OK);
			target.set_current_hp(1);
			target.adjust_attribute(VITALITY, -1);

			DEBUG_LOGF("DI succeeded: target={} roll={} chance={}", *request.target_id, roll, chance);

		} else {

			target.set_status(ASHES);

			DEBUG_LOGF("DI failed: target={} roll={} chance={}", *request.target_id, roll, chance);
		}

		break;
	}
	case KADORTO: {

		using enum Enums::Character::Status;
		using enum Enums::System::Random;

		const auto status{target.get_status()};

		if (status != DEAD && status != ASHES)
			return true;

		if (!caster.magic().spend_spell_point(spell.type, spell.level))
			return false;

		const auto chance{target.get_spell_resurrection_chance()};
		const auto roll{_ctx.get_random(D100)};

		if (roll < chance) {

			target.set_status(OK);
			target.set_current_hp(target.get_max_hp());

			DEBUG_LOGF("KADORTO succeeded: target={} roll={} chance={}", *request.target_id, roll, chance);

		} else {

			target.set_status(LOST);
			target.set_current_hp(0);

			DEBUG_LOGF("KADORTO failed: target={} roll={} chance={}", *request.target_id, roll, chance);
		}

		break;
	}

	default:
		DEBUG_LOGF("Spell resolution not implemented: {}", spell.name);
		return false;
	}
	return false;
}

auto Sorcery::SpellCasting::cancel() -> void {

	_pending.reset();
}