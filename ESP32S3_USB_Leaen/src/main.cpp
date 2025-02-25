/*
 * GPIO 0, 2, 4, 5, 12, 15, 46ʹ��ʱҪע���ϵ�ʱ���ŵ�ƽ
 * GPIO 19, 20ΪUSB����
 */

/*   ��ˢ�������Ŷ���
 *   LO  NC INT GND  HI
 *   |   |   |   |   |
 *
 *   |   |       |   |
 *   NC  NC    ����ˮGND
 */

/*
 *       ����������������������������������������������������
         ��3V3                  GND��
         ��3V3             X  U0TXD��
         ��RST             X  U0RXD��
         ��GPIO4  X           GPIO1��13
         ��GPIO5  X        X  GPIO2��
       14��GPIO6             GPIO42��12
       15��GPIO7             GPIO41��11
         ��GPIO15 X          GPIO40��10
       16��GPIO16            GPIO39��9
       17��GPIO17            GPIO38��8
       18��GPIO18            GPIO37��7
       19��GPIO8             GPIO36��6
      *20��GPIO3             GPIO35��5
         ��GPIO46 X        X  GPIO0��
      *21��GPIO9             GPIO45��4
      *22��GPIO10            GPIO48��3
      *  ��GPIO11            GPIO47��2
         ��GPIO12 X          GPIO21��1
      *23��GPIO13          X USB D+��
      *23��GPIO14          X USB D-��
         ��5Vin                 GND��
         ��GND                  GND��
         ��                        ��
         ��  ��������������      ��������������  ��
         ��  ��     ��      ��     ��  ��
         �������ة����������ة������������ة����������ة�����

 *  ��ť���Ŷ���
 *  GPIO14 ->> 23 //ʾ����
 *  GPIO13 ->> 23 //���
 *  GPIO11 ->>    //�����HI
 *  GPIO10 ->>    //�����LO
 *  GPIO9  ->>    //�����INT
 *         ->> 22 //���������
 *         ->> 21 //���������
 *  GPIO3  ->> 20 //������
 *  ע���еİ�ťȫ��Ϊʵ�ʰ�ť���Ǽ�ֵ+1
 */

#include <Arduino.h>
#include "USB.h"
#include "USBHIDGamepad.h"

USBHIDGamepad Gamepad;

#define POSITION_LAMP_PIN 14  // GPIO14 ->> btn23 ʾ����
#define HEADLIGHT_PIN 13      // GPIO13 ->> btn23 ���
#define WIPER_HI_PIN 11       // GPIO11 ->>       �����HI
#define WIPER_LO_PIN 10       // GPIO10 ->>       �����LO
#define WIPER_INT_PIN 9       // GPIO9  ->>       �����INT
#define OVERTAKING_LAMP_PIN 3 // GPIO3  ->> btn20 ������

enum wiperState
{
  WIPER_OFF,
  WIPER_INT,
  WIPER_LO,
  WIPER_HI,
};

enum lightState
{
  LIGHT_OFF,
  LIHGT_POSITION,
  LIGHT_HEAD,
};

enum overtakingLampState
{
  OVERTAKING_LAMP_OFF,
  OVERTAKING_LAMP_ON,
};

wiperState currentWiperState = WIPER_OFF;
lightState currentLightState = LIGHT_OFF;
bool currentOvertakingLampState = OVERTAKING_LAMP_OFF;


const uint8_t INPUT_PINS[] = {21, 47, 48, 45, 35, 36, 37, 38, 39, 40, 41, 42, 1, 6, 7, 16, 17, 18, 8, 3, 9, 10, 11, 13, 14};
const uint8_t NUMBER_OF_INPUT_PINS = sizeof(INPUT_PINS) / sizeof(INPUT_PINS[0]);
const uint8_t NUMBER_OF_GENERAL_BUTTONS = NUMBER_OF_INPUT_PINS - 6;
bool lastButtonState[NUMBER_OF_GENERAL_BUTTONS] = {HIGH};

uint8_t convertToActualButton(uint8_t ActualButton)
{
  return ActualButton++; // ʵ�ʰ�ť = ��ֵ +1
}

