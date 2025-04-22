#ifndef XNN_FLOAT16_UTILS_H
#define XNN_FLOAT16_UTILS_H


namespace xnn {


// convert float to half precision floating point
unsigned short float32_to_float16(float value);

// convert half precision floating point to float
float float16_to_float32(unsigned short value);


// multiply two float16 value, a and b is float32 value in float16 precision
// which means a and b is first convert to float16 and then convert back to float32
float float16_multiply(float a, float b);

// multiply two float16 value, a and b is unsigned short value in float16 format 
unsigned short float16_multiply(unsigned short a, unsigned short b);

// add two float16 value, a and b is float32 value in float16 precisioni
float float16_add(float a, float b);

// add two float16 value, a and b is unsigned short value in float16 format 
unsigned short float16_add(unsigned short a, unsigned short b);

// substract two float16 value, a and b is float32 value in float16 precisioni
float float16_sub(float a, float b);

// substract two float16 value, a and b is unsigned short value in float16 format 
unsigned short float16_sub(unsigned short a, unsigned short b);

// multiply two float16 value and add another float16 value.
// a, b and c are float32 value in float16 precision
// which means a , b and c is first convert to float16 and then convert back to float32
float float16_multiply_and_add(float a, float b, float c);

// multiply two float16 value and add another float16 value.
// a, b and c are unsigned short value in float16 format
unsigned short float16_multiply_and_add(unsigned short a, unsigned short b, unsigned short c);



} // namespace xnn

#endif
