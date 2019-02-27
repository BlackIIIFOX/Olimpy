#include "FIFO.h"
//========================Передача данных=======================================
// Variables
// Специальные операции
#define ILLEGAL_FUNCTION 0x01     // Код функции не поддерживается
#define SLAVE_DEVICE_FAILURE 0x02 // Произошла ошибка при выполнении операции
#define ILLEGAL_DATA_VALUE 0x03   // Данные неккоректны(CRC)
#define RECEIVED_DATA 0x04        // Данные получены
#define TEST_CONNECT 0x05         // Тест соединения 
#define START_OPERATIONS 0x06     // Начало работы второго робота
#define RED_ON 0x07               // Зажечь красный светодиод
#define GREEN_ON 0x08             // Зажечь зеленый светодиод
#define LED_OFF 0x09              // Выключить светодиод
#define REQUEST_LOAD_CARGO 0x0A   // Запрос погрузки
#define CONFIRM_LOAD_CARGO 0x0B   // Подтвержение начала погрузки
#define END_LOAD_CARGO 0x0C       // Завершение погрузки
#define CYLINDER_COLORS 0x0D       // Передача цветов целиндров

FIFO BufferInput;
unsigned long lastReceive = millis();
int sizePackage = 0;

// Methods
void ReceiveData(); // Принятие данных
void SendData(uint8_t operation, uint8_t* data, int data_len = 0);  // Передача данных
uint8_t crc8(const uint8_t *addr, uint8_t len); // Расчет CRC8

//===============================================================================

uint8_t CountCylinderColors[6] = {1, 2, 3, 4, 5, 6};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  ReceiveData();
  // SendData(ILLEGAL_FUNCTION, nullptr);
  // SendData(ILLEGAL_FUNCTION, nullptr);
  // SendData(CYLINDER_COLORS, CountCylinderColors, 6);
  delay(2000);
}


//========================Передача данных=======================================
void HandlePackage()
{
  uint8_t data[BufferInput.size() - 1];
  int i = 0;
  while(BufferInput.size() > 1)
  {
    data[i] = BufferInput.pop();
    i++;
  }

  // Если crc8 верный
  if(crc8(data, i) == BufferInput.pop())
  {
    // Обработка пришедших команд
    SendData(RECEIVED_DATA, nullptr);
  }
  else
  {
    SendData(ILLEGAL_DATA_VALUE, nullptr);
  }
}

void ReceiveData()
{
  uint8_t incomingByte;
  if (Serial.available() > 0) {  //если есть доступные данные
    // считываем байт
    if(millis() - lastReceive > 500)
    {
      BufferInput.Clear();
      sizePackage = 0;
    }

    if(sizePackage == 0)
    {
      sizePackage = Serial.read();
    }
    
    while(Serial.available() > 0)
    {
      BufferInput.push(Serial.read());
    }
    
    if(sizePackage == BufferInput.size())
    {
      HandlePackage();
    }
    lastReceive = millis();
  }
}

void SendData(uint8_t operation, uint8_t* data = nullptr, int data_len = 0)
{
  uint8_t data_send[data_len + 3];
  data_send[0] = data_len + 2;

  data_send[1] = operation;
  for (int i = 0; i < data_len; i++)
  {
    data_send[i + 2] = data[i];
  }
  data_send[data_len + 2] = crc8(data_send + 1, data_len + 1);
  Serial.write(data_send, data_len + 3);
}

uint8_t crc8(const uint8_t *addr, uint8_t len) {
  uint8_t crc = 0;
  while (len--) {
    uint8_t inbyte = *addr++;
    for (uint8_t i = 8; i; i--) {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}
//===============================================================================
