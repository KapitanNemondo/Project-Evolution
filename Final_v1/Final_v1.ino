// настройки ленты
#define NUM_LEDS 86    // количество светодиодов
#define LED_PIN 4      // пин ленты

// настройки пламени
#define HUE_GAP 21      // заброс по hue
#define FIRE_STEP 20    // шаг огня
#define HUE_START 0     // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
#define MIN_BRIGHT 70   // мин. яркость огня
#define MAX_BRIGHT 255  // макс. яркость огня
#define MIN_SAT 245     // мин. насыщенность
#define MAX_SAT 255     // макс. насыщенность

// для разработчиков
#include <FastLED.h>
#define ORDER_GRB       // порядок цветов ORDER_GRB / ORDER_RGB / ORDER_BRG
#define COLOR_DEBTH 2   // цветовая глубина: 1, 2, 3 (в байтах)
// на меньшем цветовом разрешении скетч будет занимать в разы меньше места,
// но уменьшится и количество оттенков и уровней яркости!

// ВНИМАНИЕ! define настройки (ORDER_GRB и COLOR_DEBTH) делаются до подключения библиотеки!
#include <microLED.h>

LEDdata leds[NUM_LEDS];  // буфер ленты типа LEDdata (размер зависит от COLOR_DEBTH)
microLED strip(leds, NUM_LEDS, LED_PIN);  // объект лента
int counter = 0;

// ленивая жопа
#define FOR_i(from, to) for(int i = (from); i < (to); i++)
#define FOR_j(from, to) for(int j = (from); j < (to); j++)


#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include "C:\Arduino\headers\RX_TX_command.h"
#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля


// Пины кнопок
#define buttonPause A0
#define buttonVolumeUp A1
#define buttonVolumeDown A2

// Таймеры
#define Period_1 100
#define Period_2 1000
#define Period_3 500

// Отладка
//#define debug_sound
//#define debug_radio
#define debug_bluet

// Переменные таймеров
unsigned long timer_1, timer_2, timer_3;

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

#ifdef debug_bluet
#define BluethousSerial Serial
#endif debug_bluet

//SoftwareSerial BluethousSerial(5, 6);

SoftwareSerial mySoftwareSerial(7, 8); // RX, TX для плеера DFPlayer Mini было 10 11
DFRobotDFPlayerMini myDFPlayer;

byte tx_a = 0;
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб


int in_telephone;

byte Rejim;
byte R, G, B;

unsigned long currentPause = millis();
bool fl_pause = 0;
boolean isPlaying = false; // статус воспроизведения/пауза


void Zason() {                    // Алгоритм Засыпания
  myDFPlayer.start();     //запускаем mp3 с паузы
  isPlaying = true;       // воспроизводим

  com transmition;
  transmition.comand = chg_mode;
  transmition.data[0] = 3;          // Режим засыпания
#ifdef debug_radio
  Serial.print("Sent: "); Serial.println(transmition.comand);
#endif debug_radio
  radio.write(&transmition, sizeof(transmition));
  int Timing = 0;
  while (1) {
    fireTick();
#ifdef debug_bluet
    while (BluethousSerial.available()) {
      Bloup();
    }
#endif debug_bluet
  }

  //  transmition.comand = chg_rosetka;
  //  transmition.data[0] = 13;        // Режим закрытия розетки
  //  Serial.print("Sent: "); Serial.println(transmition.comand);
  //  radio.write(&transmition, sizeof(transmition));
  // двигаем пламя
  //  fireTick();
  myDFPlayer.stop();     //запускаем mp3 с паузы
}

void Proson() {                   // Алгоритм Просыпания
  myDFPlayer.start();     //запускаем mp3 с паузы
  isPlaying = true;       // воспроизводим
  //   Звук на минимум
  //   оправить chg_mode просыпаемся
  //    for () {
  //      // крутим лес
  //      // увеличиваем звук
  //      delay(400);
  //    }
  myDFPlayer.stop();     //запускаем mp3 с паузы
}

