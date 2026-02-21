#define F_CPU 8000000UL            /* Defining CPU Frequency e.g. here 8MHz */

#include <avr/io.h>                /* Including AVR std. library file */
#include <util/delay.h>            /* Including inbuilt defined Delay header file */
#include <stdio.h>
#include <string.h>

#define LCD_Data_Dir DDRD          /* Defining LCD data port direction */
#define LCD_Data_Port PORTD        /* Defining LCD data port */
#define LCD_Command_Dir DDRB       /* Defining LCD command port direction register */
#define LCD_Command_Port PORTB     /* Defining LCD command port */
#define RS PB0                     /* Defining Register Select (data/command reg.) pin */
#define RW PB1                     /* Defining Read/Write signal pin */
#define EN PB2                     /* Defining Enable signal pin */

#define KEY_PRT PORTA              // keyboard PORT
#define KEY_DDR DDRA               // keyboard DDR
#define KEY_PIN PINA               // keyboard PIN

unsigned char keypad[4][4] = {
	{'7', '8', '9', '/'},
	{'4', '5', '6', '*'},
	{'1', '2', '3', '-'},
	{'C', '0', '=', '+'}
};

unsigned char colloc, rowloc;
unsigned int votes[4] = {0, 0, 0, 0}; // Votes for 4 candidates
unsigned int voter_count = 0;

void LCD_Command(unsigned char cmnd) {
	LCD_Data_Port = cmnd;
	LCD_Command_Port &= ~(1 << RS);  /* RS=0 command reg. */
	LCD_Command_Port &= ~(1 << RW);  /* RW=0 Write operation */
	LCD_Command_Port |= (1 << EN);   /* Enable pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(3);
}

void LCD_Char(unsigned char char_data) {  /* LCD data write function */
	LCD_Data_Port = char_data;
	LCD_Command_Port |= (1 << RS);  /* RS=1 Data reg. */
	LCD_Command_Port &= ~(1 << RW); /* RW=0 write operation */
	LCD_Command_Port |= (1 << EN);  /* Enable Pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(1);
}

void LCD_Init(void) {                /* LCD Initialize function */
	LCD_Command_Dir = 0xFF;          /* Make LCD command port direction as o/p */
	LCD_Data_Dir = 0xFF;             /* Make LCD data port direction as o/p */
	_delay_ms(20);                   /* LCD Power ON delay always >15ms */

	LCD_Command(0x38);               /* Initialization of 16X2 LCD in 8bit mode */
	LCD_Command(0x0C);               /* Display ON Cursor OFF */
	LCD_Command(0x01);               /* Clear display */
	LCD_Command(0x80);               /* Cursor at home position */
}

void LCD_String(char *str) {         /* Send string to LCD function */
	int i = 0;
	while (str[i] != 0) {            /* Send each char of string till the NULL */
		LCD_Char(str[i]);
		i++;
	}
}

void LCD_Clear() {
	LCD_Command(0x01);               /* Clear display */
	LCD_Command(0x80);               /* Cursor at home position */
}

char keyfind() {
	while (1) {
		KEY_DDR = 0xF0;              /* set port direction as input-output */
		KEY_PRT = 0xFF;

		/* Check for pressed key in each row */
		for (int row = 0; row < 4; row++) {
			KEY_PRT = ~(0x10 << row); /* Set row to low */
			_delay_us(1);             /* Add small delay for debounce */
			colloc = (KEY_PIN & 0x0F); /* Read columns */
			if (colloc != 0x0F) {
				rowloc = row;
				goto find_col;        /* Exit from loop */
			}
		}
	}

	find_col:
	if (colloc == 0x0E) return keypad[rowloc][0];
	if (colloc == 0x0D) return keypad[rowloc][1];
	if (colloc == 0x0B) return keypad[rowloc][2];
	return keypad[rowloc][3];
}

int check_pin() {
	char entered_pin[5];
	char correct_pin[5] = "4016";  // Set your 4-digit PIN here
	char master_key[5] = "1234";   // Set your master key here
	int i = 0;

	LCD_Clear();
	LCD_String("Enter PIN:");
	LCD_Command(0xC0);

	while (i < 4) {
		entered_pin[i] = keyfind();
		if (entered_pin[i] < '0' || entered_pin[i] > '9') {
			LCD_Clear();
			LCD_String("Invalid Char");
			_delay_ms(500);
			return -1;  // Return immediately if an invalid character is entered
		}
		LCD_Char('*'); // Display * for each digit entered
		i++;
		_delay_ms(100); // Delay to prevent multiple * being displayed
	}
	entered_pin[i] = '\0'; // Null-terminate the entered PIN

	_delay_ms(100); // Small delay before checking PIN
	if (strcmp(entered_pin, correct_pin) == 0) {
		return 1;  // Correct voter PIN
		} else if (strcmp(entered_pin, master_key) == 0) {
		return 2;  // Correct master key
		} else {
		return 0;  // Incorrect PIN
	}
}

