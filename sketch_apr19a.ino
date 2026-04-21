const int stepPin = 9;
const int dirPin = 8;
const int enPin = 7;

const int stopButton = 2;    // Nút dừng
const int revButton = 3;     // Nút đảo chiều

bool isRunning = true;       // Trạng thái chạy/dừng
bool motorDir = HIGH;        // Trạng thái chiều quay

void setup() {
  Serial.begin(9600);
  
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);

  // Thiết lập nút nhấn với điện trở kéo lên tích hợp
  pinMode(stopButton, INPUT_PULLUP);
  pinMode(revButton, INPUT_PULLUP);

  digitalWrite(enPin, LOW); // Cho phép driver hoạt động
  Serial.println("He thong san sang. Bam nut de dieu khien!");
}

void loop() {
  // 1. Kiểm tra nút Dừng (nhấn để đổi trạng thái chạy/dừng)
  if (digitalRead(stopButton) == LOW) {
    delay(200); // Chống dội phím (debounce)
    isRunning = !isRunning;
    if (isRunning) {
      digitalWrite(enPin, LOW);
      Serial.println(">>> TIEP TUC CHAY");
    } else {
      digitalWrite(enPin, HIGH); // Ngắt dòng ra motor để dừng hoàn toàn
      Serial.println(">>> DA DUNG");
    }
  }

  // 2. Kiểm tra nút Đảo chiều
  if (digitalRead(revButton) == LOW) {
    delay(200); // Chống dội phím
    motorDir = !motorDir;
    digitalWrite(dirPin, motorDir);
    Serial.print(">>> DOI CHIEU QUAY: ");
    Serial.println(motorDir ? "THUAN" : "NGHICH");
  }

  // 3. Điều khiển xung nếu đang ở trạng thái chạy
  if (isRunning) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(800);
  }
}