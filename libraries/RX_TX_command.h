struct com {
  byte comand;          // Принимает значение команд
  byte data[3];         // 
};

#define chg_mode 10             // Выбирает режим работы:   [st, peri] Hellou, GoodNight, GM, USSR, Breakfast, Chill 
#define chg_color 20            // Отправляет режим значение [R, G, B]
#define chg_bri 21              // Отправляет изменение яркости
#define chg_rosetka 30          // Принимает знаечение status включить/выключить розетку
#define chg_serv 40             // Отправляет режим работы сервы [v, stop_close] (скорость закрытие, максимальное закрытие)

