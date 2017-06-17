/**
 * Implement tagged-union sum types in C, similar to Rust enums.
 *
 * Usage:
 *
 * ENUM(<type name>, <<variant type>, <variant name>>*);
 *
 * This creates an enum that is <type name>Tag, containing all of the variant
 * names as members, and also creates a struct that holds a union of all variant
 * types with their names, and a discriminant tag that is the aforementioned
 * enum. The enum values are of the form <type name>Tag_<variant name>, because
 * enum values are not scoped but are globally accessible symbols.
 *
 * SET_VARIANT(<type name>, <variant name>, <variant body>);
 *
 * This creates an instance of the struct defined by an ENUM() declaration, and
 * sets the discriminant tag to the given variant name and the union body to the
 * given body.
 *
 * GET_VARIANT_TYPE(<ident>);
 * GET_VARIANT_BODY(<ident>, <variant>);
 *
 * This extracts the body out of a variant created with SET_VARIANT, according
 * to the given discriminant tag. The variant tag is the same name as used in
 * the ENUM() declaration; it does NOT use the fully qualified name generated
 * for the associated enum.
 *
 * The C compiler won't help maintain type safety on these, so client code will
 * have to manually ensure that is extracting the same discriminant that was put
 * in.
 *
 * Note: ENUM() currently only supports up to thirteen variants in a single enum
 * set. If more than thirteen are required ... awkward.
 */

#ifndef WYZYRDRY_ENUM_H
#define WYZYRDRY_ENUM_H

/*
 * These two macros permit counting the number of arguments given to a variadic
 * macro. It is necessary for permitting appropriate dispatch.
 *
 * It works pushing the list of decrementing numbers to the right until the
 * appropriate number is in the argument slot that will be used as the count. C
 * macros are ... not great.
 */
#define _ARGC(_a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p, \
_q, _r, _s, _t, _u, _v, _w, _x, _y, _z, n, ...) n
#define ARGC(...) _ARGC(__VA_ARGS__, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define JOIN(_name, _var) _name ## _ ## _var

/*
 * I don't know why this many layers of indirection are necessary, but here we
 * are.
 */
#define MUX2(op, count) JOIN(op, count)
#define MUX1(op, count) MUX2(op, count)
#define MUX(op, count) MUX1(op, count)

/*
 * Build enum and union items for a type/name pair
 */
#define SPLIT_2(_name, _ty, _var, ...) JOIN(_name, _var)
#define LIST_2(_ty, _var, ...) _ty _var;

/*
 * Build enum and union items for two type/name pairs. As C does not support
 * direct recursion, this (and all subsequent) simply pops a pair out of varargs
 * and hands the rest off to the next macro down the line.
 */
#define SPLIT_4(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_2(_name, __VA_ARGS__)
#define LIST_4(_ty, _var, ...) _ty _var; LIST_2(__VA_ARGS__)

#define SPLIT_6(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_4(_name, __VA_ARGS__)
#define LIST_6(_ty, _var, ...) _ty _var; LIST_4(__VA_ARGS__)

#define SPLIT_8(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_6(_name, __VA_ARGS__)
#define LIST_8(_ty, _var, ...) _ty _var; LIST_6(__VA_ARGS__)

#define SPLIT_10(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_8(_name, __VA_ARGS__)
#define LIST_10(_ty, _var, ...) _ty _var; LIST_8(__VA_ARGS__)

#define SPLIT_12(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_10(_name, __VA_ARGS__)
#define LIST_12(_ty, _var, ...) _ty _var; LIST_10(__VA_ARGS__)

#define SPLIT_14(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_12(_name, __VA_ARGS__)
#define LIST_14(_ty, _var, ...) _ty _var; LIST_12(__VA_ARGS__)

#define SPLIT_16(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_14(_name, __VA_ARGS__)
#define LIST_16(_ty, _var, ...) _ty _var; LIST_14(__VA_ARGS__)

#define SPLIT_18(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_16(_name, __VA_ARGS__)
#define LIST_18(_ty, _var, ...) _ty _var; LIST_16(__VA_ARGS__)

#define SPLIT_20(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_18(_name, __VA_ARGS__)
#define LIST_20(_ty, _var, ...) _ty _var; LIST_18(__VA_ARGS__)

#define SPLIT_22(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_20(_name, __VA_ARGS__)
#define LIST_22(_ty, _var, ...) _ty _var; LIST_20(__VA_ARGS__)

#define SPLIT_24(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_22(_name, __VA_ARGS__)
#define LIST_24(_ty, _var, ...) _ty _var; LIST_22(__VA_ARGS__)

/*
 * Hope nobody needs an enum with more than thirteen variants.
 */
#define SPLIT_26(_name, _ty, _var, ...) JOIN(_name, _var), SPLIT_24(_name, __VA_ARGS__)
#define LIST_26(_ty, _var, ...) _ty _var; LIST_24(__VA_ARGS__)

/*
 * Build enum tags out of a type/pair list
 */
#define TAGS(_name, ...) \
MUX(SPLIT, ARGC(__VA_ARGS__))(_name, __VA_ARGS__)
/*
 * Build union variants out of a type/pair list
 */
#define VALS(...) \
MUX(LIST, ARGC(__VA_ARGS__))(__VA_ARGS__)

/*
 * Declare a full enum and tagged-union set from a name and type/pair sequence.
 */
#define ENUM(_name, ...) \
typedef enum ENUM_TAG(_name) { \
	TAGS(_name, __VA_ARGS__) \
} ENUM_TAG(_name); \
typedef struct _name { \
	union { VALS(__VA_ARGS__) } body; \
	ENUM_TAG(_name) tag; \
} _name

#define ENUM_TAG(_enum) _enum ## Tag
#define ENUM_VAR(_enum, _tag) JOIN(_enum, _tag)

/*
 * Create a specific variant of a tagged-union, with the given type and body.
 */
#define SET_VARIANT(_name, _tag, _body) \
((_name) { .body._tag = _body, .tag = JOIN(_name, _tag), })

#define GET_VARIANT_TYPE(_name) \
_name.tag

/*
 * Extract the body of a given tag from a given tagged-union item.
 */
#define GET_VARIANT_BODY(_name, _tag) \
_name.body._tag

#endif
