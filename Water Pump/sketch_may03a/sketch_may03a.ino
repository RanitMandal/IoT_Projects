// For Home automation in room(Personal Uses)
// Ranit Mandal
//  Select Board -->> NodeMCU 0.9 (ESP-12 Module)*****

#define BLYNK_TEMPLATE_ID "TMPL3cVlfXbpa"
#define BLYNK_TEMPLATE_NAME "Automatic Water Pump"
#define BLYNK_AUTH_TOKEN "y5DSDGehVQGyyFSiuoy7Jt1LwR_-68dN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>

#define BLYNK_PRINT Serial


char auth[] = BLYNK_AUTH_TOKEN;

const char *ssid = "Ranit_Home";
const char *pass = "Ranit@2424";

//const char *ssid = "Personal";
//const char *pass = "Admin1234";



// define the GPIO connected with updateOutput and switches

#define WLevel_Input0  D1 //GPIO 16    ** Water Level Analog Value 25%  **
#define WLevel_Input1  D2 //GPIO 5     ** Water Level Analog Value 50%  **
#define WLevel_Input2  D3 //GPIO 4     ** Water Level Analog Value 75%  **
#define WLevel_Input3  D5 //GPIO 0     ** Water Level Analog Value 100% **

#define wifiLed D4 //GPIO 2   ** Wifi Connected Indicater **

#define Switch1 D6 //SD3     ** Motor Switch **
#define Switch2 D7 //SD2     ** Buzzer Switch **

#define MotorOutput D0 //D5      ** Motor Output **
#define BuzzerOutput D8 //SD3     ** Buzzer Output ** 


unsigned long previousMillis = 0;
unsigned long interval = 30000;

short int MotorValue = 0, Auto_Manual_Value = 0, BuzzerValue = 0;
short int wifiFlag = 1;
short int WaterLevelPercentage = 0,BuzzerCounter = 0;

BlynkTimer timer;


// called every 3 seconds by SimpleTimer
void checkBlynkStatus() {
  // Serial.println("Called functuon");
  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    digitalWrite(wifiLed, HIGH); //Turn off WiFi LED
  }
  if (isconnected == true) {
    if (wifiFlag == 1) {
      wifiFlag = 0;
      update_blynk();
    }
    digitalWrite(wifiLed, LOW); //Turn on WiFi LED
  }
}


// Blyink Functions
BLYNK_WRITE(V0)
{
  Auto_Manual_Value = param.asInt();
  if (MotorValue != 0 && Auto_Manual_Value == 1) {
    MotorValue = 0;
  }
  Serial.print("Auto_Manual_Value = ");
  Serial.println(Auto_Manual_Value);
  updateOutput();
}

BLYNK_WRITE(V1)
{
  if (Auto_Manual_Value == 0) {
    MotorValue = param.asInt();
  }
  Serial.print("MotorValue = ");
  Serial.println(MotorValue);
  updateOutput();

}

BLYNK_WRITE(V2)
{
  BuzzerValue = param.asInt();
  Serial.print("BuzzerValue = ");
  Serial.println(BuzzerValue);
  updateOutput();
}




// Device Setup the configuration pin and take ready to perform task
void setup()
{
  Serial.begin(9600);

  //initWiFi();

  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());


  pinMode(Switch1, INPUT_PULLUP);
  pinMode(Switch2, INPUT_PULLUP);

  pinMode(WLevel_Input0, INPUT_PULLUP);
  pinMode(WLevel_Input1, INPUT_PULLUP);
  pinMode(WLevel_Input2, INPUT_PULLUP);
  pinMode(WLevel_Input3, INPUT_PULLUP);


  EEPROM.begin(512);
  Auto_Manual_Value = EEPROM.read(1);
  MotorValue = EEPROM.read(2);
  BuzzerValue = EEPROM.read(3);

  pinMode(MotorOutput, OUTPUT);
  pinMode(BuzzerOutput, OUTPUT);

  pinMode(wifiLed, OUTPUT);


  Update_Water_Level();

  updateOutput();



  //  Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid, pass);
  timer.setInterval(3000L, checkBlynkStatus); // check if Blynk server is connected every 3 seconds
  Blynk.config(auth);
  delay(100);


  updateOutput();


  BuzzerON(2000);
}

void loop()
{

  Update_Water_Level();

  //  runLoopWifi(); // For wifi continious check
  //  Blynk.run();
  //  swStatus();

  if (WiFi.status() != WL_CONNECTED) {
    //Serial.println("WiFi Not Connected");
  }
  else {
    //Serial.println("WiFi Connected");
    Blynk.run();
  }

  if (wifiFlag == 0) {
    with_internet();
  }
  else {
    without_internet();
  }

  timer.run(); // Initiates SimpleTimer
  updateOutput();
  Serial.println(BuzzerCounter);
}


