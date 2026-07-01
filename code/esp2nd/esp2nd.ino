#include <WiFi.h>
#include <ESP32Servo.h>

const char* ssid = "narzo 50A";
const char* password = "12345678";

WiFiServer server(5000);

Servo panServo;
Servo tiltServo;

// 🔥 Fine-tuned angles
#define CENTER 90

#define LEFT_ANGLE 80
#define RIGHT_ANGLE 100

#define UP_ANGLE 100
#define DOWN_ANGLE 80

#define HOLD_TIME 2000   // 🔥 stay time (in ms) → 2000 = 2 seconds

void setup() {

  Serial.begin(115200);

  panServo.attach(13);
  tiltServo.attach(12);

  panServo.write(CENTER);
  tiltServo.write(CENTER);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("ESP IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {

  WiFiClient client = server.available();

  if (client) {

    Serial.println("Laptop connected");

    while (client.connected()) {

      if (client.available()) {

        char cmd = client.read();
        Serial.println(cmd);

        // ===== PAN =====
        if (cmd == 'L') {
          panServo.write(LEFT_ANGLE);
          delay(HOLD_TIME);        // 🔥 stay here
          panServo.write(CENTER);
        }

        if (cmd == 'R') {
          panServo.write(RIGHT_ANGLE);
          delay(HOLD_TIME);
          panServo.write(CENTER);
        }

        // ===== CENTER =====
        if (cmd == 'C') {
          panServo.write(CENTER);
          tiltServo.write(CENTER);
        }

        // ===== TILT =====
        if (cmd == 'U') {
          tiltServo.write(UP_ANGLE);
          delay(HOLD_TIME);
          tiltServo.write(CENTER);
        }

        if (cmd == 'D') {
          tiltServo.write(DOWN_ANGLE);
          delay(HOLD_TIME);
          tiltServo.write(CENTER);
        }
      }
    }

    client.stop();
    Serial.println("Client disconnected");
  }
}
