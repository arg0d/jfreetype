
class Vector2 {

public:
	Vector2() { x = 0; y = 0; }
	Vector2(float x, float y) { this->x = x; this->y = y; }

	float x, y;

	int GetX() const { return (int) x; }
	int GetY() const { return (int) y; }

	bool IsNull() { return x == 0 && y == 0; }

};