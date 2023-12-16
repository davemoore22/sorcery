
/*
 *  Visibility.hpp
 *  sfeMovie project
 *
 *  Copyright (C) 2010-2015 Lucas Soltic
 *  lucas.soltic@orange.fr
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */


#ifndef SFEMOVIE_VISIBILITY_HPP
#define SFEMOVIE_VISIBILITY_HPP

#include <SFML/System.hpp>

/** Define portable import / export macros
 */
#if !defined SFEMOVIE_STATIC
    #if defined(SFML_SYSTEM_WINDOWS) && defined(_MSC_VER)
        #ifdef SFE_EXPORTS
            /** From DLL side, we must export
             */
            #define SFE_API __declspec(dllexport)
        #else
            /** From client application side, we must import
             */
            #define SFE_API __declspec(dllimport)
        #endif

        /** For Visual C++ compilers, we also need to turn off this annoying C4251 warning.
         * You can read lots ot different things about it, but the point is the code will
         * just work fine, and so the simplest way to get rid of this warning is to disable it
         */
        #ifdef _MSC_VER
            #pragma warning(disable : 4251)
        #endif
    #else
        #define SFE_API
    #endif
#else
    /** Static library doesn't need DLL import/export
     */
    #define SFE_API
#endif

#endif