wiperState getWiperState(void)
{
  if (digitalRead(WIPER_INT_PIN) == HIGH && digitalRead(WIPER_LO_PIN) == HIGH && digitalRead(WIPER_HI_PIN) == HIGH)
  // �����ΪOFF��λ
  {
    return WIPER_OFF;
  }
  else if (digitalRead(WIPER_INT_PIN) == LOW && digitalRead(WIPER_LO_PIN) == HIGH && digitalRead(WIPER_HI_PIN) == HIGH)
  // �����ΪINT��λ
  {
    return WIPER_INT;
  }
  else if (digitalRead(WIPER_INT_PIN) == HIGH && digitalRead(WIPER_LO_PIN) == LOW && digitalRead(WIPER_HI_PIN) == HIGH)
  // �����ΪLO��λ
  {
    return WIPER_LO;
  }
  else if (digitalRead(WIPER_INT_PIN) == HIGH && digitalRead(WIPER_LO_PIN) == HIGH && digitalRead(WIPER_HI_PIN) == LOW)
  // �����ΪHI��λ
  {
    return WIPER_HI;
  }
  else
  {
    return WIPER_OFF;
  }
}

void wiperGearUp(void)
{
  Gamepad.pressButton(convertToActualButton(21)); // btn21 ���������
  vTaskDelay(pdMS_TO_TICKS(20));
  Gamepad.releaseButton(convertToActualButton(21));
}

void wiperGearDown(void)
{
  Gamepad.pressButton(convertToActualButton(22)); // btn22 ���������
  vTaskDelay(pdMS_TO_TICKS(20));
  Gamepad.releaseButton(convertToActualButton(22));
}

void syncWiperState(wiperState targetState)
{
  while (targetState > currentWiperState) // Ŀ�굵λ���ڵ�ǰ��λ��Ӧ����
  {
    wiperGearUp();
    currentWiperState = static_cast<wiperState>((currentWiperState + 1) % 4); // ��ǰ��λ+1��ȡ��4����ֹ���
    Serial.println("wiperGearUp");
    Serial.printf("currentWiperState: %d, targetState: %d\n", currentWiperState, targetState);
  }
  while (targetState < currentWiperState) // Ŀ�굵λС�ڵ�ǰ��λ��Ӧ����
  {
    wiperGearDown();
    currentWiperState = static_cast<wiperState>((currentWiperState - 1) % 4); // ��ǰ��λ-1��ȡ��4����ֹ���
    Serial.println("wiperGearDown");
    Serial.printf("currentWiperState: %d, targetState: %d\n", currentWiperState, targetState);
  }
}

lightState getLightState(void)
{
  if (digitalRead(POSITION_LAMP_PIN) == HIGH && digitalRead(HEADLIGHT_PIN) == HIGH)
  // ��ƺ�ʾ���ƶ��ر�
  {
    return LIGHT_OFF;
  }
  else if (digitalRead(POSITION_LAMP_PIN) == LOW && digitalRead(HEADLIGHT_PIN) == HIGH)
  // ʾ���ƿ���
  {
    return LIHGT_POSITION;
  }
  else if (digitalRead(POSITION_LAMP_PIN) == LOW && digitalRead(HEADLIGHT_PIN) == LOW)
  // ʾ���ƺʹ�ƾ�����
  {
    return LIGHT_HEAD;
  }
  else
  {
    return LIGHT_OFF;
  }
}

void lightGearChange(void)
{
  Gamepad.pressButton(convertToActualButton(23)); // btn23 ʾ����
  vTaskDelay(pdMS_TO_TICKS(20));
  Gamepad.releaseButton(convertToActualButton(23));
}

void syncLightState(lightState targetState) // ͬ����Ƶ�λ
{
  while (targetState != currentLightState)
  {
    lightGearChange();
    currentLightState = static_cast<lightState>((currentLightState + 1) % 3); // ��ǰ��λ+1��ȡ��3����ֹ���, ֻ��һ����ť��ѭ������
  }
}

