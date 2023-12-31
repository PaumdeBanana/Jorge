#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <MFRC522.h>
#include <BluetoothSerial.h>

#define tagMu "E6 D5 FD 93"
#define tagGirar "13 5A 43 10"
#define tagMuv "31 5C 04 1F"
#define tagNrf "A3 69 AE 1B"
#define tagBluetooth "63 53 AA A5"

#define btnome "Jorge"

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

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);
RF24 radio(NRF_CE_PIN, NRF_SS_PIN);
BluetoothSerial BT;

String conteudo = "";

int speed1, speed2, speed3, speed4;
char btdata = 'a';

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
  if (conteudo.substring(1) == tagMu) {
    mu();
  }
  if (conteudo.substring(1) == tagGirar) {
    girar();
  }
  if (conteudo.substring(1) == tagMuv) {
    muv();
  }
  if (conteudo.substring(1) == tagNrf) {
    nrf();
  }
  if (conteudo.substring(1) == tagBluetooth) {
    bluetooth();
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
  btdata = 'a';
  resetData();
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
    if (conteudo.substring(1) == tagMu) {
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
    if (conteudo.substring(1) == tagGirar) {
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
    if (conteudo.substring(1) == tagMuv) {
      reset();
      return;
    }
    delay(250);
  }
  if (conteudo.substring(1) == tagMuv) {
    reset();
    return;
  }
  for (int d = 0; d < 12; d++) {
    delay(250);
    if (conteudo.substring(1) == tagMuv) {
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

    //Left
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

    //Right
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
    ledcWrite(3, speed3 * data.pot2 / 255);
    ledcWrite(4, speed4 * data.pot1 / 255);

    if (data.button1 == 0) {
      SPI.end();
      SPI.begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
      mfrc522.PCD_Init();
      reset();
      return;
    }
  }
}

void bluetooth() {
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  digitalWrite(i1, LOW);
  digitalWrite(i2, LOW);
  digitalWrite(i3, LOW);
  digitalWrite(i4, LOW);

  BT.begin(btnome);
  delay(1500);

  while (true) {
    if (btdata == 'a') {
      digitalWrite(buzzer, HIGH);
      delay(1000);
      digitalWrite(buzzer, LOW);
      delay(1000);
      if (BT.available()) {
        btdata = BT.read();
      }
      conteudo = "";
      rfid();
      if (conteudo.substring(1) == tagBluetooth) {
        BT.disconnect();
        BT.end();
        reset();
        return;
      }
    }

    else {
      if (BT.available()) {
        btdata = BT.read();
      }

      if (btdata == '1') {
        ledcWrite(1, 24);
        ledcWrite(2, 24);
        ledcWrite(3, 24);
        ledcWrite(4, 24);
      }

      if (btdata == '2') {
        ledcWrite(1, 48);
        ledcWrite(2, 48);
        ledcWrite(3, 48);
        ledcWrite(4, 48);
      }

      if (btdata == '3') {
        ledcWrite(1, 72);
        ledcWrite(2, 72);
        ledcWrite(3, 72);
        ledcWrite(4, 72);
      }

      if (btdata == '4') {
        ledcWrite(1, 96);
        ledcWrite(2, 96);
        ledcWrite(3, 96);
        ledcWrite(4, 96);
      }

      if (btdata == '5') {
        ledcWrite(1, 120);
        ledcWrite(2, 120);
        ledcWrite(3, 120);
        ledcWrite(4, 120);
      }

      if (btdata == '6') {
        ledcWrite(1, 144);
        ledcWrite(2, 144);
        ledcWrite(3, 144);
        ledcWrite(4, 144);
      }

      if (btdata == '7') {
        ledcWrite(1, 168);
        ledcWrite(2, 168);
        ledcWrite(3, 168);
        ledcWrite(4, 168);
      }

      if (btdata == '8') {
        ledcWrite(1, 192);
        ledcWrite(2, 192);
        ledcWrite(3, 192);
        ledcWrite(4, 192);
      }

      if (btdata == '9') {
        ledcWrite(1, 216);
        ledcWrite(2, 216);
        ledcWrite(3, 216);
        ledcWrite(4, 216);
      }

      if (btdata == 'q') {
        ledcWrite(1, 255);
        ledcWrite(2, 255);
        ledcWrite(3, 255);
        ledcWrite(4, 255);
      }

      //Foward
      if (btdata == 'F') {
        digitalWrite(i1, LOW);
        digitalWrite(i2, HIGH);
        digitalWrite(i3, HIGH);
        digitalWrite(i4, LOW);
      }

      //Back
      else if (btdata == 'B') {
        digitalWrite(i1, HIGH);
        digitalWrite(i2, LOW);
        digitalWrite(i3, LOW);
        digitalWrite(i4, HIGH);
      }

      //Left
      else if (btdata == 'L') {
        digitalWrite(i1, HIGH);
        digitalWrite(i2, LOW);
        digitalWrite(i3, HIGH);
        digitalWrite(i4, LOW);

      }

      //Right
      else if (btdata == 'R') {
        digitalWrite(i1, LOW);
        digitalWrite(i2, HIGH);
        digitalWrite(i3, LOW);
        digitalWrite(i4, HIGH);
      }

      //Stop
      else if (btdata == 'S') {
        digitalWrite(i1, LOW);
        digitalWrite(i2, LOW);
        digitalWrite(i3, LOW);
        digitalWrite(i4, LOW);
      }

      else if (btdata == 'V') {
        digitalWrite(buzzer, HIGH);
      }

      else if (btdata == 'v') {
        digitalWrite(buzzer, LOW);
      }

      else if (btdata == 'X' || btdata == 'x') {
        BT.disconnect();
        BT.end();
        reset();
        return;
      }

      delay(20);
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
