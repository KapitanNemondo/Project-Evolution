
// настройки ленты
#define NUM_LEDS 80    // количество светодиодов
#define LED_PIN 4      // пин ленты

// настройки пламени
#define FIRE_STEP 15    // шаг огня
#define HUE_GAP 21      // заброс по hue
#define HUE_START 0     // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
#define MIN_BRIGHT 70   // мин. яркость огня
#define MAX_BRIGHT 255  // макс. яркость огня
#define MIN_SAT 245     // мин. насыщенность
#define MAX_SAT 255     // макс. насыщенность

// Настройка параметров пламени
#define HUE_GAP 21
#define HUE_START_sea 172
#define HUE_START_Embiend 180
#define HUE_START_forest 80


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

// Пины кнопок
#define buttonRFID 5
#define buttonVolumeUp 3
#define buttonVolumeDown 2

// Таймеры
#define Period_1 100
#define Period_2 1000
#define Period_3 500
#define Period_4 1000
#define Period_5 1000

// Таймеры для звука
#define time_vol_GM 27     // Было 27000
#define time_vol_GN 50     // Было 50000

// Таймеры для света
#define time_strip_GM 10    // Было 2000
#define time_strip_GN 10    // Было 9000
#define time_strip_Buy 500

// Отладка
//#define debug_sound
//#define debug_radio
#define debug_bluet
#define debug_serial_bloet

// Режимы работы !!!!!!!!!!!!!!!!!!! не трогать !!!!!!!!!!!!!!!!!
//#define Helloing 2
//#define GoodNighting 3
//#define GMing 4
//#define USSRing 5
//#define Breakfasting 6
//#define Chilling 7

#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include "C:\Arduino\headers\RX_TX_command.h"
#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля
#include <GyverTimer.h>   // Юиблиотека для работы таймеров
#include <TroykaRTC.h>    // Библиотека для работы RTC
#include <Wire.h>         // Библиотека для работы протокола I2C

RTC clock;
int set_hour, set_minute;

// Таймеры
GTimer_ms time_v_GM;
GTimer_ms time_v_GN;
GTimer_ms time_s_GM;
GTimer_ms time_s_GN;
GTimer_ms time_s_Buy;

// Переменные таймеров
unsigned long timer_1, timer_2, timer_3, timer_4, timer_5;

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги


#define BluethousSerial Serial


//SoftwareSerial BluethousSerial(5, 6);

SoftwareSerial mySoftwareSerial(7, 8); // RX, TX для плеера DFPlayer Mini было 10 11
DFRobotDFPlayerMini mp3;

byte tx_a = 0;
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб


int in_telephone, set_time;

byte volum;

byte Rejim;
byte R, G, B;
boolean flag_bri_down = 0;
boolean flag_bri_up = 0;

boolean flag_Hellou = 1; // 0 работа 1 ожидает нажатия

unsigned long currentPause = millis();
bool fl_pause = 0;
boolean isPlaying = false; // статус воспроизведения/пауза

byte bri;

int current_mode = 0;
#define LIGHT_RISING_MODE 5
#define LIGHT_RISING_MODE_DOWN 6
#define CHILL_MODE 7
#define BREAKFAST_MODE 8
#define GM_MODE 9
#define WAKE_UP_MODE 10
#define GN_MODE 11


int loop_count = 0;
long last_time = millis();

void Bloup() {                    // Обработчик Блютуза
  in_telephone = BluethousSerial.parseInt();
#ifdef debug_radio
  Serial.print("Значение Блютуз: ");
  Serial.println(in_telephone);
#endif debug_radio

  switch (in_telephone) {
    case 52: {
        Hellou();
#ifdef debug_bluet
        BluethousSerial.println("Verstein Hellou");
#endif debug_bluet
        break;
      }
    case 53: {
        GoodNight();
        break;
      }
    case 54: {
        GM();
        break;
      }
    /*    case 55: {
      //        USSR();
      //        break;
      //      }
      //    case 56: {
      //        Breakfast();
      //        break;
      //      }
      //    case 57: {
      //        Chill();
      //        break;
    */

    case 58: {
        BluethousSerial.println("Verstein");
        Swet();
        break;
      }
    case 60: {
        BluethousSerial.println("Chill");
        Chill();
        break;
      }
    case 61: {
        BluethousSerial.println("Breakfast");
        Breakfast();
        break;
      }
    case 62: {
        BluethousSerial.println("Time:");
        Real_Time();
        break;
      }
    case 63: {
        BluethousSerial.println("MP3:");
        muzzik();
        break;
      }
  }
}

