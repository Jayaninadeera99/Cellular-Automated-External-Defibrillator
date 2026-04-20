/*
  CAED - Compact Automated External Defibrillator (Prototype)
  Arduino Version (Educational Use Only)

  ⚠️ WARNING:
  This code is for simulation and low-voltage testing ONLY.
  DO NOT use with real high-voltage medical applications.
*/

#define CHARGE_PIN      3
#define HBRIDGE_A       5
#define HBRIDGE_B       6
#define VOLTAGE_PIN     A0
#define SHOCK_BUTTON    7
#define BUZZER_PIN      9

float targetVoltage = 350.0;   // Target capacitor voltage
float voltage = 0;

// -------------------- SETUP --------------------
void setup() {
  pinMode(CHARGE_PIN, OUTPUT);
  pinMode(HBRIDGE_A, OUTPUT);
  pinMode(HBRIDGE_B, OUTPUT);
  pinMode(SHOCK_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("CAED System Initializing...");
}

// -------------------- READ VOLTAGE --------------------
float readVoltage() {
  int sensorValue = analogRead(VOLTAGE_PIN);

  // Convert ADC to voltage (adjust divider scaling as needed)
  float voltage = sensorValue * (5.0 / 1023.0) * 100;

  return voltage;
}

// -------------------- CHARGING --------------------
void chargeCapacitor() {
  Serial.println("Charging Started...");
  digitalWrite(CHARGE_PIN, HIGH);

  while (readVoltage() < targetVoltage) {
    voltage = readVoltage();
    Serial.print("Voltage: ");
    Serial.println(voltage);
    delay(100);
  }

  digitalWrite(CHARGE_PIN, LOW);
  Serial.println("Charging Complete!");
}

// -------------------- SAFETY CHECK --------------------
bool isSafe() {
  voltage = readVoltage();

  if (voltage > 300) {
    Serial.println("Safe to Shock");
    return true;
  } else {
    Serial.println("Voltage too low!");
    return false;
  }
}

// -------------------- SHOCK DELIVERY --------------------
void deliverShock() {
  Serial.println("WARNING: Shock Incoming!");

  // Beep before shock
  tone(BUZZER_PIN, 2000, 500);
  delay(1000);

  // Phase 1 (Forward)
  digitalWrite(HBRIDGE_A, HIGH);
  digitalWrite(HBRIDGE_B, LOW);
  delay(10);

  // Phase 2 (Reverse)
  digitalWrite(HBRIDGE_A, LOW);
  digitalWrite(HBRIDGE_B, HIGH);
  delay(10);

  // Stop output
  digitalWrite(HBRIDGE_A, LOW);
  digitalWrite(HBRIDGE_B, LOW);

  Serial.println("Shock Delivered!");

  // Discharge safety (simulate)
  dischargeCapacitor();
}

// -------------------- DISCHARGE --------------------
void dischargeCapacitor() {
  Serial.println("Discharging capacitor...");
  
  // Simulated discharge delay
  delay(2000);

  Serial.println("Discharge Complete.");
}

// -------------------- BLUETOOTH DATA --------------------
void sendData() {
  Serial.print("ECG:");
  Serial.print(voltage);   // Placeholder for ECG/voltage
  Serial.print(",STATUS:");

  if (voltage > 300) {
    Serial.println("PASS");
  } else {
    Serial.println("FAIL");
  }
}

// -------------------- MAIN LOOP --------------------
void loop() {

  // Step 1: Charge capacitor
  chargeCapacitor();

  // Step 2: Wait for shock button
  Serial.println("Press Shock Button...");
  
  while (digitalRead(SHOCK_BUTTON) == HIGH) {
    sendData();  // Send status via Bluetooth
    delay(500);
  }

  // Step 3: Safety check
  if (isSafe()) {
    deliverShock();
  } else {
    Serial.println("Shock Aborted!");
  }

  // Small delay before next cycle
  delay(3000);
}