/*
 * src/graph/generator/ergenerator.hpp
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
#include "ergenerator.hpp"

#include "make-connected.hpp"

#include "../vector-rand.hpp"
#include "../../lib/rand.hpp"

namespace graph {

namespace generator {

namespace er {

Data::iterator Data::items() const {
	static const unsigned array[] = {
		CFG_DATA_OFFSET(nodesCount),
		CFG_DATA_OFFSET(p),
		CFG_DATA_OFFSET(radius),
		~0u,
	};
	return iterator(array);
}

Data::Data()
	: nodesCount("Number of nodes", 1, 10000, 20),
	  p("Edge probability", 0, 1, 0.3),
	  radius("Radius", 1, 1000, 30) {
	init();
}

}

ui::cfg::Data *ERGenerator::getConfigData() {
	return &*config;
}

ERGenerator::graph_ptr ERGenerator::generate() {
	unsigned nodesCount = (unsigned)config->nodesCount;
	float p = config->p, r = config->radius;

	graph_ptr g(new Graph(nodesCount));

	/* Randomize nodes */
	for (Graph::nodes_iterator i = g->nodes_begin(); i != g->nodes_end(); ++i) {
		randSpheric(*i, r);
	}

	/* Generate edges */
	Graph::edges_iterator it = g->edges_begin(), end = g->edges_end();
	for (; it != end; ++it) {
		*it = lib::rnd(1.0f) < p;
	}

	/* Make connected */
	makeConnected(*g.get());

	return g;
}

}

}
