// Выходы к которым подключаться, лучше без необходимости не менять
#define driver1_output 5  // К первому мотору ЛЕВЫЙ!
#define driver2_output 6  // Ко второму мотору ПРАВЫЙ!
#define chanel1_input 2  // 1 канал на пульте (белый провод)
#define chanel2_input 3  // 2 канал на пульте (белый провод)
#define Hall A3  // Датчик Холла ручки газа

// Константы для точной настройки, можно менять
float const max_Hall_voltage = 5.0;  // Максимальное напряжение с датчика Холла, поменять для большей чувствительности
byte const low_shim_time = 127;  // Теоретически это 1мс
byte const high_shim_time = 255;  // Теоретически это 2 мс
int const pulse_max = 2000;  // мкс, максимальное значение с пульта
int const pulse_min = 1000;  // мкс, минимальное значение с пульта
int const pulse_const = 1500;  // Нулевое положение джостика, нужно для поворотов влево и вправо. Если я продолбался - поменять


// переменные ниже не трожь!
float Hall_voltage = 0;  // Переменная для значения напряжения с датчика Холла
int Pulses_chanel1 = 0;
int Pulses_chanel2 = 0;

volatile long current_time_chanel1 = 0;  // текущее значение времени с начала работы на 1 канале
volatile long current_time_chanel2 = 0;  // текущее значение времени с начала работы на 2 канале
volatile long start_time_chanel1 = 0;  // стартовое значение временных задержек в ШИМ на 1 канале
volatile long start_time_chanel2 = 0;  // стартовое значение временных задержек в ШИМ на 2 канале

int Pulse_width_chanel1 = 0;  // если ругается дописать volatile перед строкой
int Pulse_width_chanel2 = 0;  // если ругается дописать volatile перед строкой

void setup() {
// следующую строчку при финальной прошивке удалить
  Serial.begin(9600);

  pinMode(driver1_output, OUTPUT);
  pinMode(driver2_output, OUTPUT);
  pinMode(chanel1_input, INPUT_PULLUP);
  pinMode(chanel2_input, INPUT_PULLUP);
  pinMode(Hall, INPUT_PULLUP);

attachInterrupt(digitalPinToInterrupt(chanel1_input), pulse_timer_cannel1, CHANGE);
attachInterrupt(digitalPinToInterrupt(chanel2_input), pulse_timer_cannel2, CHANGE);


}

void loop() {
  Hall_trigger();

  if (Pulses_chanel1 || Pulses_chanel2 != 0){
    sum_shim(Pulses_chanel1, Pulses_chanel2);
  }
  // Вот эта хрень для отладки через комп, при финальной прошивке удалить
  if (Pulses_chanel1 < 2000){
    Pulse_width_chanel1 = Pulses_chanel1;
  } 
  if (Pulses_chanel2 < 2000){
    Pulse_width_chanel2 = Pulses_chanel2;
  } 
  Serial.print(Pulse_width_chanel1);
  Serial.print("    ");
  Serial.println(Pulse_width_chanel2);


}

void pulse_timer_cannel1(){
  current_time_chanel1 = micros();
  if (current_time_chanel1 > start_time_chanel1){
    Pulses_chanel1 = current_time_chanel1 - start_time_chanel1;
    start_time_chanel1 = current_time_chanel1;
  }
}

void pulse_timer_cannel2(){
  current_time_chanel2 = micros();
  if (current_time_chanel2 > start_time_chanel2){
    Pulses_chanel2 = current_time_chanel2 - start_time_chanel2;
    start_time_chanel2 = current_time_chanel2;
  }
}

int sum_shim(int Pulses_chanel1,int Pulses_chanel2){  //  Очень не надёжная конструкция, здесь наверняка будет больше всего проблем
  int left = 0;
  int right = 0;
  if (Pulses_chanel2 > pulse_const){
    right = Pulses_chanel1 + (Pulses_chanel2 - pulse_const);
    left = Pulses_chanel1;
  } else if(Pulses_chanel2 < pulse_const){
    right = Pulses_chanel1;
    left = Pulses_chanel1 + (pulse_const - Pulses_chanel2);
  } else if (Pulses_chanel2 == pulse_const){
    right = Pulses_chanel1;
    left = Pulses_chanel1;
  }
  if (left > pulse_max) left = pulse_max;
  if (right > pulse_max) right = pulse_max;
  analogWrite(driver1_output, left);
  analogWrite(driver1_output, right);
}

void Hall_trigger(){
  Hall_voltage = analogRead(Hall);
  Hall_voltage = map(Hall_voltage, 0, 1023, 0, 255);
  Hall_voltage = constrain(Hall_voltage, 0, 255);
  analogWrite(driver1_output, Hall_voltage);
  analogWrite(driver2_output, Hall_voltage);
}