#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <MFRC522.h>

#define fabio "E6 D5 FD 93"
#define gira "13 5A 43 10"
#define belle "31 5C 04 1F"
#define ester "A3 69 AE 1B"

#define HSPI_CLK 14
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SS 15

#define VSPI_CLK 18
#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SS 5

#define buzzer 4
#define en1 21
#define en2 22
#define en3 17
#define en4 16
#define i1 32
#define i2 33
#define i3 25
#define i4 26

#define RFID_SS_PIN 5
#define RFID_RST_PIN 2
#define NRF_SS_PIN 15
#define NRF_CE_PIN 27

MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);
RF24 radio(NRF_CE_PIN, NRF_SS_PIN);

String conteudo = "";

int speed1, speed2, speed3, speed4;

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;
const byte address = 1;

struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
  byte j2PotX;
  byte j2PotY;
  byte j2Button;
  byte pot1;
  byte pot2;
  byte tSwitch1;
  byte tSwitch2;
  byte button1;
  byte button2;
  byte button3;
  byte button4;
};

Data_Package data;

void setup() {
  Serial.begin(9600);

  SPI.begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
  mfrc522.PCD_Init();

  ledcSetup(1, 1000, 8);
  ledcSetup(2, 1000, 8);
  ledcSetup(3, 1000, 8);
  ledcSetup(4, 1000, 8);
  ledcAttachPin(en1, 1);
  ledcAttachPin(en2, 2);
  ledcAttachPin(en3, 3);
  ledcAttachPin(en4, 4);

  pinMode(buzzer, OUTPUT);
  pinMode(i1, OUTPUT);
  pinMode(i2, OUTPUT);
  pinMode(i3, OUTPUT);
  pinMode(i4, OUTPUT);
}

void loop() {
  conteudo = "";
  rfid();
  Serial.println(conteudo);
  if (conteudo.substring(1) == fabio) {
    mu();
  }
  if (conteudo.substring(1) == gira) {
    girar();
  }
  if (conteudo.substring(1) == belle) {
    muv();
  }
  if (conteudo.substring(1) == ester) {
    nrf();
  }
}

void rfid() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      conteudo.toUpperCase();
    }
  }
}

void reset() {
  speed1 = 0;
  speed2 = 0;
  speed3 = 0;
  speed4 = 0;
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  ledcWrite(4, 0);
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(1500);
}

void mu() {
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  digitalWrite(i1, LOW);
  digitalWrite(i2, HIGH);
  digitalWrite(i3, HIGH);
  digitalWrite(i4, LOW);
  ledcWrite(1, 255);
  ledcWrite(2, 255);
  ledcWrite(3, 255);
  ledcWrite(4, 255);
  delay(1500);
  while (true) {
    conteudo = "";
    rfid();
    if (conteudo.substring(1) == fabio) {
      reset();
      return;
    }
  }
}

void girar() {
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  digitalWrite(i1, LOW);
  digitalWrite(i2, HIGH);
  digitalWrite(i3, LOW);
  digitalWrite(i4, HIGH);
  ledcWrite(1, 255);
  ledcWrite(2, 255);
  ledcWrite(3, 255);
  ledcWrite(4, 255);
  delay(1500);
  while (true) {
    conteudo = "";
    rfid();
    if (conteudo.substring(1) == gira) {
      reset();
      return;
    }
  }
}

void muv() {
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  digitalWrite(i1, LOW);
  digitalWrite(i2, HIGH);
  digitalWrite(i3, HIGH);
  digitalWrite(i4, LOW);
  speed1 = 125;
  speed2 = 125;
  speed3 = 125;
  speed4 = 125;
  delay(1000);
  for (int d = 0; d < 13; d++) {
    speed1 += 10;
    speed2 += 10;
    speed3 += 10;
    speed4 += 10;
    ledcWrite(1, speed1);
    ledcWrite(2, speed2);
    ledcWrite(3, speed3);
    ledcWrite(4, speed4);
    delay(250);
    conteudo = "";
    rfid();
    if (conteudo.substring(1) == belle) {
      reset();
      return;
    }
    delay(250);
  }
  if (conteudo.substring(1) == belle) {
    reset();
    return;
  }
  for (int d = 0; d < 12; d++) {
    delay(250);
    if (conteudo.substring(1) == belle) {
      reset();
      return;
    }
  }
  reset();
}