// For Sync Virtual Pin with letest value in server
// need to on > Sync with latest server value every time device connects to the cloud < at DataStrem


BLYNK_CONNECTED()
{
  Serial.println("connected");
  update_blynk();
  Blynk.syncAll();
  //use delay for not blink all the output when
  //ESP8266 board is powerd on or connected to internet
  delay(2000);
  updateOutput();
}


void with_internet() {
  if (digitalRead(Switch1) == LOW) {
    Auto_Manual_Value = !Auto_Manual_Value;
    if (MotorValue != 0 && Auto_Manual_Value == 1) {
      MotorValue = 0;
    }
    updateOutput();
    update_blynk();
    delay(300);

  }
  else if (digitalRead(Switch2) == LOW) {

    if (Auto_Manual_Value == 0) {
      MotorValue = !MotorValue;
      updateOutput();
      update_blynk();
      delay(300);
    }

  }
  update_blynk();


}

void without_internet() {
  if (digitalRead(Switch1) == LOW) {
    Auto_Manual_Value = !Auto_Manual_Value;
    updateOutput();
    delay(300);

  }
  else if (digitalRead(Switch2) == LOW) {

    if (Auto_Manual_Value == 0) {
      MotorValue = !MotorValue;
      updateOutput();
      delay(300);
    }

  }

}

void Update_Water_Level() {
  if (digitalRead(WLevel_Input3) == LOW) {
    WaterLevelPercentage = 100;
    delay(300);


  }
  else if (digitalRead(WLevel_Input2) == LOW) {
    WaterLevelPercentage = 75;
    BuzzerCounter = 0;
  }
  else if (digitalRead(WLevel_Input1) == LOW) {
    WaterLevelPercentage = 50;
  }
  else if (digitalRead(WLevel_Input0) == LOW) {
    WaterLevelPercentage = 25;
    delay(300);

  }
  else
  {
    WaterLevelPercentage = 0;
    delay(300);
  }
}

void update_blynk() {
  Blynk.virtualWrite(V0, Auto_Manual_Value);
  Blynk.virtualWrite(V1, MotorValue);
  Blynk.virtualWrite(V2, BuzzerValue);
  Blynk.virtualWrite(V3, WaterLevelPercentage);
  Serial.print("water level : ");
  Serial.println(WaterLevelPercentage);
}


// Update Function Which the state and Value in Blyink Client
void updateOutput() {

  if(BuzzerValue != 0 && WaterLevelPercentage == 100 && BuzzerCounter <= 2){

    BuzzerON(5000);
    BuzzerCounter = BuzzerCounter + 1;
  }
  
  if (MotorValue == 0 && WaterLevelPercentage < 25 && Auto_Manual_Value != 0) {

    MotorStart();

  }
  else if (MotorValue == 0 && WaterLevelPercentage == 100 && Auto_Manual_Value != 0) {

    MotorStop();

  }
  if (MotorValue != 0 && Auto_Manual_Value == 0) {

    MotorStart();

  }
  else if (MotorValue == 0 && Auto_Manual_Value == 0) {

    MotorStop();

  }

  
  write_eeprom();
}

void BuzzerON(int DelayTime){
  Serial.println("Buzzer on");
  
   digitalWrite(BuzzerOutput, HIGH);
  delay(DelayTime);
  digitalWrite(BuzzerOutput, LOW);
}


void MotorStart() {

  Serial.println("Motor start Delay Time Start");
  delay(3000);
  digitalWrite(MotorOutput, HIGH);
  Serial.println("Motor Started");

}

void MotorStop() {
  Serial.println("Motor Stop Delay Time Start");
  delay(3000);
  digitalWrite(MotorOutput, LOW);
  Serial.println("Motor Stoped");
}




void write_eeprom() {
  EEPROM.write(1, Auto_Manual_Value);
  EEPROM.write(2, MotorValue);
  EEPROM.write(3, BuzzerValue);
  EEPROM.commit();
}



// Fot wifi re-connect initial setup

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.println();
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  // The ESP8266 tries to reconnect automatically when the connection is lost
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  // Once WiFi is reconnected then Connect to Blynk server
  Blynk.begin(auth, ssid, pass);
  updateOutput();
}


// For wifi continious check(at loop cheacking)

void runLoopWifi()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    switch (WiFi.status())
    {
      case WL_NO_SSID_AVAIL:
        Serial.println("Configured SSID cannot be reached");
        break;
      case WL_CONNECTED:
        Serial.println("Connection successfully established");
        // Once WiFi is reconnected then Connect to Blynk server
        Blynk.begin(auth, ssid, pass);
        updateOutput();
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Connection failed");
        break;
    }
    Serial.printf("Connection status: %d\n", WiFi.status());
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
    previousMillis = currentMillis;
  }
}
