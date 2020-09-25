#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SR04.h>
SR04 sr04 = SR04(5,6);
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,177);
EthernetServer server(80);
EthernetClient client;
String readString="";
unsigned long nowtime=0;
unsigned long starttime=0;
unsigned long time=0;
int minute = 0;
int second = 0;
float balancepin;
float levelhigh;
float levelhalf;
float levellow;
float ItoVPin ;
float cm;

float ItoV = 0;
float t = 0;
float p = 0;
String b;
String level;
String balance;
String turnoff;

void setup() {
Serial.begin(9600);
Ethernet.begin(mac, ip);
server.begin();
sensors.begin();
pinMode(6, OUTPUT); // TrigPin
pinMode(5, INPUT); // EchoPin
pinMode(7, OUTPUT); //pin 7 is switch
pinMode(A0, INPUT); //POWER A1 has problem
pinMode(A1, INPUT); //mechanical switch for the position of pot
pinMode(A2, INPUT); //balance
pinMode(A5, INPUT); //high position
pinMode(A4, INPUT); //half position
pinMode(A3, INPUT); //lowposition
}

void loop() {

float a[20]={0};

float maxv = 2.150;
float minv = 2.150;
float Vin1 = 0;
float Vin2 = 0;

//Temperature
sensors.requestTemperatures();
t = sensors.getTempCByIndex(0);

//Power
for(int i=0;i<20;i++){
ItoV = analogRead(ItoVPin)*4.740/1023.000;
a[i]=ItoV;
delay(1);
}
for(int i=0;i<20;i++){
if (a[i]>maxv){
maxv = a[i];
}
if (a[i]<minv){
minv = a[i];
}
}
Vin1 = (maxv-2.15)/0.0982;
Vin2 = (minv-2.15)/0.0982;
p = 220*(-Vin2+Vin1)/(2*sqrt(2));

//Water level
cm=sr04.Distance();
if (cm >= 12.00){
b="Please add water";
}
if (2.00 < cm & cm < 12.00){
b="Has water";
}

//Coffee position
levelhigh = analogRead(A5);
levelhalf = analogRead(A4);
levellow = analogRead(A3);
if (analogRead(A1)==0){
if (levellow > 654 && t <=27){
if (levelhalf > 655){
if (levelhigh >= 650){
level = "The coffee pot is full";
}
else{
level = "At least half coffee in the pot";
}
}
else{
level = "There is little coffee in the pot";
}
}

else{
level = "No coffee in the pot";
}

if ((levellow >= 656) && t > 28.00 && t <= 38.00){
if (levelhalf > 658){
if (levelhigh > 650){
level = "The coffee pot is full";
}
else{
level = "At least half coffee in the pot";
}
}
else{
level = "There is little coffee in the pot";
}
}

if (levellow > 659 && t > 38 && t <= 45){
if (levelhalf > 660){
if (levelhigh > 655){
level = "The coffee pot is full";
}
else{
level = "At least half coffee in the pot";
}
}

else{
level = "There is little coffee in the pot";
}
}

if (levellow >= 662 && t > 45 && t <= 55){
if (levelhalf > 662){
if (levelhigh > 659){
level = "The coffee pot is full";
}
else{
level = "At least half coffee in the pot";
}
}
else{
level = "There is little coffee in the pot";
}
}

if (levellow >= 665 && t > 55 && t <= 65){
if (levelhalf > 665){
if (levelhigh > 662){
level = "The coffee pot is full";
}
else{
level = "At least half coffee in the pot";
}

}
else{
level = "There is little coffee in the pot";
}
}

if (levellow >= 670 && t > 65){
if (levelhalf > 670){
if (levelhigh > 660){
level = "The coffee pot is full";
}
else{
level = "At least half coffee in the pot";
}
}
else{
level = "There is little coffee in the pot";
}
}

}
else
{
level="Coffee pot is not in the maker";
digitalWrite(7, LOW);
time = 0;
t = 0.00;

}

// Maker position(balance)
balancepin = analogRead(A2);
if (time >= 600){
turnoff = "<div>out of time, turn off automatically</div>";
}
else{
turnoff = "";
}
if(balancepin!=0){
balance= "<h2 style=\"color:red\">Maker lays down</h2>";
time = 0;
}
else{
balance ="";
}
if (time >= 600 || balancepin!=0){
digitalWrite(7, LOW);
}

if (digitalRead(7)==HIGH){
nowtime = millis();
time = nowtime - starttime;
time = time/1000;
minute = time/60;

second = time%60;
}
else{
p = 00.00;
}
delay(5);

// WIFI
client = server.available();
if (client) {

boolean currentLineIsBlank = false;
while (client.connected()) {
if (client.available()) {
char c = client.read();
readString += c;
if (c == '\n') {

if(readString.indexOf("?on") >0) {
digitalWrite(7, HIGH);
starttime = millis();
break;
}

if(readString.indexOf("?off") >0) {
digitalWrite(7, LOW);

time = 0;
break;
}

if(readString.indexOf("?getTemperature") >0) {
client.println(F("<i>Tempreture: </i><b> "));
client.println(t);
client.println(F(" degree</b><i>&nbsp;||&nbsp;Power: </i><b>"));
client.println(p);
client.println(F(" </b>"));
break;
}

if(readString.indexOf("?getCoffeelevel") >0) {
client.println(F("<h3>"));
client.println(level);
client.println(F("</h3><h3>"));
client.println(b);
client.println(F("</h3>"));
client.println(balance);
break;
}

if(readString.indexOf("?getTime") >0) {

client.println(F("working time: "));
if (time ==0){
client.println(F("NOT WORKING"));
}
else{
client.println(minute);
client.println(F(" : "));
client.println(second);
}
client.println(turnoff);
break;
}

delay(2);
SendHTML();
break;
}
}
}
delay(10);
client.stop();
readString="";
}
}

