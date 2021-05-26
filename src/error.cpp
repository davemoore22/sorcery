// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "error.hpp"

Sorcery::Error::Error(
	Enums::System::Error error_code, std::exception &exception, std::string notes = "")
	: _error_code{error_code}, _exception{exception} {

	_gui = std::nullopt;

	_timestamp = std::chrono::system_clock::now();

	_details.clear();
	_details.emplace_back(std::to_string(magic_enum::enum_integer(_error_code)));
	_details.emplace_back(magic_enum::enum_name(_error_code));
	_details.emplace_back(_exception.what());
	_details.emplace_back(get_when());
	_details.emplace_back(_notes);
}

Sorcery::Error::Error(tgui::Gui *gui, Enums::System::Error error_code, std::exception &exception,
	std::string notes = "")
	: _gui{gui}, _error_code{error_code}, _exception{exception} {
	_timestamp = std::chrono::system_clock::now();

	_details.clear();
	_details.emplace_back(std::to_string(magic_enum::enum_integer(_error_code)));
	_details.emplace_back(magic_enum::enum_name(_error_code));
	_details.emplace_back(_exception.what());
	_details.emplace_back(get_when());
	_details.emplace_back(_notes);

	(*_gui)->removeAllWidgets();

	auto background{tgui::Panel::create({"100%", "100%"})};
	background->getRenderer()->setBackgroundColor({0, 0, 0, 175});
	(*_gui)->add(background, "TransparentBackground");

	auto window{tgui::ChildWindow::create()};
	window->setTitle("Fatal Error!");
	window->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Center);
	window->setTitleButtons(0);
	window->setPosition({"28%", "15%"});
	window->setSize(640, 480);
	background->add(window, "Window");

	auto title_panel{tgui::Panel::create()};
	title_panel->setPosition(16, 16);
	title_panel->setSize(608, 64);
	window->add(title_panel, "TitlePanel");

	auto body_panel{tgui::Panel::create()};
	body_panel->setPosition(16, 82);
	body_panel->setSize(608, 348);
	window->add(body_panel, "BodyPanel");

	auto title_text{tgui::Label::create()};
	title_text->setText(fmt::format("{:>5}: {} - {}", "Error", _details[0], _details[1]));
	title_text->setPosition(16, 0);
	title_text->setTextSize(32);
	title_panel->add(title_text, "TitleText");

	auto what{tgui::Label::create()};
	what->setText(_details[2]);
	what->setPosition(16, 48);
	what->setTextSize(16);
	body_panel->add(what, "What");

	auto when{tgui::Label::create()};
	when->setText(_details[3]);
	when->setPosition(16, 96);
	when->setTextSize(16);
	body_panel->add(when, "When");

	auto info{tgui::Label::create()};
	info->setText(_details[4]);
	info->setPosition(16, 144);
	info->setTextSize(12);
	body_panel->add(info, "Info");

	auto close_button{tgui::Button::create()};
	close_button->setPosition(window->getSize().x - 115.f, window->getSize().y - 50.f);
	close_button->setText("Exit");
	close_button->setSize(100, 40);
	close_button->connect("pressed", [&]() {
		exit(-1);
	});
	window->add(close_button);

	auto copy_button{tgui::Button::create()};
	copy_button->setPosition(window->getSize().x - 230.f, window->getSize().y - 50.f);
	copy_button->setText("Copy");
	copy_button->setSize(100, 40);
	copy_button->connect("pressed", [&]() {
		sf::Clipboard::setString(_details[4]);
	});
	window->add(copy_button);
}

auto Sorcery::Error::get_code() -> Enums::System::Error {

	return _error_code;
}

auto Sorcery::Error::get_exception() -> std::exception & {

	return _exception;
}

auto Sorcery::Error::get_what() -> std::string {

	return std::string{_exception.what()};
}

auto Sorcery::Error::get_when() -> std::string {

	auto in_time_t{std::chrono::system_clock::to_time_t(_timestamp)};
	std::stringstream ss{};
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	return ss.str();
}

auto Sorcery::Error::get_notes() -> std::string {

	return _notes;
}

auto Sorcery::Error::get() -> std::vector<std::string> {

	return _details;
}

namespace Sorcery {

	auto operator<<(std::ostream &out_stream, const Sorcery::Error &error) -> std::ostream & {

		out_stream << fmt::format("{:>5}: {} - {}", "Error", error._details[0], error._details[1])
				   << std::endl;
		out_stream << fmt::format("{:>5}: {}", "What", error._details[2]) << std::endl;
		out_stream << fmt::format("{:>5}: {}", "When", error._details[3]) << std::endl;
		out_stream << fmt::format("{:>5}: {}", "Info", error._details[4]) << std::endl;

		return out_stream;
	}
} // namespace Sorcery