/**
 * @file si_units.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Library for dealing with SI units and prefixes.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef SI_UNITS_H_
#define SI_UNITS_H_

/**
 * @brief Enumerator defining the unit conversion directions.
 * 
 */
typedef enum {
    /**
     * @brief The given number is in SI units and should be converted to the destination unit.
     * 
     */
	FROM_SI,
    /**
     * @brief The given number is in the unit and should be converted to SI units.
     * 
     */
	TO_SI
} UnitConversionDirection_t;

/**
 * @brief Defines a type representing a unit.
 */
typedef struct Unit Unit_t;
/**
 * @brief Defines a type representing a quantity. 
 */
typedef struct Quantity Quantity_t;

typedef Quantity_t (*UnitConverter_t)(Quantity_t, UnitConversionDirection_t);

/**
 * @brief Enumerator defining the SI prefixes. The enumerator values
 * correspond to the decimal exponent.
 * 
 */
typedef enum {
    /** @brief Exa prefix, \f$10^{18}\f$ */
    EXA = 18,
    /** @brief Peta prefix, \f$10^{15}\f$ */
    PETA = 15,
    /** @brief Tera prefix, \f$10^{12}\f$ */
    TERA = 12,
    /** @brief Giga prefix, \f$10^{9}\f$ */
    GIGA = 9,
    /** @brief Mega prefix, \f$10^{6}\f$ */
    MEGA = 6,
    /** @brief Kilo prefix, \f$10^{3}\f$ */
    KILO = 3,
    /** @brief No prefix, \f$10^{0}\f$ */
    NONE = 0,
    /** @brief Milli prefix, \f$10^{-3}\f$ */
    MILLI = -3,
    /** @brief Micro prefix, \f$10^{-6}\f$ */
    MICRO = -6,
    /** @brief Nano prefix, \f$10^{-9}\f$ */
    NANO = -9,
    /** @brief Pico prefix, \f$10^{-12}\f$ */
    PICO = -12,
    /** @brief Femto prefix, \f$10^{-15}\f$ */
    FEMTO = -15,
    /** @brief Atto prefix, \f$10^{-18}\f$ */
    ATTO = -18
} SI_Prefix_t;

/**
 * @brief Defines a struct representing a unit.
 * 
 */
struct Unit {
    /**
     * @brief Full name of the unit, e.g. _Volt_.
     * 
     */
    const char* const name;
    /**
     * @brief Unit abbreviation, e.g. _V_.
     * 
     */
    const char* const abbreviation;
    /**
     * @brief Converter function which converts the specified quantity in the specified direction.
     * 
     * The first parameter given will be a @ref Quantity_t which indicates the initial quantity. The
     * second parameter is an enumerator of type @ref UnitConversionDirection_t which indicates the
     * desired conversino direction.
     */
    UnitConverter_t converter;
	/**
	 * @brief Specifies if the unit allows SI prefixing.
	 * 
	 */
	bool allowPrefixing;
};

/**
 * @brief Defines a struct representing a quantity. 
 */
struct Quantity {
    /**
     * @brief The numeric part of the quantity. 
     */
	double _number;
    /**
     * @brief The unit the quantity is stored in. 
     */
	const Unit_t* const _unit;
    /**
     * @brief The unit's SI prefix.
     * 
     */
	SI_Prefix_t _prefix;
	/**
	 * @brief The number of significant digits (excluding leading zeros, including trailing zeros)
     * the quantity includes. 
	 */
	uint8_t _precision;
};

/**
 * @brief Gets the prefix factor from a SI prefix.
 * 
 * @param prefix A SI prefix, as a @ref SI_Prefix_t type.
 * 
 */
#define SI_getPrefixFactor(prefix) pow(10, prefix)

/** @brief Dimensionless unit. */
extern const Unit_t* const DIMENSIONLESS;
/** @brief Meter, unit of distance. */
extern const Unit_t* const METER;
/** @brief Second, unit of time. */
extern const Unit_t* const SECOND;
/** @brief Gram, unit of mass. */
extern const Unit_t* const GRAM;
/** @brief Kilogram, unit of mass. */
extern const Unit_t* const KILOGRAM;
/** @brief Volt, unit of electrical voltage. */
extern const Unit_t* const VOLT;
/** @brief Ampere, unit of electrical current. */
extern const Unit_t* const AMPERE;
/** @brief Ohm, unit of electrical resistance. */
extern const Unit_t* const OHM;
/** @brief Decibel, logarithmic pseudo-unit. */
extern const Unit_t* const DECIBELS_P;

/**
 * @brief Initializes a new @ref Quantity_t. Always
 * use this to create a new @ref Quantity_t.
 * 
 * This also auto-applies a prefix to the unit if possible.
 * 
 * @param number The numerical part of the quantity.
 * @param unit The unit the numerical part of the quantity is given in.
 * @param precision The precision the numerical part of the quantity is given in.
 * @return A new @ref Quantity_t struct containing the specified information.
 */
Quantity_t SI(double number, const Unit_t* unit, uint8_t precision);
/**
 * @brief Applies a matching SI prefix to a quantity if possible.
 * 
 * @param quantity A pointer to the @ref Quantity_t you want to apply a SI prefix to.
 */
void SI_applyPrefix(Quantity_t* quantity);
/**
 * @brief Removes any SI prefixes from a quantity.
 * 
 * @param quantity A pointer to the @ref Quantity_t you want to remove the SI prefixes from.
 */
void SI_removePrefix(Quantity_t* quantity);
/**
 * @brief Converts the given SI prefix to the specified prefix.
 * 
 * @param quantity A pointer to the @ref Quantity_t you want to apply a SI prefix to.
 * @param prefix The SI prefix you want to apply to the quantity.
 * 
 * @exception ERR_PRECISION_VIOLATED The number can't possibly be expressed without violating the quantities precision. However,
 * the quantity was still adjusted. This means you can catch the error and therefore bypass this check.
 */
void SI_setPrefix(Quantity_t* quantity, SI_Prefix_t prefix);

/**
 * @brief Converts a given quantity to the specified target unit.
 * 
 * @param quantity The initial quantity you want to convert from.
 * @param unit The unit you want to convert to.
 * @return A new @ref Quantity_t struct containing the converted quantity.
 */
Quantity_t SI_convert(Quantity_t quantity, const Unit_t* const unit);

/**
 * @brief Gets a pointer pointing to the string with the prefix abbreviation of prefix of the specified quantity.
 * 
 * @param prefix The quantity you want to get the prefix abbreviation from.
 * @return A pointer to the string-form prefix abbreviation.
 */
const char* SI_getPrefixString(Quantity_t quantity);

/**
 * @brief Converts a given quantity a string and stores it in @p s.
 * 
 * @param quantity The quantity you want to convert into a string.
 * @param s A pointer to the string you want to convert to.
 * @param maxLength Length of @p s limiting the maximum output length.
 */
void SI_toString(Quantity_t quantity, char* s, size_t maxLength);
/**
 * @brief Prints the given quantity to stdout.
 * 
 * @param quantity The quantity you want to print.
 */
void SI_print(Quantity_t quantity);

#endif /* SI_UNITS_H_ */