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

		// DIOS healing roll here.
		const auto healing{_ctx.random->get(Enums::System::Random::D165)};
		target.heal(healing);

		const auto healed{target.get_current_hp() - before};
		DEBUG_LOGF("DIOS: caster={} target={} healed={} remaining={}", request.caster_id, *request.target_id, healed,
				   caster.magic().priest_current_spellpoints().at(spell.level));

		return caster.magic().can_cast(spell.type, static_cast<int>(spell.level));
	}

	default:
		DEBUG_LOGF("Spell resolution not implemented: {}", spell.name);
		return false;
	}
}

auto Sorcery::SpellCasting::cancel() -> void {

	_pending.reset();
}