/*
Дозатор налива литров в минуту
Подключения:
Пин D2 вход счетчика
Пин D3 енкодер
Пин D4 енкодер
Пин D5 кнопка енкодера
Пин D6 шим выход на управление двигателем
Пин D7 направление вращения двигателя
Пин D8 направление вращения двигателя
Пин A4 дисплей
Пин A5 дисплей
*/
#include <EEPROM.h>             // Библиотека для работы с ПЗУ
#include <GyverTimer.h>         // Библиотека таймера
#include <LiquidCrystal_I2C.h>  //Библиотека экрана

LiquidCrystal_I2C lcd(0x27, 16, 2);  // устанавливаем адрес 0x27, и дисплей 20 символов в 4 строки (20х4)
#include <EncButton.h>               //Библиотека енкодера
EncButton eb(3, 4, 5);               // энкодер с кнопкой <A, B, KEY>


GTimer Timer_Disp(MS, 1000);  //Таймер обновления экрана
GTimer Timer_Motor(MS, 200);  //Таймер обновления мотора

#define motor_EN 6  //Пин подключения мотора EN
#define L_PWM 7     //Пин подключения мотора L_PWM
#define R_PWM 8     //Пин подключения мотора R_PWM

volatile int flow_frequency;   // измеряет частоту
float l_minute;                // рассчитанные литр/минуту
unsigned char flowsensor = 2;  // Вход сенсора
unsigned long currentTime;
unsigned long cloopTime;
int speed = 125;
bool status;
float litr_m;

void flow()  // функция прерывания
{
  flow_frequency++;
}

void setup() {
  EEPROM.put(1, 30.0);
  pinMode(flowsensor, INPUT);
  pinMode(motor_EN, OUTPUT);  // Конфигурируем вывод EN    как выход (выход Arduino, вход драйвера)
  pinMode(L_PWM, OUTPUT);     // Конфигурируем вывод L_PWM как выход (выход Arduino, вход драйвера)
  pinMode(R_PWM, OUTPUT);     // Конфигурируем вывод R_PWM как выход (выход Arduino, вход драйвера)
  digitalWrite(flowsensor, HIGH);
  digitalWrite(motor_EN, LOW);  // Устанавливаем логический 0 на входах драйвера L_EN и R_EN, значит выходы M+ и M- перейдут в состояние высокого импеданса и мотор будет электрически отключён.
  Serial.begin(9600);

  attachInterrupt(0, flow, RISING);  // настраиваем прерывания
  sei();                             // активируем прерывания
  currentTime = millis();
  cloopTime = currentTime;

  EEPROM.get(0, status);

  if ((status != 0) || (status != 1)) {
    status = 0;
    EEPROM.put(0, status);
  }
  EEPROM.get(1, litr_m);
  if (litr_m > 60.0) {
    litr_m = 30.0;
    EEPROM.put(1, litr_m);
  }

  lcd.init();           // инициализация LCD
  lcd.backlight();      // включаем подсветку
  lcd.clear();          // очистка дисплея
  lcd.setCursor(0, 0);  // устанавливаем курсор
  lcd.print("     Dozator    ");
  lcd.setCursor(0, 1);  // устанавливаем курсор
  lcd.print("V.1.0 08/04/2024");
  delay(3000);
  lcd.clear();
}

void loop() {
  eb.tick();
  Encoder();
  if (Timer_Disp.isReady()) Display();

  if (Timer_Motor.isReady()) {
    if (status == 1) {
      digitalWrite(L_PWM, LOW);   // Устанавливаем логический 0 на входе драйвера L_PWM, значит на выходе драйвера M- будет установлен потенциал S-
      digitalWrite(R_PWM, HIGH);  // Устанавливаем логическую 1 на входе драйвера R_PWM, значит на выходе драйвера M+ будет установлен потенциал S+

      if (l_minute > litr_m) {
        if (speed > 0) --speed;
      }
      if (l_minute < litr_m) {
        if (speed < 255) ++speed;
      }
      analogWrite(motor_EN, speed);
    }
    if (status == 0) analogWrite(motor_EN, 0);
    if (status == 0) digitalWrite(motor_EN, LOW);  // Устанавливаем логический 0 на входах драйвера L_EN и R_EN, значит выходы M+ и M- перейдут в состояние высокого импеданса и мотор будет электрически отключён.
  }

  currentTime = millis();
  // Каждую секунду рассчитываем и выводим на экран литры в минуту
  if (currentTime >= (cloopTime + 1000)) {
    cloopTime = currentTime;  // Обновление cloopTime
    // Частота импульсов (Гц) = 7.5Q, Q - это расход в л/мин.
    l_minute = (flow_frequency / 7.5);  // Частота  / 7.5Q = расход в л/минутах
    flow_frequency = 0;                 // Сбрасываем счетчик
    Serial.print(l_minute, DEC);        // Отображаем л/мин
    Serial.println(" L/minute");
  }
}