void Swet_up() {                  // Выбор фиксированных цветов
  com transmition;
  if (Rejim == 0) {                 // Цвет морской волны
    R = 0;
    G = 255;
    B = 255;

  }
  if (Rejim == 1) {                 // Цвет синий Крайола
    R = 31;
    G = 117;
    B = 254;
  }
  if (Rejim == 2) {                 // Цвет синий Градуса
    R = 0;
    G = 125;
    B = 255;
  }
  if (Rejim == 3) {                 // Цвет Бирюзовый
    R = 48;
    G = 213;
    B = 200;
  }
  if (Rejim == 4) {                 // Цвет Аметистовый
    R = 153;
    G = 102;
    B = 204;
  }
  if (Rejim == 5) {                 // Цвет Светло бирюзовый
    R = 64;
    G = 224;
    B = 208;
  }
  if (Rejim == 6) {                 // Цвет Малиновый
    R = 220;
    G = 20;
    B = 60;
  }
  if (Rejim == 7) {                 // Цвет Коричнево Бордовый
    R = 165;
    G = 42;
    B = 42;
  }
  if (Rejim == 8) {                 // Цвет Красный Крайола
    R = 238;
    G = 32;
    B = 77;
  }
  if (Rejim == 9) {                 // Цвет Рыжий
    R = 215;
    G = 125;
    B = 49;
  }
  if (Rejim == 10) {                // Цвет Шоколадный
    R = 210;
    G = 105;
    B = 30;
  }
  if (Rejim == 11) {                // Цвет Шоколадный
    R = 255;
    G = 0;
    B = 0;
  }
  transmition.comand = chg_color;
  transmition.data[0] = R;
  transmition.data[1] = G;
  transmition.data[2] = B;

#ifdef debug_radio
  Serial.print("Sent: "); Serial.println(transmition.comand);
#endif debug_radio
  radio.write(&transmition, sizeof(transmition));
  Rejim++;
  if (Rejim > 11) {
    Rejim = 0;
  }
}
#ifdef debug_bluet
void Bloup() {                    // Обработчик Блютуза
  in_telephone = BluethousSerial.parseInt();
#ifdef debug_radio
  Serial.print("Значение Блютуз: ");
  Serial.println(in_telephone);
#endif debug_radio

  if (in_telephone == 12) {       // Засыпаем
    Zason();
  }
  if (in_telephone == 13) {       // Просыпаемся
    Proson();
  }
  if (in_telephone == 14) {       // Меняем цвет
    Swet_up();
  }
  if (in_telephone == 15) {       // Выключение
#ifdef debug_radio
    Serial.print("Значение Блютуз 15; ");
    Serial.println("Остановлен режим огня");
#endif debug_radio

  }
}
#endif debug_bluet
void setup() {
  pinMode(buttonPause, INPUT_PULLUP);
  pinMode(buttonVolumeUp, INPUT_PULLUP);
  pinMode(buttonVolumeDown, INPUT_PULLUP);

  // put your setup code here, to run once:
#ifdef debug_radio
  Serial.begin(9600);
#endif debug_radio
#ifdef debug_bluet
  BluethousSerial.begin(9600);
#endif debug_bluet
  mySoftwareSerial.begin(9600);
  strip.setBrightness(255);

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик
  if (millis() - timer_1 >= Period_1) {
    timer_1 = millis();   // "сбросить" таймер
    // дейтвия, которые хотим выполнить один раз за период
  }
#ifdef debug_sound
  Serial.println();
  Serial.println("DFPlayer Mini Demo");
  Serial.println("Initializing DFPlayer...");
#endif debug_sound
  if (!myDFPlayer.begin(mySoftwareSerial)) {
#ifdef debug_sound
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
#endif debug_sound
    while (true);
  }
#ifdef debug_sound
  Serial.println(F("DFPlayer Mini online."));
#endif debug_sound
  myDFPlayer.setTimeOut(300);
  //----Set volume----
  myDFPlayer.volume(15); //Set volume value (0~30).
  //----Set different EQ----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  //myDFPlayer.play(1); //Воспроизведение первого mp3

  myDFPlayer.playLargeFolder(2, 999);

  if (millis() - timer_2 >= Period_2) {
    timer_2 = millis();   // "сбросить" таймер
    // дейтвия, которые хотим выполнить один раз за период
  }

  isPlaying = true; // воспроизводим
#ifdef debug_sound
  Serial.println("Playing..");
  //----Читать информацию----
  Serial.println(myDFPlayer.readState()); //читать состояние mp3
  Serial.println(myDFPlayer.readVolume()); //Текущая громкость
  Serial.println(myDFPlayer.readEQ()); // читаем настройку эквалайзера
  Serial.println(myDFPlayer.readFileCounts()); // читать все файлы на SD-карте
  Serial.println(myDFPlayer.readCurrentFileNumber()); // текущий номер файла воспроизведения
#endif debug_sound
}

void loop() {
  // put your main code here, to run repeatedly:
  //  Serial.println("Work");
#ifdef debug_bluet
  while (BluethousSerial.available()) {
    Bloup();
  }
#endif debug_bluet
  /*
      in_telephone = BluethousSerial.parseInt();
      Serial.print("Значение Блютуз: ");
      Serial.println(in_telephone);
      if (in_telephone == 12) {       // Засыпаем
        Zason();
      }
      if (in_telephone == 13) {       // Просыпаемся
        Proson();
      }
      if (in_telephone == 14) {       // Меняем цвет
        Swet_up();
      }
    }
  */



  if (digitalRead(buttonPause) == LOW) {        // Пауза/стоп музыки
    if (isPlaying) { // если было воспроизведение трека
      myDFPlayer.pause(); // пауза
      isPlaying = false; // пауза
#ifdef debug_sound
      Serial.println("Paused..");
#endif debug_sound
    } else { // иначе
      isPlaying = true; // воспроизводим
      myDFPlayer.start(); //запускаем mp3 с паузы
    }
    delay(500);
  }
  if (digitalRead(buttonVolumeUp) == LOW) {     // Увеличение Громкости
    if (isPlaying) {
      myDFPlayer.volumeUp(); //Volume Up
#ifdef debug_sound
      Serial.println("Volume Up..");
#endif debug_sound
    }
    delay(500);
  }
  if (digitalRead(buttonVolumeDown) == LOW) {   // Уменьшение Громкости
    if (isPlaying) {
      myDFPlayer.volumeDown(); //Volume Down
#ifdef debug_sound
      Serial.println("Volume Down..");
#endif debug_sound
    }
    delay(500);
  }
}


void fireTick() {
  static uint32_t prevTime;

  // двигаем пламя
  if (millis() - prevTime > 50) {
    prevTime = millis();
    int thisPos = 0, lastPos = 0;
    FOR_i(0, NUM_LEDS) {
      leds[i] = getFireColor((inoise8(i * FIRE_STEP, counter)));
    }
    counter += 20;
    strip.show();
  }
}

// возвращает цвет огня для одного пикселя
LEDdata getFireColor(int val) {
  // чем больше val, тем сильнее сдвигается цвет, падает насыщеность и растёт яркость
  return mHSV(
           HUE_START + map(val, 0, 255, 0, HUE_GAP),                    // H
           constrain(map(val, 0, 255, MAX_SAT, MIN_SAT), 0, 255),       // S
           constrain(map(val, 0, 255, MIN_BRIGHT, MAX_BRIGHT), 0, 255)  // V
         );
}
