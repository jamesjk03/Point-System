#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>

using namespace std;

#pragma region vector_class

// custom vector class
class vector2 {
public:
	// x and y declaration
	float x;
	float y;

	// no parameters given
	vector2() {
		set(0, 0); // set to 0, 0
	}

	// parameters given
	vector2(float x_p, float y_p) {
		set(x_p, y_p); // set to given parameters
	}

	// set position with two params
	void set(float x_p, float y_p) {
		x = x_p;
		y = y_p;
	}

	// overload arithmetic operators (where applicable)

	vector2 operator+(vector2 const& v2) {
		return vector2(x + v2.x, y + v2.y); // vector addition = (x1 + x2, y1 + y2)
	}

	vector2 operator-(vector2 const& v2) {
		return vector2(x - v2.x, y - v2.y); // vector subtraction = (x1 - x2, y1 - y2)
	}

	vector2 operator*(float const& scale) {
		return vector2(x * scale, y * scale); // vector * float = (x * n, y * n)
	}

	vector2 operator/(float const& d) {
		return vector2(x / d, y / d); // vector / float = (x / n, y / n)
	}

	// gets magnitude of this vector
	float getMagnitude() {
		return powf(powf(x, 2) + powf(y, 2), 0.5); // pythaorean theorem to find magnitude of a vector
	}

	// resizes vector to certain magnitude
	vector2 rescale(float m) {
		float scale = m / getMagnitude(); // find ratio between current and desired magnitude

		return vector2(scale * x, scale * y); // multiply by ratio to get new vector
	}

	friend ostream& operator<<(ostream& os, const vector2& v2); // for outputting vector
};

// overload output operator for vectors
ostream& operator<<(ostream& os, const vector2& v2) {
	os << "(" << v2.x << ", " << v2.y << ")"; // output vector as (x, y)
	return os;
}

// linear interpolation function
vector2 lerp(vector2 v1, vector2 v2, float t) {
	return v1 + ((v2 - v1) * t); // linear interpolation
}

// finds distance between vectors in terms of another vector
vector2 vectorDistance(vector2 v1, vector2 v2) {
	return v2 - v1; // distance from v1 to v2 is v2 - v1
}

// finds distance between vectors in term of a float
float floatDistance(vector2 v1, vector2 v2) {
	// find distance in vector form
	vector2 vDistance = vectorDistance(v1, v2);

	// pythagorean theorem to find float distance
	float d = powf(vDistance.x, 2) + powf(vDistance.y, 2);
	d = powf(d, 0.5);

	return d;
}

#pragma endregion

#pragma region point_class

class point {
public:
	// declarations
	vector2 position;
	sf::CircleShape drawableObject;
	float radius;

	// parameters given
	point(vector2 p, float r) {
		position = p;
		radius = r;
		drawableObject.setRadius(radius); // set radius of sfml object (one time event)
		updateDrawableObjectPosition(); // updates sfml object position
	}

	// no parameters given
	point() {
		position = vector2(0, 0); // set (0,0)
		drawableObject.setRadius(4); // default radius of 4
		updateDrawableObjectPosition(); // updates sfml object position
	}

	// update sfml object's position to reflect point's position
	void updateDrawableObjectPosition() {
		drawableObject.setPosition(sf::Vector2f(position.x - radius, position.y - radius));
	}

	// draw to window (passed by reference)
	void draw(sf::RenderWindow* window) {
		updateDrawableObjectPosition(); // update drawableObject's position
		window->draw(drawableObject); // draw to passed in window
	}
};

#pragma endregion

#pragma region point_manager

class pointManager {
public:
	vector<point> points; // stores current points

	int clickRadius = 20;

	bool mouseDownLastFrame = false; // stores state of mouse button in previous frame
	bool pickup = false; // stores if point is currently picked up
	int currentDragIndex; // tracks which point is picked up, by index

	// to be called once per frame
	void step(sf::RenderWindow* window) {
		sf::Vector2i sfMousePosition = sf::Mouse::getPosition(*window); // get mouse position from sfml
		vector2 mousePosition = vector2(sfMousePosition.x, sfMousePosition.y); // convert mouse position to vector2 class
		bool leftMouseButtonDown = sf::Mouse::isButtonPressed(sf::Mouse::Left); // get lmb input


		// click actions

		// dragging when mouse down in current and previous frame as well as pickup in progress
		if (mouseDownLastFrame && leftMouseButtonDown && pickup) {
			points.at(currentDragIndex).position = mousePosition; // set position of point being dragged
		}

		// pick point up when lmb is down and wasn't last frame
		else if (leftMouseButtonDown && !mouseDownLastFrame) {
			vector<point> validPoints; // stores points within click radius
			vector<int> originalIndexes; // stores points index, respective to validPoints
			bool validSeen = false; // stores whether there are any points in click radius
			pickup = true; // resets pickup

			// loops through all points
			for (int i = 0; i < size(points); i++) {
				// checks if point within mouse click radius
				if (floatDistance(points.at(i).position, mousePosition) <= clickRadius) {
					validPoints.push_back(points.at(i)); // adds point to validPoints
					originalIndexes.push_back(i); // adds index from points to originalIndexes
					validSeen = true; // point within click radius seen
				}
			}

			// checks if there is at least one valid option
			if (validSeen) {
				float closestDistance = clickRadius + 1; // sets closest distance to be larger than click radius, means any points in validPoints will override it

				// loop through valid points
				for (int i = 0; i < size(validPoints); i++) {
					// checks if distance between current point and mouse is smaller than smallest distance found so far
					if (floatDistance(validPoints.at(i).position, mousePosition) < closestDistance) {
						closestDistance = floatDistance(validPoints.at(i).position, mousePosition); // sets closestDistance to new closest distance
						currentDragIndex = originalIndexes.at(i); // sets currentDragIndex to original index of point
					}
				}
				points.at(currentDragIndex).position = mousePosition; // move's point to mouse
			}
			// checks if no point within click radius
			else {
				pickup = false; // no pickup, stops drag
			}
		}
		// checks if neither last or current frame lmb
		else if (!mouseDownLastFrame && !leftMouseButtonDown) {
			int currentDragIndex; // resets drag index (not necessary but probably a good idea
		}

		mouseDownLastFrame = leftMouseButtonDown; // stores state of current frame in last frame before ticking over

		// draw window
		drawPoints(window);
	}

	void drawPoints(sf::RenderWindow* window) {
		// iterate vector
		for (int i = 0; i < size(points); i++) {
			points.at(i).draw(window); // call point draw function for vector element
		}
	}

	void addPoint(vector2 position, float r) {
		points.push_back(point(position, r)); // add point to vector
	}
};

#pragma endregion