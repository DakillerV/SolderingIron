#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define tempSensor A0
#define iron 10

#define button8Pin 8 // button 8 decrease (left button) button 2 increase (right button)
#define button2Pin 4 // pins 3 and 2 can be interrupts (edge detection)
#define button3Pin 13
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
unsigned long startTime = 0;
volatile int button8State = HIGH;
volatile int button2State = HIGH;
volatile int button3State = HIGH;
const int buttonReleased = HIGH;
const int buttonPressed = LOW;
int
	minTemp = 27,  // Minimum aquired iron tip temp during testing (°C)
	maxTemp = 525, // Maximum aquired iron tip temp during testing (°C)
	minADC = 234,  // Minimum aquired ADC value during minTemp testing
	maxADC = 733,  // Maximum aquired ADC value during minTemp testing
	setPower = 0,
	maxPWM = 255,	  // Maximum PWM Power
	avgCounts = 5,	  // Number of avg samples
	lcdInterval = 80, // LCD refresh rate (miliseconds)
	pwm = 0,		  // System Variable
	tempRAW = 0,	  // System Variable
	setTemp = 0,	  // System Variable
	setTempAVG = 0,	  // System Variable
	previewTemp = 0,
	counter = 0,		// System Variable
	previousMillis = 0; // System Variable
float
	currentTemp = 0.0, // System Variable
	store = 0.0,	   // System Variable
	knobStore = 0.0;   // System Variable
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
	Serial.begin(115200);
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for (;;)
			; // Don't proceed, loop forever
	}

	pinMode(tempSensor, INPUT); // Set Temp Sensor pin as INPUT
	pinMode(button8Pin, INPUT);
	pinMode(button2Pin, INPUT);
	pinMode(button3Pin, INPUT);
	pinMode(iron, OUTPUT); // Set MOSFET PWM pin as OUTPUT
	pinMode(A6, INPUT);	   // Passthru Pin
	display.clearDisplay();

	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE); //
	display.setCursor((SCREEN_WIDTH - 90) / 2, (SCREEN_HEIGHT) / 2);
	display.cp437(true);
	display.write("Soldering Iron");
	display.setCursor((SCREEN_WIDTH - 90) / 2, ((SCREEN_HEIGHT) / 2) + 10);
	display.println("FW:0.1");

	display.display();
	delay(1000);
	display.clearDisplay();
	display.display();
}
void loop()
{
	tempRAW = analogRead(tempSensor);											 // Get analog value of temp sensor
	currentTemp = map(analogRead(tempSensor), minADC, maxADC, minTemp, maxTemp); // Sacle raw analog temp values as actual temp units

	button8State = digitalRead(button8Pin);
	button2State = digitalRead(button2Pin);
	button3State = digitalRead(button3Pin);
	if (button8State == 1 && setPower <= maxPWM)
	{
		Serial.println("Up");
		setPower++;
	}
	else if (setPower >= maxPWM)
	{
		setPower = maxPWM;
	}
	if (button2State == 1 && setPower >= 0)
	{
		Serial.println("Down");
		setPower--;
	}
	else if (setPower <= 0)
	{
		setPower = 0;
	}

	if (currentTemp <= setPower)
	{ // Turn on iron when iron temp is lower than preset temp
		pwm = maxPWM;
		startTime = millis();
	}
	else
	{
		pwm = 0;
	}

	if (button3State)
	{
		pwm = 0;
	}
	analogWrite(iron, pwm);
	//--------Display Data--------//
	unsigned long currentMillis = millis(); // Use and aquire millis function instead of using delay
	if (currentMillis - previousMillis >= lcdInterval && button8State != 1 && button2State != 1)
	{ // LCD will only display new data ever n milisec intervals
		previousMillis = currentMillis;
		display.clearDisplay();
		String sysInfo = "Profile: LED \n   Power: " + String(setPower);
		display.setCursor(10, 1);
		display.write("Soldering OS: 0.1");
		display.setCursor(10, 15);
		display.println(sysInfo);
		if (currentTemp <= setPower)
		{
			unsigned long currentTime = millis();
			unsigned long elapsedTime = currentTime - startTime;
			String firingInfo = "Heating!\n  Time:" + String(elapsedTime);
			display.setCursor(10, 40);
			display.println(firingInfo);
		}

		display.display();
	}
	delay(100);
}
