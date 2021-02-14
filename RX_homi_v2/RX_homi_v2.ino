#include <GyverTimer.h>

#include "C:\Arduino\headers\RX_TX_command.h"

#include <Servo.h>
Servo myservo;

int pos = 0;



#include <GyverRGB.h>
#include <PWM.h>
GRGB strip(5, 3, 6);  // куда подключены цвета (R, G, B)

// Настройка эффектов
#define HUE_START 140     // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
#define HUE_GAP 10      // коэффициент цвета огня (чем больше - тем дальше заброс по цвету)
#define SMOOTH_K 0.15   // коэффициент плавности огня
#define MIN_BRIGHT 80   // мин. яркость огня
#define MAX_BRIGHT 255  // макс. яркость огня
#define MIN_SAT 255     // мин. насыщенность
#define MAX_SAT 255     // макс. насыщенность

#define Period_1 1000

// Таймеры для света
#define time_strip_GM 40
#define time_strip_GN 40
#define time_strip_Buy 350

GTimer_ms time_s_GM;
GTimer_ms time_s_GN;
GTimer_ms time_s_Buy;


//#define Helloing 2
//#define GoodNighting 3
//#define GMing 4
//#define USSRing 5
//#define Breakfasting 6
//#define Chilling 7

unsigned long timer_1;

byte R, G, B;
byte Bri = 0;
#define rele 4
int Rejim;
byte Op_cl;

int deg;

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void fireTick() {
  static uint32_t prevTime, prevTime2;
  static byte fireRnd = 0;
  static float fireValue = 0;

  // задаём направление движения огня
  if (millis() - prevTime > 100) {
    prevTime = millis();
    fireRnd = random(0, 10);
  }
  // двигаем пламя
  if (millis() - prevTime2 > 20) {
    prevTime2 = millis();
    fireValue = (float)fireValue * (1 - SMOOTH_K) + (float)fireRnd * 10 * SMOOTH_K;
    strip.setHSV(
      constrain(map(fireValue, 20, 60, HUE_START, HUE_START + HUE_GAP), 0, 255),   // H
      constrain(map(fireValue, 20, 60, MAX_SAT, MIN_SAT), 0, 255),       // S
      constrain(map(fireValue, 20, 60, MIN_BRIGHT, MAX_BRIGHT), 0, 255)  // V
    );
  }
}



void Chill() {
  fireTick();
}

void Breakfast() {
  R = 248;
  G = 0;
  B = 0;
  strip.setRGB(R, G, B);
}

void Buy() {
  if (time_s_Buy.isReady()) {
    Bri--;   // "сбросить" таймер
    strip.setBrightness(Bri);
    Serial.println(Bri);
    strip.setRGB(R, G, B);

  }
}


void GoodNight() {
  Bri = 255;
  strip.setBrightness(Bri);
  Serial.println("start");
  while (Bri != 0) {
    if (time_s_GN.isReady()) {
      Bri--;   // "сбросить" таймер
      strip.setBrightness(Bri);
      Serial.println(Bri);
      strip.setRGB(R, G, B);
    }
  }
    myservo.writeMicroseconds(1600); // Крутит по часовой в течение секунды закрытие
    delay(5300);                     //
    myservo.writeMicroseconds(1500); // Останавливается
  
}

void GM() {
  Bri = 0;

  strip.setBrightness(Bri);
  while (Bri != 255) {
    if (time_s_GM.isReady()) {
      Bri++;
      strip.setBrightness(Bri);
      strip.setRGB(R, G, B);
    }

    
    if (Bri == 180) {
      digitalWrite(rele, HIGH);
    }
  }
  myservo.writeMicroseconds(1400); // Крутит по часовой в течение секунды
    delay(6000);                     //
    myservo.writeMicroseconds(1500); // Останавливается
}

void Hellou() {
  digitalWrite(rele, HIGH);

  while (Bri < 255) {
    if (millis() - timer_1 >= Period_1) {
      timer_1 = millis();   // "сбросить" таймер
      strip.setRGB(255, 255, 255); // Заливаем белым
      Bri = Bri + 15;
      strip.setBrightness(Bri);
    }
  }
}



void setup() {

  pinMode(rele, OUTPUT);

  Serial.begin(9600); //открываем порт для связи с ПК

  //strip.setDirection(REVERSE);
  //  R = 0;
  //  G = 250;
  //  B = 0;
  //  strip.setBrightness(Bri);

  myservo.attach(2);

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль

  time_s_GM.setInterval(time_strip_GM);
  time_s_GN.setInterval(time_strip_GN);
  time_s_Buy.setInterval(time_strip_Buy);

}

void loop() {
  com recived;
  byte pipeNo, gotByte;
  while ( radio.available(&pipeNo)) {                                 // слушаем эфир со всех труб
    //radio.read( &gotByte, sizeof(gotByte) );                          // чиатем входящий сигнал
    radio.read( &recived, sizeof(com) );

    Serial.print("Recieved command: ");
    Serial.println(recived.comand);

    switch (recived.comand) {
      case chg_mode: {
          Rejim = recived.data[0];
          switch (Rejim) {
            case 2: {
                Serial.println("Hellou");
                Hellou();
                break;
              }
            case 3: {
                Serial.println("GoodNight");
                GoodNight();
                break;
              }
            case 4: {
                Serial.println("GM");
                GM();
                break;
              }
            //            case 5: {
            //                Serial.println("USSR");
            //                USSR();
            //                break;
            //              }
            case 6: {
                Serial.println("Breakfast");
                Breakfast();
                break;
              }
            case 7: {
                Serial.println("Chill");
                Chill();
                break;
              }
            case 8: {
                Serial.println("Buy");
                Buy();
                break;
              }
          }
          break;
        }
      case chg_color: {
          R = recived.data[0];
          G = recived.data[1];
          B = recived.data[2];
          Serial.print("R: ");  Serial.print(R);
          Serial.print(" G: "); Serial.print(G);
          Serial.print(" B: "); Serial.println(B);
          strip.setRGB(R, G, B);
          break;
        }
      case chg_rosetka: {
          Op_cl = recived.data[0];
          if (Op_cl == 1) {
            Serial.println("Открыть");
            digitalWrite(rele, HIGH);
          }
          if (Op_cl == 2) {
            Serial.println("Закрыть");
            digitalWrite(rele, LOW);
          }
          else {
            Serial.println("Eror Rosetka");
          }
          break;
        }
      case chg_serv: {
          if (recived.data[0] == 3) {
            Serial.println("Serv to go Niga");
            Serial.println();
          }
          else {
            Serial.println("Niga go out");
            Serial.println();
          }
          break;
        }
    }
  }
}
