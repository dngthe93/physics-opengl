#include "data-loader.hpp"

#include <assert.h>
#include <stdio.h>
#include <math.h>

#include <stdexcept>
#include <vector>
#include <string>

#include "object.hpp"
#include "lexer.hpp"


namespace mn {

namespace physics {



static void autoVelocity(Object &object, const char *name) {
	const Object *const o = object.find(name);
	delete[] name;
	if (!o) {
		return;
	}

	const Object::Vector r = o->getPosition() - object.getPosition();
	const float l2 = r.length2();
	if (l2 < 0.01) {
		return;
	}

	const float V2 = Object::G * o->getMass() / sqrtf(l2);
	object.getVelocity().normalize();
	object.setVelocity(object.getVelocity() * sqrt(V2));
}


Object *loadData(const char *filename) {
	Lexer lexer(filename);
	if (!lexer) {
		fprintf(stderr, "%s: could not open\n", filename);
		return 0;
	}

	enum State {
		S_START = 0,
		S_CONT,
		S_POSITION,         /* takes 3 floats */
		S_POSITION_READ_1,
		S_POSITION_READ_2,
		S_VELOCITY,         /* takes 3 floats */
		S_VELOCITY_READ_1,
		S_VELOCITY_READ_2,
		S_VELOCITY_DONE,
		S_COLOR,            /* takes 3 floats */
		S_COLOR_READ_1,
		S_COLOR_READ_2,
		S_FACTOR
	};
	unsigned state = S_START;
	Object *object = 0;

	Lexer::Value value;
	Lexer::Location location;
	int token, lights = 0;
	float x, y;

	float massFactor = 1, sizeFactor = 1, distFactor = 1, velFactor = 1;

	for(;;) {
		token = lexer.nextToken(value, location);

		switch ((enum State)state) {
		case S_START:
			if (token == '*') { state = S_FACTOR; break; }
			if (token != Lexer::T_STRING) goto error;
			goto s_cont_string;

		case S_VELOCITY_DONE:
			state = S_CONT;
			if (token == Lexer::T_AUTO) {
				token = lexer.nextToken(value, location);
				if (token != Lexer::T_STRING) goto error;
				autoVelocity(*object, value.string);
				break;
			}
			/* FALL THROUGH */

		case S_CONT:
			switch (token) {

			s_cont_string:
				state = S_CONT;
			case Lexer::T_STRING:
				object = new Object(value.string, object);
				delete[] value.string;
				break;

			case '*'              : state = S_FACTOR  ; break;
			case '@'              : state = S_POSITION; break;
			case Lexer::T_VELOCITY: state = S_VELOCITY; break;
			case Lexer::T_COLOR   : state = S_COLOR   ; break;

			case 'x':
				token = lexer.nextToken(value, location);
				if (token != Lexer::T_REAL) goto error;
				object->getPosition().x =value.real * distFactor;
				break;

			case 'y':
				token = lexer.nextToken(value, location);
				if (token != Lexer::T_REAL) goto error;
				object->getPosition().y =value.real * distFactor;
				break;

			case 'z':
				token = lexer.nextToken(value, location);
				if (token != Lexer::T_REAL) goto error;
				object->getPosition().z =value.real * distFactor;
				break;

			case Lexer::T_FROZEN:
				if (object->isFrozen()) goto error;
				object->setFrozen(true);
				break;

			case Lexer::T_LIGHT:
				if (object->getLight() >= 0) goto error;
				object->setLight(++lights);
				break;

			case Lexer::T_SIZE:
				token = lexer.nextToken(value, location);
				if (token != Lexer::T_REAL) goto error;
				object->setSize(value.real * sizeFactor);
				break;

			case Lexer::T_MASS:
				token = lexer.nextToken(value, location);
				if (token != Lexer::T_REAL) goto error;
				object->setMass(value.real * massFactor);
				break;

			case Lexer::T_TEXTURE:
				if (object->texture) goto error;
				token = lexer.nextToken(value, location);
				if (token != Lexer::T_STRING) goto error;
				object->texture.load(value.string);
				object->colorFromTexture();
				delete[] value.string;
				break;

			case Lexer::T_EOF:
				return object;

			default:
				goto error;
			}
			break;

		case S_VELOCITY:
		case S_POSITION:
		case S_COLOR:
			if (token != Lexer::T_REAL) goto error;
			x = value.real;
			++state;
			break;

		case S_POSITION_READ_1:
		case S_VELOCITY_READ_1:
		case S_COLOR_READ_1:
			if (token != Lexer::T_REAL) goto error;
			y = value.real;
			++state;
			break;

		case S_POSITION_READ_2:
			if (token != Lexer::T_REAL) goto error;
			object->setPosition(x * distFactor, y * distFactor, value.real * distFactor);
			state = S_CONT;
			break;

		case S_VELOCITY_READ_2:
			if (token != Lexer::T_REAL) goto error;
			object->setVelocity(x * velFactor, y * velFactor, value.real * velFactor);
			state = S_VELOCITY_DONE;
			break;

		case S_COLOR_READ_2:
			if (token != Lexer::T_REAL) goto error;
			object->setColor(x, y, value.real);
			state = S_CONT;
			break;


		case S_FACTOR: {
			switch (token) {
			case Lexer::T_STRING  : goto s_cont_string;
			case '@'              :
			case Lexer::T_MASS    :
			case Lexer::T_SIZE    :
			case Lexer::T_VELOCITY: break;
			default: goto error;
			}
			int prevToken = token;
			token = lexer.nextToken(value, location);
			if (token != Lexer::T_REAL) goto error;
			switch (prevToken) {
			case '@'              : distFactor = value.real; break;
			case Lexer::T_MASS    : massFactor = value.real; break;
			case Lexer::T_SIZE    : sizeFactor = value.real; break;
			case Lexer::T_VELOCITY: velFactor  = value.real; break;
			default: assert(0);
			}
		}
			break;


		default:
			assert(0);
			break;
		}
	}




error:
	fprintf(stderr, "%s:%u:%u: unexpected token %s\n",
	        lexer.getFileName().c_str(),
	        location.begin.line, location.begin.column,
	        Lexer::tokenName(token));
	if (token == Lexer::T_STRING) {
		delete[] value.string;
	}
	return 0;
}


}

}
