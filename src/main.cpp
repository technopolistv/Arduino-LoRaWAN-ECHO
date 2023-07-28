#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#include "conf.h"

void txMode() {
  LoRa.setFrequency(FREQ);
  LoRa.setSpreadingFactor(TX_SF);
  LoRa.disableInvertIQ();       
  LoRa.idle();  
}

void rxMode() {
  LoRa.setFrequency(FREQ);
  LoRa.setSpreadingFactor(RX_SF);
  LoRa.enableInvertIQ();
  LoRa.receive();
}

void setup() {
  SPI.begin(SCK, MISO, MOSI, SS);
  
  LoRa.setPins(SS, RST, DI0);
  LoRa.setFrequency(FREQ);
  LoRa.setCodingRate4(RATE);
  LoRa.setSignalBandwidth(BANDWIDTH);

  Serial.begin(115200);

  if (!LoRa.begin(FREQ)) {
    while (1) {
      Serial.println("Starting LoRa failed!");
      delay(5000);
    }
  } else {
    Serial.println("LoRa started");
  }

  // necessary settings for LoRaWAN gateway
  LoRa.enableCrc();
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x34);

  rxMode();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 15000) {
    Serial.print("Sending LoRaWAN packet... ");

    txMode();

    // send Proprietary LoRaWAN packet
    LoRa.beginPacket(0);
    LoRa.write(0xFF);
    LoRa.print(millis());
    LoRa.print(" HELLO TECHNOPOLIS CITIZEN");
    LoRa.endPacket(false);

    rxMode();

    Serial.println("done");

    previousMillis = currentMillis;
  }

  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    LoRa.read(); // get rid of first byte

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }

}
