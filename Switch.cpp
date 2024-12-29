/**
 * Simuliert einen Schalter über einen Taster.
 * Beim ersten Tippen wird HIGH zurückgeben. Venn der Taster losgelassen wurde
 * und über LOW gegangen ist, dann wird beim zweiten Tippen LOW zurückgeben
 */

#include "Switch.h"

/**
 * Konstruktor -> Set up der Klasse
 */
Switch::Switch(int pin) {
	this->pin = pin;
	this->wasLow = true;
}

/**
 * Schaltet EIN oder AUS, je nach aktuellem Status.
 * EIN - wird zu AUS
 * AUS - wird zu EIN
 */
boolean Switch::toggle() {

	/**
	 * Wenn der Eingang HIGH ist, der Taster zwischenzeitlich gelöst wurde und der Merker 0 ist,
	 * wird der Merker auf 1 gesetzt und true zurückgeben, was dem Status AN entspricht.
	 */
	if (digitalRead(pin) == HIGH && wasLow && !mem) {
		wasLow = false;
		mem = 1;
	}

	/**
	 * Wenn der Eingang HIGH ist, der Taster zwischenzeitlich gelöst wurde und der Merker 1 ist,
	 * wird der Merker auf 0 gesetzt und false zurückgeben, was dem Status AUS entspricht.
	 */
	if (digitalRead(pin) == HIGH && wasLow && mem) {
		wasLow = false;
		mem = 0;
	}

	/**
	 * Da der Taster über mehrere loop - Durchläufe gedrückt bleiben kann, muss
	 * sichergestellt werden, dass er für das Toggeln zuvor auf LOW gewesen ist,
	 * also losgelassen wurde.
	 */
	if (digitalRead(pin) == HIGH) {
		wasLow = false;
	} else {
		wasLow = true;
	}

	return isOn();
}

/**
 * Abfrage ob der Schalter EIN ist
 */
boolean Switch::isOn() {
	if (mem == 1)
		return true;
	else
		return false;
}

/**
 * Abfrage ob der Schalter AUS ist
 */
boolean Switch::isOff() {
	if (mem == 0)
		return true;
	else
		return false;
}
