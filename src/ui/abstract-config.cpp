/*
 * src/gl/config.cpp
 * Copyright 2010 by Michal Nazarewicz    <mina86@mina86.com>
 *               and Maciej Swietochowski <m@swietochowski.eu>
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
#include "abstract-config.hpp"

namespace ui {

namespace cfg {

Value::~Value() {
}

void Bool::set(bool v) {
	if (v != value) {
		value = v;
		emit changed(v);
		emit Value::changed(*this);
	}
}

Integer::Integer(const char *theName,
                 value_type theMin, value_type theMax, value_type v)
	throw(std::out_of_range) : Value(theName, this), min(theMin), max(theMax) {
	if (theMin > theMax || v < theMin || v > theMax) {
		throw std::out_of_range("invalid range or value out of range");
	}
	value = v;
}

Integer::Integer(const char *theName,
                 value_type theMin, value_type theMax)
	throw(std::out_of_range) : Value(theName, this), min(theMin), max(theMax) {
	if (theMin > theMax) {
		throw std::out_of_range("invalid range");
	}
	value = theMin;
}

void Integer::set(value_type v) throw(std::out_of_range) {
	if (v != value) {
		if (v < min || v > max) {
			throw std::out_of_range("value out of range");
		}
		value = v;
		emit changed(v);
		emit Value::changed(*this);
	}
}

Real::Real(const char *theName,
           value_type theMin, value_type theMax, value_type v)
	throw(std::out_of_range)
	: Value(theName, this), min(theMin), max(theMax) {
	if (theMin > theMax || v < theMin || v > theMax) {
		throw std::out_of_range("invalid range or value out of range");
	}
	value = v;
}

Real::Real(const char *theName, value_type theMin, value_type theMax)
	throw(std::out_of_range)
	: Value(theName, this), min(theMin), max(theMax) {
	if (theMin > theMax) {
		throw std::out_of_range("invalid range");
	}
	value = theMin;
}

void Real::set(value_type v) throw(std::out_of_range) {
	if (v != value) {
		if (v < min || v > max) {
			throw std::out_of_range("value out of range");
		}
		value = v;
		emit changed(v);
		emit Value::changed(*this);
	}
}

static List::value_type count(const List::Item *items)
	throw(std::out_of_range) {
	const List::Item *it = items;
	while (*it) {
		++it;
	}
	if (it - items < 2) {
		throw std::out_of_range("at least two items required");
	}
	return it - items;
}

List::List(const char *theName, const Item *theItems, value_type v)
	throw(std::out_of_range)
	: Integer(theName, 0, count(theItems) - 1, v), items(theItems) { }

Data::~Data() { }

void Data::init() {
	for (iterator it = items(); it; ++it) {
		connect(&it(this), SIGNAL(changed(const Value&)),
		        this, SLOT(valueChanged(const Value&)));
	}
}

void Data::valueChanged(const Value &) {
	emit changed();
}

}

}