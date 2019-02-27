#define LeftLine A0
#define RightLine A1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LeftLine, INPUT);
  pinMode(RightLine, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int valueLeft = analogRead(LeftLine);
  int valueRight = analogRead(RightLine);

  Serial.print(valueLeft);
  Serial.print(" ");
  Serial.println(valueRight);
}
