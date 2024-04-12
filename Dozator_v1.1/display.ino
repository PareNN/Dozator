void Display() {
  lcd.setCursor(0, 0);
  lcd.print("L/Min-Ust   ");
  if (litr_m < 10.0) lcd.print(" ");
  lcd.print(litr_m, 1);

  lcd.setCursor(0, 1);
  lcd.print("L/Min-Fact  ");
  if (l_minute < 10.0) lcd.print(" ");
  lcd.print(l_minute, 1);
}