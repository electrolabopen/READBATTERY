
// Este ejemplo recibe el valor de la bateria de una Cubecell via Lora y la muestro en pantalla.

#include "Arduino.h"
#include "LoRaWan_APP.h"
//agregadas nuevas para la prueba
 
#include "HT_SSD1306Wire.h"



//Configuración de Parametros
#define RF_FREQUENCY                                915000000 // Hz

#define TX_OUTPUT_POWER                             14        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

int16_t txNumber;

int16_t rssi,rxSize;

bool lora_idle = true;

//agrego constructor
SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst



void setup() {
  Serial.begin(115200);
  Mcu.begin();
  factory_display.init(); // Inicializa la pantalla OLED
  factory_display.flipScreenVertically(); // Voltea la pantalla OLED (opcional)
  factory_display.setFont(ArialMT_Plain_16); // Establece la fuente de texto
  factory_display.drawString(0, 0, "Esperando dato"); // Muestra el mensaje "Hola mundo!" en la pantalla OLED
  factory_display.display(); // Muestra el contenido de la pantalla OLED
    
    txNumber=0;
    rssi=0;
  
    RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

}


void loop() {
  if(lora_idle)
  {
    lora_idle = false;
    Serial.println("into RX mode");
    Radio.Rx(0);
  }
  Radio.IrqProcess( );

}


void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
  
rssi=rssi;

rxSize=size;
memcpy(rxpacket, payload, size );
rxpacket[size]='\0';
Radio.Sleep( );
Serial.printf("\r\nPaquete Recibido \"%s\" con rssi %d , longitud %d\r\n",rxpacket,rssi,rxSize);
//lora_idle = true;

factory_display.clear(); // Limpia la pantalla OLED

factory_display.drawString(0, 0, "RSSI:");
factory_display.drawString(0, 12, String(rssi));
factory_display.display(); // Muestra el contenido de la pantalla OLED
delay(2000);
factory_display.clear();


    



int atrValue;



// Busca "ATR" y extrae su valor
char* atrStart = strstr(rxpacket, "ATR");
if (atrStart != NULL) {
  sscanf(atrStart, "ATR%d", &atrValue);
}





Serial.print("ATR: ");
Serial.println(atrValue);









factory_display.clear();
factory_display.drawString(0, 0, "Volt: ");
factory_display.drawString(40, 0, String(atrValue)); // 0,12
factory_display.drawString(75, 0, " (mV)");

factory_display.display();
delay(3000);
factory_display.clear();


lora_idle = true;

}