/*
  void Knopki() {
  if (digitalRead(buttonPause) == LOW) {        // Пауза/стоп музыки
    if (isPlaying) { // если было воспроизведение трека
      mp3.pause(); // пауза
      isPlaying = false; // пауза
  #ifdef debug_sound
      Serial.println("Paused..");
  #endif debug_sound
    } else { // иначе
      isPlaying = true; // воспроизводим
      mp3.start(); //запускаем mp3 с паузы
    }
    delay(500);
  }
  if (digitalRead(buttonVolumeUp) == LOW) {     // Увеличение Громкости
    if (isPlaying) {
      mp3.volumeUp(); //Volume Up
  #ifdef debug_sound
      Serial.println("Volume Up..");
  #endif debug_sound
    }
    delay(500);
  }
  if (digitalRead(buttonVolumeDown) == LOW) {   // Уменьшение Громкости
    if (isPlaying) {
      mp3.volumeDown(); //Volume Down
  #ifdef debug_sound
      Serial.println("Volume Down..");
  #endif debug_sound
    }
    delay(500);
  }
  }
*/
void Hellou() {
  strip.clear();
  strip.show();
  com transmition;
  transmition.comand = chg_mode;
  transmition.data[0] = 2;          // Режим Hellou
#ifdef debug_bluet {
  BluethousSerial.print("Sent: "); BluethousSerial.println(transmition.comand);
#endif debug_bluet }
  radio.write(&transmition, sizeof(transmition));

  bri = 1;
  strip.setBrightness(bri);

  strip.fill(mRGB(250, 250, 250));
  strip.show();

  loop_count = 0;
  last_time = millis();
  current_mode = LIGHT_RISING_MODE;
  BluethousSerial.println("end hello");
}

void GoodNight() {
  strip.clear();
  strip.show();
  clock.read();
  //BluethousSerial.println(clock.getHour());
  //lay(20);
  //BluethousSerial.println(clock.getMinute());
  BluethousSerial.println("Хозяин, когда изволите просыпаться?");

  //BluethousSerial.setTimeout(100500);
  while (!BluethousSerial.available()) {}
  set_hour = BluethousSerial.parseInt();
  while (!BluethousSerial.available()) {}
  set_minute = BluethousSerial.parseInt();


  /*while (!fla) {
    set_hour = BluethousSerial.parseInt();
    set_minute = BluethousSerial.parseInt();
    fla = BluethousSerial.parseInt();
    }*/
  BluethousSerial.print("Ваше время пробуждения установлено на: "); BluethousSerial.print(String(set_hour) + ":"); BluethousSerial.println(set_minute);
  BluethousSerial.println("Приятных снов");

  com transmition;
  transmition.comand = chg_mode;
  transmition.data[0] = 3;
#ifdef debug_bluet {
  BluethousSerial.print("Sent: "); BluethousSerial.println(transmition.comand);
#endif debug_bluet }
  radio.write(&transmition, sizeof(transmition));

  bri = 130;
  volum = 16;
  strip.setBrightness(bri);
  mp3.volume(volum);
  mp3.play(0003);
  loop_count = 0;
  last_time = millis();

  current_mode = GN_MODE;
}

void GM() {
  strip.clear();
  strip.show();
  com transmition;
  transmition.comand = chg_mode;
  transmition.data[0] = 4;
  radio.write(&transmition, sizeof(transmition));

#ifdef debug_bluet {
  BluethousSerial.print("Sent: "); BluethousSerial.println(transmition.comand);
#endif debug_bluet }
  current_mode = GM_MODE;
}

void Breakfast() {
  strip.clear();
  strip.show();
  com transmition;
  transmition.comand = chg_mode;
  transmition.data[0] = 6;
  radio.write(&transmition, sizeof(transmition));
  current_mode = BREAKFAST_MODE;
}
//
void Chill() {
  strip.clear();
  strip.show();
  com transmition;
  transmition.comand = chg_mode;
  transmition.data[0] = 7;
  radio.write(&transmition, sizeof(transmition));
  mp3.play(0001);
  current_mode = CHILL_MODE;
}

void Real_Time() {
  //  if (millis() - timer_4 >= Period_4) {    // условие таймера
  //    timer_4 = millis();
  //  }
  delay(1000);
  clock.read();
  //  BluethousSerial.println(String(clock.getHour() + ":" + (clock.getMinute())));
  BluethousSerial.print(clock.getHour());
  BluethousSerial.print(":");
  BluethousSerial.print(clock.getMinute());
  BluethousSerial.println();

}

void muzzik() {
  mp3.volume(16);
  mp3.play();
}