void vote_counting() {
	char key;
	char vote;
	LCD_Clear();
	LCD_String("1:BJP 2:INC");
	LCD_Command(0xC0);
	LCD_String("3:AAP 4:JDS");

	while (1) {
		key = keyfind();
		if (key >= '1' && key <= '4') {  /* Valid candidate */
			vote = key;                  /* Store the selected candidate */
			LCD_Clear();
			LCD_String("Confirm: =");
			_delay_ms(100);              /* Short delay for debounce */
			while (keyfind() != '=');    /* Wait for confirmation */
			votes[vote - '1']++;         /* Increment vote count */
			voter_count++;
			LCD_Clear();
			LCD_String("Vote Recorded");
			_delay_ms(200);
			break;
		}
	}
}

void display_votes() {
	char buffer1[16];
	char buffer2[16];
	LCD_Clear();
	snprintf(buffer1, sizeof(buffer1), "BJP:%d INC:%d", votes[0], votes[1]);
	snprintf(buffer2, sizeof(buffer2), "AAP:%d JDS:%d", votes[2], votes[3]);
	LCD_Command(0x80); // Move cursor to the first line
	LCD_String(buffer1);
	LCD_Command(0xC0); // Move cursor to the second line
	LCD_String(buffer2);
	_delay_ms(300);
}

void display_winner() {
	int max_votes = 0;
	int winner = -1;
	int max_vote_count = 0;
	int equal_votes = 0;

	for (int i = 0; i < 4; i++) {
		if (votes[i] > max_votes) {
			max_votes = votes[i];
			winner = i;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (votes[i] == max_votes) {
			max_vote_count++;
		}
	}

	if (max_vote_count == 4) {
		equal_votes = 1;
	}

	LCD_Clear();
	if (equal_votes) {
		LCD_String("Equal Votes");
		} else {
		if (winner != -1) {
			LCD_String("Winner:");
			LCD_Command(0xC0);
			switch (winner) {
				case 0: LCD_String("BJP"); break;
				case 1: LCD_String("INC"); break;
				case 2: LCD_String("AAP"); break;
				case 3: LCD_String("JDS"); break;
			}
			} else {
			LCD_String("No Votes");
		}
	}
	_delay_ms(300);
}


void master_function() {
	char key;
	LCD_Clear();
	LCD_String("1:Show Votes");
	LCD_Command(0xC0);
	LCD_String("2:Show Winner");

	while (1) {
		key = keyfind();
		if (key == '1') {
			display_votes();
			_delay_ms(400);
			display_winner();
			break;
			} else if (key == '2') {
			display_winner();
			_delay_ms(300);
			break;
		}
	}
}

int main(void) {
	LCD_Init();
	LCD_String("    TEAM_11");
	_delay_ms(300);
	LCD_Clear();
	LCD_String("Every vote counts");
	_delay_ms(300);
	LCD_Clear();
	LCD_String("   Vote wise");
	_delay_ms(200);
	LCD_Clear();
	LCD_String(" Authentication:");
	_delay_ms(200);

	// Define the initial security key
	char initial_key[5] = "7676";  // Set your initial security key here
	char entered_key[5];

	while (1) { // Infinite loop to handle continuous voting
		// Check initial security key
		LCD_Clear();
		LCD_String("Enter Key:");
		LCD_Command(0xC0);

		// Loop to read the entered key
		int i = 0;
		while (i < 4) {
			entered_key[i] = keyfind();
			LCD_Char('*'); // Display * for each digit entered
			i++;
			_delay_ms(100); // Delay to prevent multiple * being displayed
		}
		entered_key[i] = '\0'; // Null-terminate the entered key

		_delay_ms(100); // Small delay before checking the key
		if (strcmp(entered_key, initial_key) != 0) {
			LCD_Clear();
			LCD_String("Wrong Key");
			_delay_ms(500);
			continue; // Restart the loop if the key is incorrect
		}

		// Key is correct, proceed to PIN entry and voting
		int pin_check = check_pin();
		if (pin_check == 1) {
			vote_counting();
			LCD_Clear();
			LCD_String("Next Voter");
			_delay_ms(500);
			} else if (pin_check == 2) {
			master_function();
			break;  // Exit the loop after displaying master functions
			} else if (pin_check == 0) {
			LCD_Clear();
			LCD_String("Invalid PIN");
			_delay_ms(500);
		}
	}

	while (1); // Infinite loop to stop execution
	return 0;
}


