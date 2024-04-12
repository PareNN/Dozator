void Encoder() {

if (eb.left()) {
  if (cfg.litr_m >=0.0) cfg.litr_m -= 0.1;
  EEPROM.put(0, cfg);
}

if (eb.right()) {
  if (cfg.litr_m <=60.0) cfg.litr_m += 0.1;
  EEPROM.put(0, cfg);
}

 if (eb.click()) cfg.status = !cfg.status;
EEPROM.put(0, cfg);
}