overtakingLampState getOvertakingLampState(void) // ��ȡ������״̬
{
  if (digitalRead(OVERTAKING_LAMP_PIN) == LOW) // ��������Ʊ�����
  {
    return OVERTAKING_LAMP_ON;
  }
  else
  {
    return OVERTAKING_LAMP_OFF;
  }
}

void syncOvertakingLampState(overtakingLampState targetState) // ͬ��������״̬
{
  if (targetState != currentOvertakingLampState) // ���д������ڷ�ֹ����ͬ��������״̬��ֻ�п���״̬�ı�Ž���ͬ��
  {
    if (targetState == OVERTAKING_LAMP_ON)
    {
      Gamepad.pressButton(convertToActualButton(20)); // btn20 ������
      Serial.println("overtaking lamp ON");
    }
    else
    {
      Gamepad.releaseButton(convertToActualButton(20));
      Serial.println("overtaking lamp off");
    }
    currentOvertakingLampState = targetState;
  }
}

void overtakingLamp(void *pvParameters) // �����ƿ��ƽ���
{
  while (1)
  {
    overtakingLampState targetState = getOvertakingLampState();
    if (targetState != currentOvertakingLampState)
    {
      syncOvertakingLampState(targetState); 
    }
    
    // if (digitalRead(OVERTAKING_LAMP_PIN) == LOW)
    // {
    //   syncOvertakingLampState(OVERTAKING_LAMP_ON);
    // }
    // else
    // {
    //   syncOvertakingLampState(OVERTAKING_LAMP_OFF);
    // }
    vTaskDelay(1); // ��ֹ���̿���
  }
}

void generalButton(void *pvParameters) // ͨ�ð�ť���ƽ���
{
  while (1)
  {
    for (int i = 0; i < NUMBER_OF_GENERAL_BUTTONS; i++)
    {
      bool buttonState = digitalRead(INPUT_PINS[i]); // ��ȡ��ť״̬
      if (buttonState != lastButtonState[i])         // �����ǰ��ť״̬����һ�β�һ��
      {
        if (buttonState == LOW) // ����ť������
        {
          Gamepad.pressButton(i);
          vTaskDelay(pdMS_TO_TICKS(20));
          Gamepad.releaseButton(i); // ����һ�ο�������ť
          Serial.println("Button " + String(i) + " pressed");
        }
        else // ����ť��̧��
        {
          Gamepad.pressButton(i);
          vTaskDelay(pdMS_TO_TICKS(20));
          Gamepad.releaseButton(i); // ����һ�ο�������ť
          Serial.println("Button " + String(i) + " released");
        }
      }
      lastButtonState[i] = buttonState; // ������һ�ΰ�ť״̬
    }
    vTaskDelay(1); // ��ֹ���̿���
  }
}

void wiper(void *pvParameters) // �������������
{
  while (1)
  {
    wiperState targetState = getWiperState();
    if (targetState != currentWiperState)
    {
      syncWiperState(targetState);
    }
    vTaskDelay(1); // ��ֹ���̿���
  }
}

void light(void *pvParameters) // ��ƿ��ƽ���
{
  while (1)
  {
    lightState targetLightState = getLightState();
    if (targetLightState != currentLightState)
    {
      syncLightState(targetLightState);
    }
    vTaskDelay(1); // ��ֹ���̿���
  }
}

void setup()
{
  Gamepad.begin();
  USB.begin();
  Serial.begin(115200);
  for (int i = 0; i < NUMBER_OF_INPUT_PINS; i++)
  {
    pinMode(INPUT_PINS[i], INPUT_PULLUP);
  }
  vTaskDelay(pdMS_TO_TICKS(2000));
  xTaskCreate(generalButton, "double trigger", 1024 * 10, NULL, 1, NULL);
  xTaskCreate(wiper, "control wiper", 1024 * 10, NULL, 1, NULL);
  xTaskCreate(light, "control light", 1024 * 10, NULL, 1, NULL);
  xTaskCreate(overtakingLamp, "control overtaking lamp", 1024 * 12, NULL, 1, NULL);
}

void loop()
{
  vTaskDelete(NULL);
}
