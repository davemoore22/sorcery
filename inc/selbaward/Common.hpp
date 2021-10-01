//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
//
// Copyright(c) 2015-2020 M.J.Silk
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// M.J.Silk
// MJSilk2@gmail.com
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SELBAWARD_COMMON_HPP
#define SELBAWARD_COMMON_HPP

#include <exception>
#include <string>

#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define SELBAWARD_NOEXCEPT
#else
#define SELBAWARD_NOEXCEPT noexcept
#endif

namespace selbaward
{

class Exception : public std::exception
{
public:
	Exception(const std::string& errorMessage = "Unknown error.") :
		m_errorMessage("[Selba Ward] " + errorMessage)
	{
	}
	virtual const char* what() const SELBAWARD_NOEXCEPT override
	{
		return m_errorMessage.c_str();
	}

private:
	std::string m_errorMessage;
};

} // namespace selbaward

#ifndef SELBAWARD_NO_NAMESPACE_SHORTCUT
namespace sw = selbaward; // create shortcut namespace
#endif // SELBAWARD_NO_NAMESPACE_SHORTCUT

#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#if (SFML_VERSION_MAJOR == 2)
	#if (SFML_VERSION_MINOR < 4)
		#define USE_SFML_PRE_2_4
	#endif
#endif

#endif // SELBAWARD_COMMON_HPP
