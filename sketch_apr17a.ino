void setup() {
  Serial.begin(115200);
}

void loop() {
  int sensorValue = analogRead(1); // Đọc chân GPIO 1
  Serial.println(sensorValue);    // Gửi lên máy tính
  delay(10);
}