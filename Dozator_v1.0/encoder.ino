void Encoder() {

if (eb.left()) {
  if (litr_m >=0) --litr_m;
  EEPROM.write(1, litr_m);
}

if (eb.right()) {
  if (litr_m <=60) ++litr_m;
  EEPROM.write(1, litr_m);
}

 if (eb.click()) status = !status;
EEPROM.write(0, status);
}