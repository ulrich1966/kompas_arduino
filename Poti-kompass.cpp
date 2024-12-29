#include "Arduino.h"
#include "U8g2lib.h"
#include "Point.h"

#define POT 0

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

const double oneGrad = 360.0 / 1023;
const double oneMin = 60.0 / 100.0;
const int x_mid = 128 / 2;
const int y_mid = 64 / 2;
const int length = 36;

static int dist = 6;
static int mem = 0;
static boolean intr = false;
static boolean was_low = false;

boolean refresh = false;

void setup() {
	Serial.begin(115200);
	u8g2.begin();
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_6x13_tf);
	u8g2.sendBuffer();
}

void drawCompassRose(Point pos) {
	u8g2.drawLine(x_mid, y_mid, pos.getX(), pos.getY());
	u8g2.sendBuffer();
}

/**
 * Hier wird durch den Anstellwinkel und der Zeigerläng die Ankathete und die
 * Gegenkathete ermittelt und so die Endposition der Zeigerspitze errechnet, die
 * als Punkt zurückgegeben wird.
 */
Point caldulatePos(int value) {
	int hypo = length / 2;

	float rad = value * DEG_TO_RAD;
	float sinus = sin(rad);
	float cosinus = cos(rad);
	int gk = round(sinus * hypo);
	int ak = round(cosinus * hypo);

	Serial.print(value), Serial.print(" Sinus: "), Serial.print(sinus), Serial.print(" Gegenkathete: "), Serial.print(gk);
	Serial.print(" Kosinus: "), Serial.print(cosinus), Serial.print(" Ankathete: "), Serial.println(ak);

	int x = x_mid + gk;
	int y = y_mid - ak;
	Point point(x, y);

	//drawCompassRose(value);
	return point;
}

void printRose() {
	u8g2.setCursor(x_mid - 2, 10), u8g2.print("N");
	u8g2.setCursor(x_mid + 23, y_mid + 5), u8g2.print("O");
	u8g2.setCursor(x_mid - 2, y_mid + 32), u8g2.print("S");
	u8g2.setCursor(x_mid - 28, y_mid + 5), u8g2.print("W");

	u8g2.setFont(u8g2_font_4x6_tf);

	u8g2.setCursor(x_mid + 16, y_mid - 16),	u8g2.print("NO");
	u8g2.setCursor(x_mid + 17, y_mid + 19),	u8g2.print("SO");
	u8g2.setCursor(x_mid - 24, y_mid + 20),	u8g2.print("SW");
	u8g2.setCursor(x_mid - 23, y_mid - 15),	u8g2.print("NW");

	u8g2.setFont(u8g2_font_6x13_tf);
	u8g2.drawCircle(x_mid, y_mid, 20, U8G2_DRAW_ALL);
}

int writeOut(double value) {
	double decVal = oneGrad * value;
	int deg = oneGrad * value;
	int min = ((decVal - deg) * oneMin) * 100;
	int x = 0;
	int y = 10;
	refresh = deg >= (mem + 1) || deg <= (mem - 1);
	//refresh = true;

	if (refresh) {
		dist = 6;
		if (deg > 10) {
			dist = 12;
		}
		if (deg > 100) {
			dist = 18;
		}
		u8g2.clearBuffer();
		printRose();
		u8g2.setCursor(x, y);
		u8g2.print(deg);
		u8g2.drawUTF8(x + dist, y, "°");
		u8g2.setCursor(x + 110, 10);
		u8g2.print(min);
		u8g2.print("'");
		u8g2.sendBuffer();

		mem = deg;
	}

	return deg;
}

void interrupt(int sw) {
	if (!intr && sw == 1 && was_low) {
		Serial.println("enable interrupt: ");
		intr = true;
		was_low = false;
	}

	if (intr && sw == 1 && was_low) {
		Serial.println("disable interrupt: ");
		intr = false;
		was_low = false;
	}

	if (sw == 1) {
		was_low = false;
	} else {
		was_low = true;
	}
}

void test() {
	//int degs[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
	//int degs[] = {100, 110, 120, 130, 140, 150, 160, 170, 180};
	//int degs[] = {190, 200, 210, 220, 230, 240, 250, 260, 270};
	int degs[] = { 280, 290, 300, 310, 320, 330, 340, 350, 360 };
	for (int deg : degs) {
		Point point = caldulatePos(deg);
		Serial.print(point.getX()), Serial.print(" : "), Serial.println(point.getY());
		drawCompassRose(point);
	}
	Serial.println("-----------------");
}

void loop() {
	interrupt(digitalRead(13));
	if (!intr) {
		refresh = false;
		int rawVal = analogRead(POT);
		int deg = writeOut(rawVal);
		if (refresh) {
			//test();
			Point point = caldulatePos(deg);
			drawCompassRose(point);
		}
		delay(500);
	}
}
