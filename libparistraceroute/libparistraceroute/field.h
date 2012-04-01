#ifndef FIELD_H
#define FIELD_H

/**
 * \file field.h
 * \brief Header for header fields
 */

#include <stddef.h> // size_t
#include <stdint.h>

/**
 * \enum fieldtype_t
 * \brief Enumeration of the possible data types for a header field
 */
typedef enum {
	/** 4 bit integer */
    TYPE_INT4,
	/** 8 bit integer */
    TYPE_INT8,
	/** 16 bit integer */
    TYPE_INT16,
	/** 32 bit integer */
    TYPE_INT32,
	/** String */
    TYPE_STRING
} fieldtype_t;

/**
 * \struct field_t
 * \brief Structure describing a header field
 */
typedef struct {
	/** Pointer to a unique identifier key */
    char *key;
	/** Union of all field data */
    union {
		/** Pointer to raw data */
        void          * value;
		/** Value of data as a 4 bit integer */
        unsigned int    int4_value:4; 
		/** Value of data as an 8 bit integer */
        uint8_t         int8_value;
		/** Value of data as a 16 bit integer */
        uint16_t        int16_value;
		/** Value of data as a 32 bit integer */
        uint32_t        int32_value;
		/** Pointer to string data */
        unsigned char * string_value;
    };
    fieldtype_t type;
} field_t;

/**
 * \brief Create a field structure to hold an 8 bit integer value
 * \param key Pointer to a unique header identifier
 * \param value Value to store in the field
 * \return Structure containing the newly created field
 */
field_t * field_create_int8  (char *key, uint8_t  value);
/**
 * \brief Create a field structure to hold a 16 bit integer value
 * \param key Pointer to a unique header identifier
 * \param value Value to store in the field
 * \return Structure containing the newly created field
 */
field_t * field_create_int16 (char *key, uint16_t value);
/**
 * \brief Create a field structure to hold a 32 bit integer value
 * \param key Pointer to a unique header identifier
 * \param value Value to store in the field
 * \return Structure containing the newly created field
 */
field_t * field_create_int32 (char *key, uint32_t value);
/**
 * \brief Create a field structure to hold a string
 * \param key Pointer to a unique header identifier
 * \param value Value to store in the field
 * \return Structure containing the newly created field
 */
field_t * field_create_string(char *key, unsigned char * value);

field_t *field_create(fieldtype_t type, char *key, void *value);

/**
 * \brief Delete a field structure
 * \param field Pointer to the field structure to delete
 */
void      field_free(field_t *field);

/**
 * \brief Macro shorthand for field_create_int8
 * \param x Pointer to a char* key to identify the field
 * \param y Value to store in the field
 * \return Structure containing the newly created field
 */
#define I8(x, y)  field_create_int8(x, y)
/**
 * \brief Macro shorthand for field_create_int16
 * \param x Pointer to a char* key to identify the field
 * \param y Value to store in the field
 * \return Structure containing the newly created field
 */
#define I16(x, y) field_create_int16(x, y)
/**
 * \brief Macro shorthand for field_create_int32
 * \param x Pointer to a char* key to identify the field
 * \param y Value to store in the field
 * \return Structure containing the newly created field
 */
#define I32(x, y) field_create_int32(x, y)
/**
 * \brief Macro shorthand for field_create_string
 * \param x Pointer to a char* key to identify the field
 * \param y String to store in the field
 * \return Structure containing the newly created field
 */
#define STR(x, y) field_create_string(x, (unsigned char*)y)

size_t field_get_type_size(fieldtype_t type);
size_t field_get_size(field_t *field);

//Dump
void field_dump(field_t *field);

#endif
