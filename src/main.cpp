#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ButtonHandler.h"

#define tempSensor A0
#define iron 10

#define raiseButtonPin 9		// button 8 decrease (left button) button 2 increase (right button)
#define decreaseButtonPin 7 // pins 3 and 2 can be interrupts (edge detection)
#define resetButtonPin 8
#define ledPin 6
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
unsigned long startTime = 0;
const int buttonReleased = HIGH;
const int buttonPressed = LOW;
volatile bool heatingBannerVissibility = false; ///
int
		minTemp = 27,	 // Minimum aquired iron tip temp during testing (°C)
		maxTemp = 525, // Maximum aquired iron tip temp during testing (°C)
		minADC = 234,	 // Minimum aquired ADC value during minTemp testing
		maxADC = 733,	 // Maximum aquired ADC value during minTemp testing
		setTemp = 0,
		maxPWM = 255,			// Maximum PWM Power
		avgCounts = 5,		// Number of avg samples
		lcdInterval = 80, // LCD refresh rate (miliseconds)
		pwm = 0,					// System Variable
		tempRAW = 0,			// System Variable
		setTempAVG = 0,		// System Variable
		previewTemp = 0,
		counter = 0,				// System Variable
		previousMillis = 0; // System Variable
float
		currentTemp = 0.0, // System Variable
		store = 0.0,			 // System Variable
		knobStore = 0.0;	 // System Variable
#define OLED_RESET -1	 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ButtonHandler raiseButton(raiseButtonPin);
ButtonHandler decreaseButton(decreaseButtonPin);
ButtonHandler resetButton(resetButtonPin);
void setup()
{
	Serial.begin(9600);
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for (;;)
			; // Don't proceed, loop forever
	}

	pinMode(tempSensor, INPUT); // Set Temp Sensor pin as INPUT
	pinMode(iron, OUTPUT);			// Set MOSFET PWM pin as OUTPUT
	pinMode(A6, INPUT);					// Passthru Pin
	pinMode(ledPin, OUTPUT);
	display.clearDisplay();

	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE); //
	display.setCursor((SCREEN_WIDTH - 90) / 2, (SCREEN_HEIGHT) / 2);
	display.cp437(true);
	display.write("Soldering Iron");
	display.setCursor((SCREEN_WIDTH - 90) / 2, ((SCREEN_HEIGHT) / 2) + 10);
	display.println("FW: 0.5");

	display.display();
	delay(1000);
	display.clearDisplay();
	display.display();
}
void handleButtons()
{
	if (raiseButton.isPressed() && setTemp <= maxPWM)
	{
		setTemp++;
	}
	if (raiseButton.isDoubleClick())
	{
		Serial.println("Long");
	}
	if (decreaseButton.isPressed() == HIGH && setTemp >= 0)
	{
		setTemp--;
	}

	if (resetButton.isPressed())
	{
		setTemp = 0;
	}
}

void resetHeatingBanner()
{
	// Check if the variable is true
	if (heatingBannerVissibility)
	{
		// Wait for 2 seconds (2000 milliseconds)
		delay(2000);

		// After 2 seconds, turn the variable off
		heatingBannerVissibility = false;
	}
}
void loop()
{
	raiseButton.update();
	decreaseButton.update();
	resetButton.update();
	tempRAW = analogRead(tempSensor);																						 // Get analog value of temp sensor
	currentTemp = map(analogRead(tempSensor), minADC, maxADC, minTemp, maxTemp); // Sacle raw analog temp values as actual temp units
	handleButtons();
	if (currentTemp <= setTemp)
	{ // Turn on iron when iron temp is lower than preset temp
		pwm = maxPWM;
		startTime = millis();
	}
	else
	{
		pwm = 0;
	}
	analogWrite(iron, pwm);
	//--------Display Data--------//
	unsigned long currentMillis = millis(); // Use and aquire millis function instead of using delay
	if (currentMillis - previousMillis >= lcdInterval)
	{ // LCD will only display new data ever n milisec intervals
		previousMillis = currentMillis;
		display.clearDisplay();
		String sysInfo = "Profile: LED \nPower: " + String(setTemp);
		display.setCursor(0, 1);
		display.write("Soldering OS: 0.5");
		display.setCursor(0, 15);
		display.println(sysInfo);
		if (currentTemp <= setTemp)
		{
			heatingBannerVissibility = true;
		}

		if (heatingBannerVissibility)
		{
			unsigned long currentTime = millis();
			unsigned long elapsedTime = currentTime - startTime;
			String firingInfo = "Heating!\nTime:" + String(elapsedTime);
			display.setCursor(0, 40);
			display.println(firingInfo);
			digitalWrite(LED_BUILTIN, HIGH);
		}

		display.display();
	}
	delay(100);
}
