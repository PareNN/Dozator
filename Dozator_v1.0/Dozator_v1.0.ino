/*
Дозатор налива литров в минуту
Подключения:
Пин D2 вход счетчика
Пин D3 енкодер
Пин D4 енкодер
Пин D5 кнопка енкодера
Пин D6 шим выход на управление двигателем
Пин A4 дисплей
Пин A5 дисплей
*/
#include <EEPROM.h>                  // Библиотека для работы с ПЗУ
#include <GyverTimer.h>              // Библиотека таймера
#include <LiquidCrystal_I2C.h>       //Библиотека экрана
LiquidCrystal_I2C lcd(0x27, 16, 2);  // устанавливаем адрес 0x27, и дисплей 20 символов в 4 строки (20х4)
#include <EncButton.h>               //Библиотека енкодера
EncButton eb(3, 4, 5);               // энкодер с кнопкой <A, B, KEY>


GTimer Timer_Disp(MS, 1000);   //Таймер обновления экрана
GTimer Timer_Motor(MS, 1000);  //Таймер мотора

volatile int flow_frequency;   // измеряет частоту
int l_minute;                  // рассчитанные литр/час
unsigned char flowsensor = 2;  // Вход сенсора
#define motor 6                //Пин подключения мотора
unsigned long currentTime;
unsigned long cloopTime;


int speed = 125;
int litr_m = EEPROM.read(1);
bool status = EEPROM.read(0);


void flow()  // функция прерывания
{
  flow_frequency++;
}

void setup() {
  pinMode(flowsensor, INPUT);
  pinMode(motor, OUTPUT);
  digitalWrite(flowsensor, HIGH);
  Serial.begin(9600);

  attachInterrupt(0, flow, RISING);  // настраиваем прерывания
  sei();                             // активируем прерывания
  currentTime = millis();
  cloopTime = currentTime;

  if ((status != 0) || (status != 1)) EEPROM.write(0, 0);
  if (litr_m > 60) EEPROM.write(1, 30);

  lcd.init();           // инициализация LCD
  lcd.backlight();                // включаем подсветку
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

  currentTime = millis();

  // Каждую секунду рассчитываем и выводим на экран литры в час
  if (currentTime >= (cloopTime + 1000)) {
    cloopTime = currentTime;  // Обновление cloopTime
    // Частота импульсов (Гц) = 7.5Q, Q - это расход в л/мин.
    l_minute = (flow_frequency / 7.5);  // Частота  / 7.5Q = расход в л/минутах
    flow_frequency = 0;                 // Сбрасываем счетчик
    Serial.print(l_minute, DEC);        // Отображаем л/мин
    Serial.println(" L/minute");


    if (status == 1) {

      if (Timer_Motor.isReady()) {
        if (l_minute > litr_m) {
          if (speed > 0) --speed;
        }

        if (l_minute < litr_m) {
          if (speed < 255) ++speed;
        }
        analogWrite(motor, speed);
      }
    }
    if (status == 0) analogWrite(motor, 0);
  }
}