// Функция Огня
void fireTick() {
  static uint32_t prevTime;

  // двигаем пламя
  if (millis() - prevTime > 20) {
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

void seatick() {
  static uint32_t prevTime;

  // двигаем пламя
  if (millis() - prevTime > 20) {
    prevTime = millis();
    int thisPos = 0, lastPos = 0;
    FOR_i(0, NUM_LEDS) {
      leds[i] = getSeaColor((inoise8(i * FIRE_STEP, counter)));
    }
    counter += 20;
    strip.show();
  }
}

// возвращает цвет моря для одного пикселя
LEDdata getSeaColor(int val) {
  // чем больше val, тем сильнее сдвигается цвет, падает насыщеность и растёт яркость
  return mHSV(
           HUE_START_sea + map(val, 0, 255, 0, HUE_GAP),                    // H
           constrain(map(val, 0, 255, MAX_SAT, MIN_SAT), 0, 255),       // S
           constrain(map(val, 0, 255, MIN_BRIGHT, MAX_BRIGHT), 0, 255)  // V
         );
}

void foresttick() {
  static uint32_t prevTime;

  // двигаем пламя
  if (millis() - prevTime > 20) {
    prevTime = millis();
    int thisPos = 0, lastPos = 0;
    FOR_i(0, NUM_LEDS) {
      leds[i] = getForestColor((inoise8(i * FIRE_STEP, counter)));
    }
    counter += 20;
    strip.show();
  }
}

// возвращает цвет леса для одного пикселя
LEDdata getForestColor(int val) {
  // чем больше val, тем сильнее сдвигается цвет, падает насыщеность и растёт яркость
  return mHSV(
           HUE_START_forest + map(val, 0, 255, 0, HUE_GAP),             // H
           constrain(map(val, 0, 255, MAX_SAT, MIN_SAT), 0, 255),       // S
           constrain(map(val, 0, 255, MIN_BRIGHT, MAX_BRIGHT), 0, 255)  // V
         );
}

void embiend_tick() {
  static uint32_t prevTime;

  // двигаем пламя
  if (millis() - prevTime > 20) {
    prevTime = millis();
    int thisPos = 0, lastPos = 0;
    FOR_i(0, NUM_LEDS) {
      leds[i] = getEmbiendColor((inoise8(i * FIRE_STEP, counter)));
    }
    counter += 20;
    strip.show();
  }
}

// возвращает цвет леса для одного пикселя
LEDdata getEmbiendColor(int val) {
  // чем больше val, тем сильнее сдвигается цвет, падает насыщеность и растёт яркость
  return mHSV(
           HUE_START_Embiend + map(val, 0, 255, 0, HUE_GAP),            // H
           constrain(map(val, 0, 255, MAX_SAT, MIN_SAT), 0, 255),       // S
           constrain(map(val, 0, 255, MIN_BRIGHT, MAX_BRIGHT), 0, 255)  // V
         );
}

void Swet() {                  // Выбор фиксированных цветов
  //    BluethousSerial.println("5");
  /*
    switch (Rejim) {
      case 0: {       // Цвет морской волны
          R = 0;
          G = 255;
          B = 255;
          break;
        }
      case 1: {       // Цвет синий Крайола
          R = 31;
          G = 117;
          B = 254;
          break;
        }
      case 2: {       // Цвет синий Градуса
          R = 0;
          G = 125;
          B = 255;
          break;
        }
      case 3: {       // Цвет Бирюзовый
          R = 48;
          G = 213;
          B = 200;
          break;
        }
      case 4: {       // Цвет Аметистовый
          R = 153;
          G = 102;
          B = 204;
          break;
        }
      case 5: {       // Цвет Светло бирюзовый
          R = 64;
          G = 224;
          B = 208;
          break;
        }
      case 6: {       // Цвет Малиновый
          R = 220;
          G = 20;
          B = 60;
          break;
        }
      case 7: {      // Цвет Коричнево Бордовый
          R = 165;
          G = 42;
          B = 42;
          break;
        }
      case 8: {       // Цвет Красный Крайола
          R = 238;
          G = 32;
          B = 77;
          break;
        }
      case 9: {       // Цвет Рыжий
          R = 215;
          G = 125;
          B = 49;
          break;
        }
      case 10: {     // Цвет Шоколадный
          R = 210;
          G = 105;
          B = 30;
          break;
        }
      case 11: {
          R = 255;
          G = 0;
          B = 0;
          break;
        }
    }
  */

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
  com transmition;
  transmition.comand = chg_color;
  transmition.data[0] = R;
  transmition.data[1] = G;
  transmition.data[2] = B;

  //  BluethousSerial.print("Sent: "); BluethousSerial.println(transmition.comand);
  radio.write(&transmition, sizeof(transmition));
  //  radio.write(&transmition, sizeof(transmition));
  Rejim++;
  if (Rejim == 11) {
    Rejim = 0;
  }
}

void setup() {
  pinMode(buttonRFID, INPUT_PULLUP);
  pinMode(buttonVolumeUp, INPUT_PULLUP);
  pinMode(buttonVolumeDown, INPUT_PULLUP);

//  attachInterrupt(1, Alarm, RISING);

  // put your setup code here, to run once:
#ifdef debug_radio
  Serial.begin(9600);
#endif debug_radio
#ifdef debug_bluet
  BluethousSerial.begin(9600);
#endif debug_bluet
  mySoftwareSerial.begin(9600);
  //  strip.setBrightness(0);
  time_v_GM.setInterval(time_vol_GM);
  time_v_GN.setInterval(time_vol_GN);
  time_s_GM.setInterval(time_strip_GM);
  time_s_GN.setInterval(time_strip_GN);
  time_s_Buy.setInterval(time_strip_Buy);

  strip.clear();
  strip.show();

  clock.begin();
  clock.set(__TIMESTAMP__);

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
  if (!mp3.begin(mySoftwareSerial)) {
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
  mp3.setTimeOut(300);
  //----Set volume----
  mp3.volume(15); //Set volume value (0~30).
  //----Set different EQ----
  mp3.EQ(DFPLAYER_EQ_NORMAL);
  mp3.outputDevice(DFPLAYER_DEVICE_SD);
  //mp3.play(1); //Воспроизведение первого mp3

  mp3.playLargeFolder(2, 999);

  if (millis() - timer_2 >= Period_2) {
    timer_2 = millis();   // "сбросить" таймер
    // дейтвия, которые хотим выполнить один раз за период
  }

  isPlaying = true; // воспроизводим
#ifdef debug_sound
  Serial.println("Playing..");
  //----Читать информацию----
  Serial.println(mp3.readState()); //читать состояние mp3
  Serial.println(mp3.readVolume()); //Текущая громкость
  Serial.println(mp3.readEQ()); // читаем настройку эквалайзера
  Serial.println(mp3.readFileCounts()); // читать все файлы на SD-карте
  Serial.println(mp3.readCurrentFileNumber()); // текущий номер файла воспроизведения
#endif debug_sound
}

void norm_function_chill() {
  embiend_tick();
}

void norm_function_down() {
  if (loop_count < 100)
  {
    if (millis() - last_time > 1000)
    {
      bri -= 1;
      strip.setBrightness(bri);
      last_time = millis();
      loop_count++;
    }
  }
}

void norm_function_up() {
  if (loop_count < 90)
  {
    if (millis() - last_time > 100)
    {
      BluethousSerial.println(bri);
      bri += 1;
      strip.setBrightness(bri);
      strip.show();
      last_time = millis();
      loop_count++;
    }
  }
}

void norm_function_GN() {

  if ((volum > 0) || (bri > 0)) {
    fireTick();
    /*
    BluethousSerial.print("GN loop v=");
    BluethousSerial.print(volum);
    BluethousSerial.print(" b=");
    BluethousSerial.println(volum);
    */
    if ((time_v_GN.isReady())&&(volum > 0)) {
      volum--;
      mp3.volume(volum);
      if (volum == 0) {
        mp3.pause();
      }
    }
    if ((time_s_GN.isReady())&&(bri > 0)) {
      bri -= 1;
      strip.setBrightness(bri);
    }

  }
  else
  {
    clock.read();
    if ((set_hour == clock.getHour()) && (set_minute == clock.getMinute())) {
      BluethousSerial.println("yes");
      current_mode = WAKE_UP_MODE;
    }
  }
}

void norm_function_BreakFast() {
  fireTick();
}

void norm_function_GM() {
  if ((volum < 22) || (bri < 255)) {
    foresttick();
    if (time_v_GM.isReady()) {
      volum++;
      mp3.volume(volum);
      if (volum == 22) {
        mp3.pause();
      }
    }
    if (time_s_GM.isReady()) {
      bri++;
      strip.setBrightness(bri);
    }

  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //  Serial.println("Work");

#ifdef debug_bluet
  while (BluethousSerial.available()) {
    Bloup();
  }
#endif debug_bluet

  if (current_mode == LIGHT_RISING_MODE) {
    norm_function_up();
  }
  if (current_mode == GN_MODE) {
    norm_function_GN();
  }
  if (current_mode == CHILL_MODE) {
    norm_function_chill();
  }
  if (current_mode == BREAKFAST_MODE) {
    norm_function_BreakFast();
  }
  if (current_mode == GM_MODE) {
    norm_function_GM();
  }
  if (current_mode == WAKE_UP_MODE) {
    GM();
  }
}
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
