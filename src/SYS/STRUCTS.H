/**
 * @file STRUCTS.H
 * @brief Shared boolean and word helper types.
 */

#ifndef STRUCTS_H
#define STRUCTS_H

typedef int BOOL;
/**
 * @name Boolean values
 * @brief Integer truth values used by game interfaces.
 * @{
 */
#define FALSE 0
#define TRUE 1
/** @} */

/** @brief A signed word and its two little-endian bytes. */
union Word {
    int w;              /**< Whole word, retaining the raw bit pattern. */
    unsigned char b[2]; /**< Low byte followed by high byte. */
};
/** @brief Two signed values whose meanings are defined by each table. */
struct Pair {
    int a; /**< First value: an x coordinate, span start, or auxiliary value. */
    int b; /**< Second value: a y coordinate, span width, or auxiliary value. */
};

#endif
