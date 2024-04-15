void Encoder() {

if (eb.left()) {
  if (litr_m >=0.0) litr_m -= 0.2;
  EEPROM.put(1, litr_m);
}

if (eb.right()) {
  if (litr_m <=6.0) litr_m += 0.2;
  EEPROM.put(1, litr_m);
}

 if (eb.click()) status = !status;
EEPROM.put(0, status);
}
