#include <SPI.h>
#include <LoRa.h>

// تعريف أطراف التوصيل لوحدة LoRa مع ESP32 (SPI Pins)
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

// تردد العمل (مثلاً 433E6 أو 915E6 حسب الوحدات المتوفرة لديك)
#define BAND 433E6 

// معرف العقدة الفريد
#define NODE_ID "NODE_MARIB_01"

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(1000);

  Serial.println("Initializing ESP32 LoRa Emergency Node...");

  // تهيئة حافلة SPI لأطراف لوحة LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed! Check your hardware connections.");
    while (1);
  }

  // إعدادات متقدمة للمدى الطويل (Long Range Configuration)
  LoRa.setSpreadingFactor(12); // مدى أطول (Longer range)
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  
  Serial.println("LoRa Initialized Successfully. Ready for Off-Grid Comms.");
}

void loop() {
  // 1. التحقق من استقبال رسائل أو حزم طوارئ واردة من عقد أخرى
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet: ");
    
    String incomingMessage = "";
    while (LoRa.available()) {
      incomingMessage += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();
    float snr = LoRa.packetSnr();

    Serial.println(incomingMessage);
    Serial.print("RSSI: "); Serial.print(rssi);
    Serial.print(" | SNR: "); Serial.println(snr);
  }

  // 2. إرسال حزمة تجريبية دورية أو نبضة حياة (Heartbeat / Telemetry)
  // يمكنك ربطها بزر طوارئ (Panic Button) محلي لاحقاً
  static unsigned long lastSendTime = 0;
  unsigned long interval = 15000; // كل 15 ثانية

  if (millis() - lastSendTime > interval) {
    lastSendTime = millis();
    
    String outgoing = "HEARTBEAT|" + String(NODE_ID) + "|STATUS:OK";
    
    LoRa.beginPacket();
    LoRa.print(outgoing);
    LoRa.endPacket();
    
    Serial.println("Sent telemetry packet over LoRa mesh.");
  }
}
