
class Vector2 {

public:
	Vector2() { x = 0; y = 0; }
	Vector2(float x, float y) { this->x = x; this->y = y; }

	float x, y;

	int GetX() { return (int) x; }
	int GetY() { return (int) y; }

	bool IsNull() { return x == 0 && y == 0; }

};