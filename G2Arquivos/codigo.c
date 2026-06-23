https://docs.google.com/document/d/1_SlcHYBJQVeAbvrkxkNduAIEecrZ-06KLPzVps7wGCM/edit?usp=sharing


#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#define PIR_PIN 26


LiquidCrystal_I2C lcd(0x3F,16,2);


// ========== WIFI ==========

#define WIFI_SSID "CLARO_2G95D116"
#define WIFI_PASSWORD "w9KKU98aky"


// ========== TELEGRAM ==========

#define BOT_TOKEN "8805532843:AAHuQ0PXYmeZdkbGHKWBdhm-uU37P_N6AXc"

#define CHAT_ID "5077034082"



WiFiClientSecure client;

UniversalTelegramBot bot(
  BOT_TOKEN,
  client
);



// ========== TEMPOS ==========

const unsigned long esperaInicial = 5000; 
// espera 5 segundos


const unsigned long tempoAlarme = 3000;
// mensagem fica 3 segundos


const unsigned long intervaloTelegram = 20000;
// 20 sec entre alertas



unsigned long inicioSistema;

unsigned long inicioAlarme;

unsigned long ultimoAlerta = 0;



bool sistemaAtivo = false;

bool mostrandoAlarme = false;



// ========== LCD ==========


void mostrarSemMovimento(){

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Sem movimento");

}



void mostrarAlarme(){

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Alarme");

  lcd.setCursor(0,1);
  lcd.print("acionado");

}



// ========== TELEGRAM ==========


void enviarTelegram(){


  String mensagem =
  "🚨 ALERTA!\nMovimento detectado.";


  bot.sendMessage(
    CHAT_ID,
    mensagem,
    ""
  );


  Serial.println("Telegram enviado");


}



// ========== SETUP ==========


void setup(){


  Serial.begin(115200);


  pinMode(
    PIR_PIN,
    INPUT_PULLDOWN
  );



  lcd.init();

  lcd.backlight();



  lcd.clear();

  lcd.print("Conectando WiFi");



  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );



  while(WiFi.status()!=WL_CONNECTED){

    delay(500);

    Serial.print(".");

  }



  Serial.println();

  Serial.println("WiFi conectado");



  lcd.clear();

  lcd.print("WiFi conectado");

  delay(2000);



  lcd.clear();

  lcd.print("Sistema pronto");

  delay(2000);



  mostrarSemMovimento();



  // inicia espera de segurança do PIR

  inicioSistema = millis();



  // necessário para HTTPS do Telegram

  client.setInsecure();


}



// ========== LOOP ==========


void loop(){



  // espera inicial de 5 segundos

  if(!sistemaAtivo){


    if(
      millis()-inicioSistema >= esperaInicial
    ){

      sistemaAtivo = true;

      Serial.println("Sistema ativo");

    }
    else{

      return;

    }

  }




  // mantém mensagem de alarme

  if(mostrandoAlarme){


    if(
      millis()-inicioAlarme >= tempoAlarme
    ){

      mostrandoAlarme = false;

      mostrarSemMovimento();

    }


    return;

  }





  // leitura do PIR

  if(
    digitalRead(PIR_PIN)==HIGH
  ){


    Serial.println("Movimento detectado");



    mostrarAlarme();



    inicioAlarme = millis();

    mostrandoAlarme = true;



    // primeiro alerta imediato
    // depois espera 1 minuto

    if(
      ultimoAlerta == 0 ||
      millis()-ultimoAlerta >= intervaloTelegram
    ){


      enviarTelegram();


      ultimoAlerta = millis();


    }


  }



}
