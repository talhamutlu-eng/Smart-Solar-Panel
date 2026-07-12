/*
 * ============================================================
 * SMART SOLAR SYSTEM - V4.2 (FINAL OPTIMIZED)
 * ============================================================
 */

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "Talha Mutlu";
const char* password = "12345678";

Adafruit_INA219 ina219;
LiquidCrystal_I2C lcd(0x27, 16, 2);
WebServer server(80);

const int kirmiziLed = 5;
const int yesilLed   = 4;

float sonVoltaj = 0, sonAkim = 0, sonGuc = 0;
float yumusatmaFaktoru = 0.05;
unsigned long sonOkumaZamani = 0;
const float panelMaxGuc = 1380.0; 

void handleRoot() {
  float verim = (sonGuc / panelMaxGuc) * 100.0;
  if (verim > 100) verim = 100;
  if (verim < 0) verim = 0;

  String verimRenk = (verim >= 75) ? "#00ff88" : (verim >= 35) ? "#facc15" : "#ff4d4d";
  // "Enerji üretimi yok" yazısı kaldırıldı, her zaman Aktif yazacak
  String durumYazi = "SISTEM AKTIF - VERI ALINIYOR";

  String html = "<!DOCTYPE html><html lang='tr'><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'><meta http-equiv='refresh' content='2'>";
  html += "<link href='https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700&family=Rajdhani:wght@500;700&display=swap' rel='stylesheet'>";
  html += "<title>Smart Solar System</title>";
  html += "<style>";
  html += "body { font-family: 'Rajdhani', sans-serif; background: #05070a; color: #fff; margin: 0; padding: 20px; display: flex; justify-content: center; min-height: 100vh; ";
  html += "background-image: linear-gradient(rgba(0, 255, 163, 0.05) 1.5px, transparent 1.5px), linear-gradient(90deg, rgba(0, 255, 163, 0.05) 1.5px, transparent 1.5px); ";
  html += "background-size: 40px 40px; }";
  html += ".wrapper { width: 100%; max-width: 420px; }";
  html += "h1 { font-family: 'Orbitron', sans-serif; text-align: center; color: #00ffa3; font-size: 1.6em; letter-spacing: 5px; margin-bottom: 30px; text-shadow: 0 0 20px rgba(0,255,163,0.6); }";
  html += ".main-card { background: rgba(13, 17, 23, 0.9); backdrop-filter: blur(10px); border-radius: 40px; padding: 40px 20px; border: 2px solid rgba(255,255,255,0.08); box-shadow: 0 30px 60px rgba(0,0,0,0.8); text-align: center; position: relative; }";
  html += ".verim-label { font-size: 0.85em; color: #8b949e; letter-spacing: 3px; margin-bottom: 25px; font-weight: 700; }";
  html += ".circle-box { position: relative; width: 180px; height: 180px; margin: 0 auto; filter: drop-shadow(0 0 15px " + verimRenk + "66); }";
  html += ".svg-circle { transform: rotate(-90deg); width: 180px; height: 180px; }";
  html += ".bg-circle { fill: none; stroke: rgba(255,255,255,0.05); stroke-width: 12; }";
  html += ".fg-circle { fill: none; stroke: url(#grad); stroke-width: 14; stroke-linecap: round; stroke-dasharray: 502; stroke-dashoffset: " + String(502 - (verim * 5.02)) + "; transition: all 1s ease-in-out; }";
  html += ".verim-text { position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); font-size: 3em; font-weight: 700; color: #fff; }";
  html += ".grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 15px; margin-top: 35px; }";
  html += ".stat { background: rgba(255,255,255,0.04); border-radius: 20px; padding: 18px 5px; border: 1.5px solid rgba(255,255,255,0.1); }";
  html += ".label { font-size: 0.7em; color: #64748b; margin-bottom: 10px; font-weight: 700; }";
  html += ".value { font-size: 1.25em; font-weight: 700; font-family: 'Orbitron', sans-serif; }";
  html += ".status-panel { margin-top: 30px; padding: 18px; border-radius: 100px; background: rgba(0,0,0,0.5); border: 2px solid rgba(0,255,163,0.2); display: flex; align-items: center; justify-content: center; gap: 12px; }";
  html += ".dot { width: 12px; height: 12px; border-radius: 50%; background: " + verimRenk + "; box-shadow: 0 0 20px " + verimRenk + "; animation: blink 1.5s infinite; }";
  html += "@keyframes blink { 0%, 100% { transform: scale(1); opacity: 1; } 50% { transform: scale(1.2); opacity: 0.5; } }";
  html += ".st-text { font-size: 0.85em; color: #ffffff; letter-spacing: 1.5px; font-weight: 700; }";
  html += "</style></head><body><div class='wrapper'>";
  html += "<h1>SMART SOLAR</h1>";
  html += "<div class='main-card'>";
  html += "<svg width='0' height='0'><defs><linearGradient id='grad' x1='0%' y1='0%' x2='100%' y2='0%'><stop offset='0%' style='stop-color:#00ffa3;stop-opacity:1' /><stop offset='100%' style='stop-color:" + verimRenk + ";stop-opacity:1' /></linearGradient></defs></svg>";
  html += "<div class='verim-label'>PERFORMANS INDEX</div>";
  html += "<div class='circle-box'><svg class='svg-circle'><circle class='bg-circle' cx='90' cy='90' r='80'/><circle class='fg-circle' cx='90' cy='90' r='80'/></svg>";
  html += "<div class='verim-text'>" + String(verim, 0) + "<span style='font-size:0.4em;'>%</span></div></div></div>";
  html += "<div class='grid'>";
  html += "<div class='stat'><div class='label'>VOLTAJ</div><div class='value' style='color:#00d2ff;'>" + String(sonVoltaj, 2) + "</div></div>";
  html += "<div class='stat'><div class='label'>AKIM</div><div class='value' style='color:#9d50bb;'>" + String(sonAkim, 1) + "</div></div>";
  html += "<div class='stat'><div class='label'>GÜÇ</div><div class='value' style='color:#facc15;'>" + String(sonGuc, 1) + "</div></div>";
  html += "</div><div class='status-panel'><div class='dot'></div><div class='st-text'>" + durumYazi + "</div></div>";
  html += "</div></body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(kirmiziLed, OUTPUT); pinMode(yesilLed, OUTPUT);
  digitalWrite(kirmiziLed, HIGH); digitalWrite(yesilLed, LOW);

  lcd.init(); lcd.backlight();
  lcd.setCursor(0,0); lcd.print("SMART SOLAR");
  lcd.setCursor(0,1); lcd.print("BEKLEMEDE...");
  delay(15000); 

  if (!ina219.begin()) { while (1); }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  server.on("/", handleRoot);
  server.begin();

  digitalWrite(kirmiziLed, LOW); digitalWrite(yesilLed, HIGH);
  lcd.clear(); 
}