void nrf() {
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);

  SPI.end();
  SPI.begin(HSPI_CLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  resetData();

  delay(1500);
  while (true) {
    currentTime = millis();
    if (currentTime - lastReceiveTime > 1000) {
      resetData();
    }
    if (radio.available()) {
      radio.read(&data, sizeof(Data_Package));
      lastReceiveTime = millis();
    }

    //Foward
    if (data.j1PotY > 142) {
      digitalWrite(i1, LOW);
      digitalWrite(i2, HIGH);
      digitalWrite(i3, HIGH);
      digitalWrite(i4, LOW);

      speed1 = map(data.j1PotY, 142, 255, 140, 255);
      speed2 = map(data.j1PotY, 142, 255, 140, 255);
      speed3 = map(data.j1PotY, 142, 255, 140, 255);
      speed4 = map(data.j1PotY, 142, 255, 140, 255);
    }

    //Back
    if (data.j1PotY < 110) {
      digitalWrite(i1, HIGH);
      digitalWrite(i2, LOW);
      digitalWrite(i3, LOW);
      digitalWrite(i4, HIGH);

      speed1 = map(data.j1PotY, 110, 0, 140, 255);
      speed2 = map(data.j1PotY, 110, 0, 140, 255);
      speed3 = map(data.j1PotY, 110, 0, 140, 255);
      speed4 = map(data.j1PotY, 110, 0, 140, 255);
    }

    //left
    if (data.j1PotX > 142) {
      digitalWrite(i1, HIGH);
      digitalWrite(i2, LOW);
      digitalWrite(i3, HIGH);
      digitalWrite(i4, LOW);

      speed1 = map(data.j1PotX, 142, 255, 140, 255);
      speed2 = map(data.j1PotX, 142, 255, 140, 255);
      speed3 = map(data.j1PotX, 142, 255, 140, 255);
      speed4 = map(data.j1PotX, 142, 255, 140, 255);
    }

    //right
    if (data.j1PotX < 110) {
      digitalWrite(i1, LOW);
      digitalWrite(i2, HIGH);
      digitalWrite(i3, LOW);
      digitalWrite(i4, HIGH);

      speed1 = map(data.j1PotX, 110, 0, 140, 255);
      speed2 = map(data.j1PotX, 110, 0, 140, 255);
      speed3 = map(data.j1PotX, 110, 0, 140, 255);
      speed4 = map(data.j1PotX, 110, 0, 140, 255);
    }

    if (data.j1PotX > 110 && data.j1PotX < 142 && data.j1PotY > 110 && data.j1PotY < 142) {
      speed1 = 0;
      speed2 = 0;
      speed3 = 0;
      speed4 = 0;
    }

    ledcWrite(1, speed1 * data.pot1 / 255);
    ledcWrite(2, speed2 * data.pot2 / 255);
    ledcWrite(3, speed3 * data.pot1 / 255);
    ledcWrite(4, speed4 * data.pot2 / 255);

    if (data.button1 == 0) {
      SPI.end();
      SPI.begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
      mfrc522.PCD_Init();
      reset();
      return;
    }
  }
}

void resetData() {
  data.j1PotX = 127;
  data.j1PotY = 127;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.j1Button = 0;
  data.j2Button = 0;
  data.pot1 = 1;
  data.pot2 = 1;
  data.tSwitch1 = 1;
  data.tSwitch2 = 1;
  data.button1 = 1;
  data.button2 = 1;
  data.button3 = 1;
  data.button4 = 1;
}
