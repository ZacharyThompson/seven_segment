// Use a shift register to display digits on a seven-segment display

#define SRCLK 2
#define RCLK 3
#define SER 4
#define SRCLR 5

// Shift register used: SN74HC595N
// SRCLK - Shift register clock; determines when SER will be read.
// RCLK - Storage ragister clock; determines when the outputs will be updated with the contents of the storage register.
// SER - Serial input to the shift register.
// SRCLR - Clears the shift register (active low).

// Add 1 to enable decimal point.
byte display_digits[] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100000, // 7
  0b11111110, // 8
  0b11100110, // 9
  0b11101110, // A
  0b00111110, // B
  0b10011100, // C
  0b01111010, // D
  0b10011110, // E
  0b10001110  // F
};

byte negative_sign = 0b00000010;

void update_display() {
  digitalWrite(RCLK, HIGH);
  digitalWrite(RCLK, LOW);
}

void display_error() {
  shift_clear();
  shift_in(0b100111100000101000001010, 24);
  update_display();
}

void display_hex(word n) {
  shift_clear();

  int digit_count = 0;
  while (n != 0) {
    shift_in(display_digits[n%16], 8);
    n /= 16;
    digit_count++;
  }
  for (int i = 0; i < 4-digit_count; i++) {
    shift_in(0b00000000, 8);
  }
  update_display();
}

void display_decimal(int n) {
  shift_clear();
  if (n > 9999 || n < -999) {
    display_error();
    return;
  }

  bool isNegative = false;
  if (n < 0) {
    isNegative = true;
    n = -n;
  }

  int digit_count = 0;
  while (n != 0) {
    shift_in(display_digits[n%10], 8);
    n /= 10;
    digit_count++;
  }

  if (isNegative) {
    shift_in(negative_sign, 8);
    digit_count++;
  }

  for (int i = 0; i < 4-digit_count; i++) {
    shift_in(0b00000000, 8);
  }

  update_display();
}

void shift_in(long data, size_t num_bits) {
  for (int j = 0; j < num_bits; j++) {
    if ((data >> j) & 1) {
      digitalWrite(SER, HIGH);

      digitalWrite(SRCLK, HIGH);
      digitalWrite(SRCLK, LOW);

      digitalWrite(SER, LOW);
    } else {
      digitalWrite(SRCLK, HIGH);
      digitalWrite(SRCLK, LOW);
    }
  }
}

void shift_clear() {
  // Clear shift register
  digitalWrite(SRCLR, HIGH);
  digitalWrite(SRCLR, LOW);
  digitalWrite(SRCLR, HIGH);

  update_display();
}

void setup() {
  // put your setup code here, to run once:
  pinMode(SRCLK, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SER, OUTPUT);
  pinMode(SRCLR, OUTPUT);

  digitalWrite(SRCLK, LOW);
  digitalWrite(RCLK, LOW);
  digitalWrite(SER, LOW);

  shift_clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0x0; i < 0x10000; i++) {
    display_hex(i);
    delay(15);
  }

  for (int i = -999; i < 10000; i++) {
    display_decimal(i);
    delay(15);
  }

}