void SendHTML()
{
client.println(F("HTTP/1.1 200 OK"));
client.println(F("Content-Type: text/html"));
client.println(F("Connection: close"));
client.println();
client.println(F("<!DOCTYPE HTML>"));
client.println(F("<html><head><meta charset=\"UTF-8\"><style>div.container {width:
100%;}header{padding: 1em;color: white;background-color: black;clear: left;text-align:
center;}#wrapper { text-align: center;}"));
client.println(F(".column {width: 350px; margin: 10px;display: inline-block;text-align: left;}.container
{margin:auto;text-align: center;}"));
client.println(F(".button {position: relative;background-color: #000;border: none;font-size:

20px;color: #FFFFFF;padding: 20px;width: 120px;text-align: center;transition-duration: 0.4s;text-
decoration: none;cursor: pointer;margin: auto; }</style>"));

client.println(F("<script type=\"text/javascript\">function send2arduino(){var xmlhttp;xmlhttp=new
XMLHttpRequest();element=document.getElementById(\"Maker\");if
(element.innerHTML.match(\"Turn on\")){element.innerHTML=\"Turn off\";
xmlhttp.open(\"GET\",\"?on\",true);}else{ element.innerHTML=\"Turn
on\";xmlhttp.open(\"GET\",\"?off\",true); }xmlhttp.send();}"));

client.println(F("function getTime(){var xmlhttp;xmlhttp=new XMLHttpRequest();"));
client.println(F("xmlhttp.onreadystatechange=function(){if (xmlhttp.readyState==4 &&
xmlhttp.status==200)"));

client.println(F("document.getElementById(\"Time\").innerHTML=xmlhttp.responseText;};xmlhttp.op
en(\"GET\",\"?getTime\",true); xmlhttp.send();}window.setInterval(getTime,6000);"));

client.println(F("function getCoffeelevel(){var xmlhttp;xmlhttp=new XMLHttpRequest();"));
client.println(F("xmlhttp.onreadystatechange=function(){if (xmlhttp.readyState==4 &&
xmlhttp.status==200)"));

client.println(F("document.getElementById(\"Coffeelevel\").innerHTML=xmlhttp.responseText;};xml

http.open(\"GET\",\"?getCoffeelevel\",true);
xmlhttp.send();}window.setInterval(getCoffeelevel,6000);"));

client.println(F("function getTemperature(){var xmlhttp;xmlhttp=new XMLHttpRequest();"));
client.println(F("xmlhttp.onreadystatechange=function(){if (xmlhttp.readyState==4 &&
xmlhttp.status==200)"));

client.println(F("document.getElementById(\"Temperature\").innerHTML=xmlhttp.responseText;};x
mlhttp.open(\"GET\",\"?getTemperature\",true);
xmlhttp.send();}window.setInterval(getTemperature,6000);</script>"));

client.println(F("</head><body><div align=\"center\"><header><h1>Coffee
Maker</h1></header><br/><br/><div class= ́\"container\"><div class=\"column\">"));
client.println(F("<div id=\"Temperature\"></div>"));
client.println(F("<br/><h2>"));
client.println(F("<div id=\"Time\"></div></h2>"));
client.println(F("<div id=\"Coffeelevel\"></div><br/><br/><div class = \"wrapper\">"));
client.println(F("<button align=\"right\" class=\"button\" id=\"Maker\" type=\"button\"
onclick=\"send2arduino()\">Turn on</button>"));
client.println(F("</div></div></body></html>"));
}
