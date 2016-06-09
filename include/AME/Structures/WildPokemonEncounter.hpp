//////////////////////////////////////////////////////////////////////////////////
//
//
//                     d88b         888b           d888  888888888888
//                    d8888b        8888b         d8888  888
//                   d88''88b       888'8b       d8'888  888
//                  d88'  '88b      888 '8b     d8' 888  8888888
//                 d88Y8888Y88b     888  '8b   d8'  888  888
//                d88""""""""88b    888   '8b d8'   888  888
//               d88'        '88b   888    '888'    888  888
//              d88'          '88b  888     '8'     888  888888888888
//
//
// AwesomeMapEditor: A map editor for GBA Pokémon games.
// Copyright (C) 2016 Diegoisawesome, Pokedude
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////////


#ifndef __AME_WILDPOKEMONENCOUNTER_HPP__
#define __AME_WILDPOKEMONENCOUNTER_HPP__


///////////////////////////////////////////////////////////
// Include files
//
///////////////////////////////////////////////////////////
#include <QBoy/Config.hpp>


namespace ame
{
    ///////////////////////////////////////////////////////////
    /// \file    WildPokemonEncounter.hpp
    /// \author  Pokedude
    /// \version 1.0.0.0
    /// \date    6/4/2016
    /// \brief   Contains one Pokémon encounter.
    ///
    ///////////////////////////////////////////////////////////
    struct WildPokemonEncounter
    {
        ///////////////////////////////////////////////////////////
        /// \brief Holds the minimum appearance level.
        ///
        ///////////////////////////////////////////////////////////
        UInt8 min;

        ///////////////////////////////////////////////////////////
        /// \brief Holds the maximum appearance level.
        ///
        ///////////////////////////////////////////////////////////
        UInt8 max;

        ///////////////////////////////////////////////////////////
        /// \brief Holds the ID of the Pokémon to appear.
        ///
        ///////////////////////////////////////////////////////////
        UInt16 id;
    };
}


#endif // __AME_WILDPOKEMONENCOUNTER_HPP__
