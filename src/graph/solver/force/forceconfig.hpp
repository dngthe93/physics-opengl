/*
 * src/graph/solver/force/forceconfig.hpp
 * Copyright 2010 by Michal Nazarewicz <mina86@mina86.com>
 *               and Maciej Swietochowski
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FORCECONFIG_HPP
#define FORCECONFIG_HPP

#include "../../../ui/abstract-config.hpp"

namespace graph {

namespace solver {

namespace force {

struct Data : public ui::cfg::Data {
	ui::cfg::Real repulsionForce, attractionForce, hitForce, middleForce;
	ui::cfg::Real desiredDistance, damping, dt;
	ui::cfg::Bool attractionMultiply;

	virtual struct iterator items() const;

private:
	Data();

	friend struct ui::Config<Data>;

	Q_OBJECT
};

typedef ui::Config<Data> Config;

}

}

}

#endif // CONFIG_HPP
