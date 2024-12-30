#include "Arduino.h"
#include "U8g2lib.h"
#include "Point.h"
#include "Switch.h"

#define POT 0
#define PLUS 11
#define MINUS 12

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

const double oneGrad = 360.0 / 1023;
const double oneMin = 60.0 / 100.0;
const int x_mid = 128 / 2;
const int y_mid = 64 / 2;
const int length = 34;

static int dist = 6;
static int mem = 0;
static int stepOffset = 0;

boolean refresh = false;
boolean offsetLow = false;
Switch interrupt = Switch(13);

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
	return Point(x, y);
}

void printRose() {
	u8g2.drawStr(x_mid - 2, 10, "N");
	u8g2.drawStr(x_mid + 23, y_mid + 5, "O");
	u8g2.drawStr(x_mid - 2, y_mid + 32, "S");
	u8g2.drawStr(x_mid - 28, y_mid + 5, "W");

	u8g2.setFont(u8g2_font_4x6_tf);

	u8g2.drawStr(x_mid + 16, y_mid - 16, "NO");
	u8g2.drawStr(x_mid + 17, y_mid + 19, "SO");
	u8g2.drawStr(x_mid - 24, y_mid + 20, "SW");
	u8g2.drawStr(x_mid - 23, y_mid - 15, "NW");

	u8g2.setFont(u8g2_font_6x13_tf);
	u8g2.drawCircle(x_mid, y_mid, 20, U8G2_DRAW_ALL);
}

int writeOut(double value, int offset) {
	double decVal = oneGrad * value;
	int deg = (oneGrad * value) + offset;
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

void test() {
	int degs[] = { 280, 290, 300, 310, 320, 330, 340, 350, 360 };
	for (int deg : degs) {
		Point point = caldulatePos(deg);
		Serial.print(point.getX()), Serial.print(" : "), Serial.println(point.getY());
		drawCompassRose(point);
	}
	Serial.println("-----------------");
}

int offset() {
	Serial.print("war LOW: "), Serial.print(offsetLow), Serial.print(" Plus: "), Serial.print(digitalRead(PLUS)), Serial.print(" Minus: "), Serial.print(digitalRead(MINUS));

	if(digitalRead(PLUS) == LOW && offsetLow){
		stepOffset = stepOffset + 1;
		offsetLow = false;
	}

	if(digitalRead(MINUS) == LOW && offsetLow){
		stepOffset = stepOffset - 1;
		offsetLow = false;
	}

	if(digitalRead(PLUS) == HIGH && digitalRead(MINUS) == HIGH){
		offsetLow = true;
	}

	Serial.print(" offset value: "), Serial.println(stepOffset);
	return stepOffset;
}

void loop() {

	if (interrupt.toggle()) {
		refresh = false;
		int rawVal = analogRead(POT);
		int deg = writeOut(rawVal, offset());
		if (refresh) {
			if(deg == 0 || deg == 360)
				stepOffset = 0;
			Point point = caldulatePos(deg);
			drawCompassRose(point);
		}
		delay(500);
	}
}