void loop() {
  server.handleClient();
  if (millis() - sonOkumaZamani >= 400) {
    sonOkumaZamani = millis();
    float anlikVoltaj = ina219.getBusVoltage_V();
    float anlikAkim   = abs(ina219.getCurrent_mA());
    sonVoltaj = (sonVoltaj * (1.0 - yumusatmaFaktoru)) + (anlikVoltaj * yumusatmaFaktoru);
    sonAkim   = (sonAkim * (1.0 - yumusatmaFaktoru)) + (anlikAkim * yumusatmaFaktoru);
    sonGuc    = sonVoltaj * sonAkim;

    if (sonVoltaj < 0.01) {
      digitalWrite(yesilLed, LOW); digitalWrite(kirmiziLed, HIGH);
    } else {
      digitalWrite(kirmiziLed, LOW); digitalWrite(yesilLed, HIGH);
    }

    // LCD EKRAN GÜNCELLEME (En Garanti Yöntem)
    lcd.setCursor(0, 0);
    lcd.print("V:"); lcd.print(sonVoltaj, 2);
    lcd.print("V I:"); lcd.print(sonAkim, 1);
    lcd.print("mA ");
    
    lcd.setCursor(0, 1);
    lcd.print("Guc: "); lcd.print(sonGuc, 1);
    lcd.print(" mW      ");
  }
}