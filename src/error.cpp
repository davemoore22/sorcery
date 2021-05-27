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

Sorcery::Error::Error(Enums::System::Error error_code, std::exception &exception, std::string notes)
	: _error_code{error_code}, _exception{exception}, _notes{notes} {

	_gui = std::nullopt;

	_timestamp = std::chrono::system_clock::now();

	_details.clear();
	_details.emplace_back(std::to_string(magic_enum::enum_integer(_error_code)));
	_details.emplace_back(magic_enum::enum_name(_error_code));
	_details.emplace_back(_exception.what());
	_details.emplace_back(get_when());
	_details.emplace_back(_notes);
}

Sorcery::Error::Error(
	tgui::Gui *gui, Enums::System::Error error_code, std::exception &exception, std::string notes)
	: _gui{gui}, _error_code{error_code}, _exception{exception}, _notes{notes} {
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
	title_text->setText(fmt::format("{:>5}: #{} - {}", "Error", _details[0], _details[1]));
	title_text->setPosition(16, 0);
	title_text->setTextSize(32);
	title_panel->add(title_text, "TitleText");

	auto what{tgui::Label::create()};
	what->setText(fmt::format("{:>5}: ", "What"));
	what->setPosition(16, 16);
	what->setTextSize(16);
	body_panel->add(what, "What");

	auto what_e{tgui::EditBox::create()};
	what_e->setText(_details[2]);
	what_e->setPosition(80, 10);
	what_e->setSize(528, 32);
	what_e->setTextSize(16);
	what_e->setReadOnly(true);
	body_panel->add(what_e, "WhatEdit");

	auto when{tgui::Label::create()};
	when->setText(fmt::format("{:>5}: ", "When"));
	when->setPosition(16, 48);
	when->setTextSize(16);
	body_panel->add(when, "When");

	auto when_e{tgui::EditBox::create()};
	when_e->setText(_details[3]);
	when_e->setPosition(80, 42);
	when_e->setSize(528, 32);
	when_e->setTextSize(16);
	when_e->setReadOnly(true);
	body_panel->add(when_e, "WhenEdit");

	auto info{tgui::Label::create()};
	info->setText(fmt::format("{:>5}: ", "Info"));
	info->setPosition(16, 80);
	info->setTextSize(16);
	body_panel->add(info, "Info");

	auto info_e{tgui::TextBox::create()};
	info_e->setPosition(80, 74);
	info_e->setSize(528, 272);
	info_e->setTextSize(16);
	info_e->setEnabled(true);
	info_e->setReadOnly(true);

	// Get the Stack Trace
	if (_details[4].size() == 0) {

		backward::StackTrace st;
		st.load_here(32);

		backward::TraceResolver tr;
		tr.load_stacktrace(st);

		for (size_t i = 0; i < st.size(); ++i) {
			backward::ResolvedTrace trace = tr.resolve(st[i]);

			info_e->addText(fmt::format(
				"#{} {} {} [{}]\n", i, trace.object_filename, trace.object_function, trace.addr));
		}

	} else {

		// Split the display lines
		std::string wrapped_notes = WORDWRAP(_details[4], 80);
		const std::regex regex(R"([@]+)");
		std::sregex_token_iterator it{wrapped_notes.begin(), wrapped_notes.end(), regex, -1};
		std::vector<std::string> lines{it, {}};
		lines.erase(std::remove_if(lines.begin(), lines.end(),
						[](std::string const &s) {
							return s.size() == 0;
						}),
			lines.end());
		for (auto line_of_text : lines) {
			info_e->addText(line_of_text);
		}
	}

	body_panel->add(info_e, "InfoEdit");

	auto close_button{tgui::Button::create()};
	close_button->setPosition(window->getSize().x - 115.f, window->getSize().y - 50.f);
	close_button->setText("Exit");
	close_button->setSize(100, 40);
	close_button->connect("pressed", [&]() {
		exit(-1);
	});
	close_button->setFocused(true);
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
