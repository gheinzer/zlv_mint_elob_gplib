/**
 * @file si_units.c
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Source code for @ref si_units.h.
 */

#include <elob/util/si_units.h>
#include <elob/util/error.h>

// SI converter functions
Quantity_t _SI_converter_none(Quantity_t, UnitConversionDirection_t);
Quantity_t _SI_converter_db(Quantity_t, UnitConversionDirection_t);
Quantity_t _SI_converter_gram(Quantity_t, UnitConversionDirection_t);

// Internal unit structs
const Unit_t _DIMENSIONLESS = { .name = "[Dimensionless]", .abbreviation = "", .converter = _SI_converter_none, .allowPrefixing = true };
const Unit_t _METER = { .name = "Meter", .abbreviation = "m", .converter = _SI_converter_none, .allowPrefixing = true };
const Unit_t _SECOND = { .name = "Second", .abbreviation = "s", .converter = _SI_converter_none, .allowPrefixing = true };
const Unit_t _GRAM = { .name = "Gram", .abbreviation = "g", .converter = _SI_converter_gram, .allowPrefixing = true };
const Unit_t _KILOGRAM = { .name = "Kilogram", .abbreviation = "kg", .converter = _SI_converter_none };
const Unit_t _VOLT = { .name = "Volt", .abbreviation = "V", .converter = _SI_converter_none, .allowPrefixing = true };
const Unit_t _AMPERE = { .name = "Ampere", .abbreviation = "A", .converter = _SI_converter_none, .allowPrefixing = true };
const Unit_t _OHM = { .name = "Ohm", .abbreviation = "Ohm", .converter = _SI_converter_none, .allowPrefixing = true };
const Unit_t _DECIBELS_P = { .name = "Decibels", .abbreviation = "dB", .converter = _SI_converter_db };
	
// Unit struct pointers
const Unit_t* const DIMENSIONLESS = &_DIMENSIONLESS;
const Unit_t* const METER = &_METER;
const Unit_t* const SECOND = &_SECOND;
const Unit_t* const GRAM = &_GRAM;
const Unit_t* const KILOGRAM = &_KILOGRAM;
const Unit_t* const VOLT = &_VOLT;
const Unit_t* const AMPERE = &_AMPERE;
const Unit_t* const OHM = &_OHM;
const Unit_t* const DECIBELS_P = &_DECIBELS_P;

Quantity_t _SI_converter_none(Quantity_t input, UnitConversionDirection_t direction) { return input; }
Quantity_t _SI_converter_db(Quantity_t input, UnitConversionDirection_t direction) {
    switch(direction) {
        case FROM_SI:
            assert(input._unit == DIMENSIONLESS, ERR_INVALID_UNIT);
            return SI(log10(input._number) * 10.0, DECIBELS_P, input._precision);
            break;

        case TO_SI:
            assert(input._unit == DECIBELS_P, ERR_INVALID_UNIT);
			return SI(pow(10, input._number / 10.0), DIMENSIONLESS, input._precision);
            break;
    }
}
Quantity_t _SI_converter_gram(Quantity_t input, UnitConversionDirection_t direction) {
	switch(direction) {
		case FROM_SI:
			return SI(input._number * 1000.0, GRAM, input._precision);
			break;

		case TO_SI:
			return SI(input._number / 1000.0, KILOGRAM, input._precision);
			break;
	}
}

/**
 * @brief Determines the number of digits before the decimal point.
 * 
 * @param n The number you want to analyze.
 * 
 * @returns The number of digits before the decimal point.
 */
uint8_t _digitsBeforeDecimalPoint(double n) {
	if(fabs(n) < 1) return 0;
	
	char valueString[32];
	dtostrf(fabs(n), 0, 0, valueString);
	uint8_t numberOfDigitsBeforeDecimalPoint = strlen(valueString);
	return numberOfDigitsBeforeDecimalPoint;
}

Quantity_t SI(double number, const Unit_t* unit, uint8_t precision) {
	Quantity_t output = { ._number = number, ._unit = unit, ._prefix = NONE, ._precision = precision };
	SI_applyPrefix(&output);
	return output;
}

void SI_removePrefix(Quantity_t* quantity) {
	if(!quantity->_prefix == NONE) {
		quantity->_number *= SI_getPrefixFactor(quantity->_prefix);
		quantity->_prefix = NONE;
	}
}

void SI_applyPrefix(Quantity_t* quantity) {
	SI_Prefix_t prefix = floor(log10(quantity->_number) / 3) * 3;
	
	try {
		SI_setPrefix(quantity, prefix);
	} catch {
		SI_setPrefix(quantity, prefix + 3);
	}
}

void SI_setPrefix(Quantity_t* quantity, SI_Prefix_t prefix) {
	quantity->_number *= SI_getPrefixFactor(quantity->_prefix);	
	quantity->_number /= SI_getPrefixFactor(prefix);
	quantity->_prefix = prefix;

    // If the precision is lower than the number of digits before the decimal point
    if(quantity->_precision < _digitsBeforeDecimalPoint(quantity->_number)) throw(ERR_PRECISION_VIOLATED);
}

Quantity_t SI_convert(Quantity_t quantity, const Unit_t* unit) {
	SI_removePrefix(&quantity);
	Quantity_t si = quantity._unit->converter(quantity, TO_SI);
	Quantity_t output = unit->converter(si, FROM_SI);
	SI_applyPrefix(&output);
	return output;
}

const char* SI_getPrefixString(Quantity_t quantity) {
	if(quantity._unit->allowPrefixing) {
		switch(quantity._prefix) {
			case EXA: return "E";
			case PETA: return "P";
			case TERA: return "T";
			case GIGA: return "G";
			case MEGA: return "M";
			case KILO: return "k";
			case NONE: return "";
			case MILLI: return "m";
			case MICRO: return "u";
			case NANO: return "n";
			case PICO: return "p";
			case FEMTO: return "f";
			case ATTO: return "a";
		}
	} else {
		switch(quantity._prefix) {
			case EXA: return "* 10^18 ";
			case PETA: return "* 10^15 ";
			case TERA: return "* 10^12 ";
			case GIGA: return "* 10^9 ";
			case MEGA: return "* 10^6 ";
			case KILO: return "* 10^3 ";
			case NONE: return "";
			case MILLI: return "* 10^-3 ";
			case MICRO: return "* 10^-6 ";
			case NANO: return "* 10^-9 ";
			case PICO: return "* 10^-12 ";
			case FEMTO: return "* 10^-15 ";
			case ATTO: return "* 10^-18 ";
		}
	}
}

void SI_toString(Quantity_t quantity, char* s, size_t maxLength) {
	const char* unit = quantity._unit->abbreviation;
	const char* prefix = SI_getPrefixString(quantity);
	
	double rounded = round(quantity._number * pow(10, quantity._precision)) / pow(10, quantity._precision);
	double test = log10(rounded);
	
	uint8_t digitsBeforeDecimalPoint = _digitsBeforeDecimalPoint(quantity._number);
	
	int precision = quantity._precision - digitsBeforeDecimalPoint;
	if(precision < 0) precision = 0;
	
	char valueString[32];
	dtostrf(quantity._number, 0, precision, valueString);
	snprintf(s, maxLength, "%s %s%s", valueString, prefix, unit);
}

void SI_print(Quantity_t quantity) {
	char valueString[32];
	SI_toString(quantity, valueString, sizeof(valueString) / sizeof(*valueString));
	printf(valueString);
}