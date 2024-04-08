void Display() {
  lcd.setCursor(0, 0);
  lcd.print("L/Min-Ust     ");
  if (litr_m < 10) lcd.print("0");
  lcd.print(litr_m, DEC);

  lcd.setCursor(0, 1);
  lcd.print("L/Min-Fact    ");
  if (l_minute < 10) lcd.print("0");
  lcd.print(l_minute, DEC);
}