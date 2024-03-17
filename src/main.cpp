#include "config.h"
#include "secrets.h"
#include <stdio.h>
#include <TinyGsmClient.h>
#include <SSLClient.h>
#include <ArduinoHttpClient.h>
#include "certs.h"

int counter, lastIndex, numberOfPieces = 24;
String pieces[24], input;

// Your GPRS credentials, if any
const char apn[] = APN_NAME;
const char gprsUser[] = APN_USER;
const char gprsPass[] = APN_PASS;

HardwareSerial serialGsm(1);

TinyGsm modem(serialGsm);
TinyGsmClient client(modem);
SSLClient secure(&client);
HttpClient http(secure, HASS_HOST, HASS_PORT);

void enableGPS(void) {
	bool res = modem.disableGPS();
	if (!res) {
		Serial.println("Disnable GNSS Failed");
	}

	// Configure GNSS. Enable all but GLONASS.
	res = modem.setGNSSMode(7, 1);
	if (!res) {
		Serial.println("Set GNSS mode Failed");
	}

	// Enable GNSS.
	res = modem.enableGPS();
	if (!res) {
		Serial.println("Enable GNSS Failed");
	}
}

void disableGPS(void) {
	bool res = modem.disableGPS();
	if (!res) {
		Serial.println("Disnable GNSS Failed");
	}
}

void modemPowerOn() {
	pinMode(MODEM_PWRKEY, OUTPUT);
	digitalWrite(MODEM_PWRKEY, HIGH);
	delay(300);
	digitalWrite(MODEM_PWRKEY, LOW);

	/*
	MODEM_FLIGHT IO:25 Modulator flight mode control,
	need to enable modulator, this pin must be set to high
	*/
	pinMode(MODEM_FLIGHT, OUTPUT);
	digitalWrite(MODEM_FLIGHT, HIGH);
}

void modemPowerOff() {
	pinMode(MODEM_PWRKEY, OUTPUT);
	digitalWrite(MODEM_PWRKEY, HIGH);
	delay(1500);    //Datasheet Ton mintues = 1.2S
	digitalWrite(MODEM_PWRKEY, LOW);
}

void send_data(const String data) {
	uint32_t start = millis();

	Serial.println("Making POST request securely");

	String contentType = "Content-Type: application/json";
	String Authorization = "Bearer " HASS_TOKEN;

	http.beginRequest();
	http.post("/api/states/device_tracker.pet_tracker");
	http.sendHeader("Content-Type", contentType);
	http.sendHeader("Content-Length", data.length());
	http.sendHeader("Authorization", Authorization);
	http.beginBody();
	http.write((const byte*)data.c_str(), data.length());

	int status_code = http.responseStatusCode();
	String response = http.responseBody();

	if (status_code < 200 || status_code > 299) {
		Serial.printf("Gor error response with status %d", status_code);
		Serial.println("");
		Serial.print("Response: ");
		Serial.println(response);
	}

	uint32_t end = millis();
	Serial.printf("HTTPS request took %0.3f seconds.\n", (end - start) / 1000);
}

/**
 * Setup routine.
 */
void setup() {
	// Set console baud rate
	Serial.begin(9600);
	sleep(10);

	serialGsm.begin(UART_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);

	modemPowerOn();

	// Restart takes quite some time
	// To skip it, call init() instead of restart()
	Serial.println("Initializing modem...");
	if (!modem.init()) {
		Serial.println("Failed to initialise modem, attempting to restart");
		modem.restart();
	}

	String name = modem.getModemName();
	Serial.println("Modem Name: " + name);

	String modemInfo = modem.getModemInfo();
	Serial.println("Modem Info: " + modemInfo);

	// Set full functionality
	modem.sendAT("+CFUN=1");
	modem.waitResponse();

	// Set mobile operation band
	modem.sendAT("+CBAND=ALL_MODE");
	modem.waitResponse();

	// Set network mode to automatic.
	bool nres = modem.setNetworkMode(2);
	if (!nres) {
		Serial.println("setNetworkMode false");
		return ;
	}
	delay(200);

  while (!modem.waitForNetwork()) {
		Serial.println("Waiting for network...");
		delay(10000);
	}

	if (modem.isNetworkConnected()) {
		Serial.println("Network connected");
	}

	String res;

	String ccid = modem.getSimCCID();
	Serial.println("CCID: " + ccid);

	String imei = modem.getIMEI();
	Serial.println("IMEI: " + imei);

	String cop = modem.getOperator();
	Serial.println("Operator: " + cop);

	Serial.println("Connecting through GPRS...");
	while (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
		delay(10000);
	}

	Serial.print("GPRS status: ");
	if (modem.isGprsConnected()) {
		Serial.println("connected");
	} else {
		Serial.println("not connected");
	}

	IPAddress local = modem.localIP();
	Serial.println("Local IP: " + String(local));

	int csq = modem.getSignalQuality();
	Serial.println("Signal quality: " + String(csq));

	// Get connection type and band.
	modem.sendAT("+CPSI?");
	String r;
	if (modem.waitResponse(1000L, r)) {
		Serial.println(r);
	}

	// Set root CA.
	secure.setCACert(root_ca);

	http.connectionKeepAlive();

	enableGPS();
}

/**
 * Main loop.
 */
void loop() {

	//Serial.println(modem.getGPSraw());

	float lat, lon, speed, alt, acc;
	int vsat, usat;
	if (modem.getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &acc)) {
		char char_data[1024];
		sprintf(char_data, "{\"state\": \"not_home\", \"attributes\": {\"latitude\": %0.6f, \"longitude\": %0.6f, \"source_type\": \"gps\", \"location_accuracy\": %0.2f}}", lat, lon, acc);
		String data = char_data;

		Serial.println(data);

		send_data(data);
	}

	delay(10000);
}
