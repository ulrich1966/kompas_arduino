#include <Arduino.h>
class Point{
	private:
		int x = 0;
		int y = 0;

	public:
		Point(int x, int y);
		void setX(int x);
		void setY(int y);
		int getX();
		int getY();
};
