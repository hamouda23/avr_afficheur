/*
 * LABO1_EX2.c
 *
 * Created 15/05/2021 09:33:36
 * Author : SAMIR HAMOUDA
 */ 

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#define DEBOUNCE_TIME 1000 /* microseconds */
// variable //
uint8_t LIMITE=45;
uint8_t j;
uint8_t i;
// uint8_t unite, dizaine;
uint8_t Afficheur[10] ={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x98} ;
uint8_t debounce(uint8_t PIN , uint8_t bit) {
	if (bit_is_clear(PIN, bit)) { /* button is pressed now */
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_clear(PIN, bit)) { /* still pressed */
			return (1);
		}
	}
	return (0);
}
// routine d'interruption de INT0
ISR(INT0_vect) { /* executer à chaque front descendant sur INT0 */
	i++; //incrementer i
	if (i==100) i=0; // vérifier si i=10
}
// routine d'interruption de INT1
ISR(INT1_vect) { /* executer à chaque front descendant sur INT0 */
	if (debounce(PIND, PD3)) {
		i=0;
		PORTC &= ~(1<<PC2); //Eteindre la LED PLEIN
		EIMSK |= (1<<INT0) ; //Activé INT0
	}
}
// fonction d'initilisation de l'interruption
void initInterrupt0(void) {
	EIMSK |= (1<<INT0) ; //Activé INT0
	EICRA |=(1 << ISC01); // trigger sur front déscendant
	EICRA &=~(1 << ISC00) ; //de INT0

	EIMSK |= (1<<INT1) ; //Activé INT1
	EICRA |=(1 << ISC11); // trigger sur front déscendant
	EICRA &=~(1 << ISC10) ; //de INT1
	sei(); // Activé les interruptions
}
// fonction d'affichae
void display (uint8_t AFF) {
	uint8_t AFF_unite, AFF_dizaine; //déclarer des variables locales UNITE ET	DIZAINE
	// Selectionner l'afficheur des unites
	AFF_unite = AFF % 10; //extraire les unité
	AFF_dizaine = AFF/10; //extraire les dizaines
	// Selectionner l'afficheur des unités
	PORTC &= ~(1<<PC1);
	PORTC |= (1<<PC0);
	PORTB = Afficheur [AFF_unite];
	_delay_ms(5);
	// Selectionner l'afficheur des dizaines
	PORTC &= ~(1<<PC0);
	PORTC |= (1<<PC1);
	PORTB = Afficheur [AFF_dizaine];
	_delay_ms(5);
}
//Initialisation des ports
void initPort(void){
	DDRB=0xFF;// PORTD programmé en sortie
	DDRC|=(1<<PC0); //PC0 en sortie
	DDRC|=(1<<PC1); //PC1 en sortie
	DDRC|=(1<<PC2); //PC1 en sortie
	PORTD |=(1<<PD2); // activé le PULL-UP sur PD2, pour avoir une activité
	PORTD |=(1<<PD3); // activé le PULL-UP sur PD3, pour avoir une activité
	PORTD |=(1<<PD4); // activé le PULL-UP sur PD4, pour avoir une activité
	PORTD |=(1<<PD5); // activé le PULL-UP sur PD5, pour avoir une activité

	i=0;
	PORTC &= ~(1<<PC0); // désactiver l'afficheur#1
	PORTC &= ~(1<<PC1);// désactiver l'afficheur#2
	PORTC &= ~(1<<PC2); // Etteindre la LED PLEIN	
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------programme principal----------------------------------
//-------------------------------------------------------------------------------------------------------

int main(void) {
	
	initPort();
	initInterrupt0();
	while (1) {
		if(bit_is_set(PIND,PD4) && bit_is_set(PIND,PD5) ) {
			display(i);
			if(i==LIMITE) {
				EIMSK &= ~(1<<INT0) ; //Désactivé INT0 ;
				PORTC |= (1<<PC2); //allumé la LED PLEIN
			}
		} else
		{
			// Vérifier que le bouton de la limite+ est appuyé
			if (debounce(PIND,PD4)){
				//incrémenter la limite
				LIMITE=LIMITE+1;
				_delay_ms(500);
				if(LIMITE==100) LIMITE=1;
			// Vérifier que le bouton de la limite- est appuyé	
			} else if (debounce(PIND,PD5)){
				LIMITE=LIMITE-1;
				_delay_ms(500);
				//if(LIMITE==1) LIMITE=1;
			}
			for(j=0;j<20;j++){
				//Afficher la limite
				display(LIMITE);
			}
		}
	}
	return (0);
}