int waterPerPlant(){
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER WATER ",11);
	lcd_line_two();
	lcd_string("AMOUNT(ml):",11);
	key = scankey();
	int wtrPerPlant = displayKey();
	return wtrPerPlant;
}

int fertilizePerPlant(){
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER FERTILIZER",16);
	lcd_line_two();
	lcd_string("AMOUNT(ml):",11);
	key = scankey();
	int fertilizePerPlant = displayKey();
	return fertilizePerPlant;
}

int humidityMin(){
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER HUMIDITY",16);
	lcd_line_two();
	lcd_string("MIN(%RH):",9);
	key = scankey();
	int humidityMin = displayKey();
	return humidityMin;
}

int humidityMax(){
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER HUMIDITY",16);
	lcd_line_two();
	lcd_string("MAX(%RH):",9);
	key = scankey();
	int humidityMax = displayKey();
	return humidityMax;
}

int tempMin(){
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER TEMPARAT.",15);
	lcd_line_two();
	lcd_string("MIN(C):",7);
	key = scankey();
	int tempMin = displayKey();
	return tempMin;
}

int tempMax(){
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER TEMPARAT.",15);
	lcd_line_two();
	lcd_string("MAX(C):",7);
	key = scankey();
	int tempMax = displayKey();
	return tempMax;
}

int getLDR(){
	lcd_clear();
	lcd_line_one();
	lcd_string("ENTER LDR ADC",15);
	lcd_line_two();
	lcd_string("MIN:",5);
	key = scankey();
	int ldrADC = displayKey();
	return ldrADC;
}