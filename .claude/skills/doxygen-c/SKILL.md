---
name: doxygen-c
description: Write or review Doxygen comments for Street Rod's MSC 5.1 C headers, structs, enums and functions; configure or run the documentation lint gate.
---

# Document Street Rod C interfaces

Adapted from bak2's doxygen-c standard for MSC 5.1 and this tree. Apply when
explicitly asked to document or review C. CLAUDE.md still forbids unsolicited
source comments; this skill does not authorize a documentation sweep.

Documentation explains the **game**, its interfaces, algorithms and runtime
formats. Decompilation addresses, hash/budget/stack-layout matching constraints,
source-reconstruction choices and storage banners belong in
`docs/RECONSTRUCTION.md`, never source comments. Preserve every **PROVISIONAL**
marker and its full text. State uncertainty honestly; byte identity does not
prove a semantic name or explanation.

## Ownership and dialect

Files use uppercase DOS 8.3 names in `src/<subsystem>/`. A module's `.H` owns
its public function/global declarations. All function documentation lives in
the module's `.H` (user rule, 2026-11-05): public functions at their header
declaration; private static functions in a `/** @fn <full prototype> ... */`
block in the same `.H` (they stay declared and defined only in the `.C`; don't add
static prototypes to headers). The `.C` carries no Doxygen blocks. Use one block
per entity, not duplicates in both places. Enum values in prose and `@param`
contracts are `@ref` names (e.g. `@ref CAR_PULL_IN`), never raw numbers. Types belong in the header of
the module defining their primary global, or their producer/API when there is no global storage; related enums and helper records
follow that owner. Only independent ownerless helpers remain in `src/SYS/STRUCTS.H`.

`./sr headers` rewrites C include blocks and MAKEFILE dependencies. Keep docs
outside those generated include blocks and beside the owning declarations.
Do not add ad hoc externs, local structs, or public prototypes to a `.C` merely
to give Doxygen something to parse. Run `./sr headers --check` after changes.

Write actual MSC 5.1 C: block comments `/** ... */` or `/*! ... */`, including
trailing `/**< ... */`. Never introduce `//`, C++ references, anonymous unions,
C99 syntax, inline assembly or static_assert. Don't add modern compiler checks
to game source. Keep declaration spelling and far/near qualifiers intact.
Medium-model data pointers are normally near; far pointers include a segment.
Describe sizes, ownership, lifetimes and units from evidence, not modern-host
sizeof assumptions. Changing int to enum or renaming a local is a code change
that needs byte gates, not a comment-only edit.

## Core rules

1. Every documented `.H`/`.C` declaring file-level items needs a `@file` block.
   Use the real uppercase filename or bare `@file`. Without this, file-scope
   entities can be omitted with EXTRACT_ALL=NO even when their comments exist.
2. Put comments adjacent to declarations. Avoid detached `@fn`, `@var`,
   `@struct` commands unless adjacency is impossible.
3. The first sentence supplies a brief (JAVADOC_AUTOBRIEF); use a blank line
   for details. A file's brief names its purpose, not the reconstruction work.
4. Every parameter gets `@param`, including self-evident ones. Explain contract:
   range, units, nullability, ownership, capacity, direction or lifetime. Use
   `[in]`, `[out]`, `[in,out]` where helpful. Use `@p name` in prose.
5. Document results with `@return` or individual `@retval` cases. Explain
   sentinel/error values and relevant side effects. Void needs no @return.
6. Document every struct/union member and enum value. Short descriptions use
   `/**< ... */`; long ones use a preceding block. Include meaningful runtime
   format offsets/sizes, coordinate systems, units, bit meanings and sentinels.
   Do not annotate original EXE or DGROUP addresses. Don't invent packing.
7. **Every reference to a code symbol or file must use `@ref name`.** No bare
   prose names, `fn()`/`::Type` autolinks, `@see name`, or FILE.C:123 pins.
   Only explicit refs are guarded against renames by Doxygen warnings. Use
   `@ref Type::member` for members, `@see @ref name` for see-also entries.
8. If a target is not yet visible/documented, use `@c name (TBD-ref: reason)`.
   Keep the literal TBD-ref token so it can be searched and upgraded. Never
   use `#tbd` (the # operator requests a link). A nested block comment would
   terminate the documentation comment, so use parentheses for the reason.
9. Wrap manually at 80 columns. Keep `@ref name` and `@p name` together.
   When a trailing member comment overflows, move it before the declaration.

Examples below are illustrative interfaces, not claims about existing symbols:

```c
/**
 * @file EXAMPLE.H
 * @brief Car record access for the garage menu.
 */

/** One car entry in a menu. */
typedef struct MenuCar {
    unsigned short price; /**< Asking price, in dollars. */
    unsigned char kind;   /**< Index into the menu's car-kind table. */
    /** Name stored in the menu's buffer; valid until the menu closes. */
    char *name;
} MenuCar;

/** Menu selection outcome. */
typedef enum MenuResult {
    MENU_CANCEL = -1, /**< The player dismissed the menu. */
    MENU_SELECT = 0   /**< A car was selected. */
} MenuResult;

/**
 * Find a car in the current menu.
 *
 * @param[in] slot Zero-based car slot; must be within the current list.
 * @return Borrowed pointer valid until the menu closes, or NULL if empty.
 * @see @ref MenuCar
 */
MenuCar *ex_find_car(int slot);
```

For a union, explain which tag selects each member and how it is interpreted.
Only document actual named C members; don't replace legacy declarations with
anonymous members for presentation. For format records, say whether an offset
is relative to the record or file and verify byte order and padding separately.

For module narratives use `@defgroup module_name` once, normally in the owning
header, followed by `@{` and a closing `@}`. Additional files use `@ingroup` or
`@addtogroup`. `@file` is about the file; a group explains a game subsystem.
Don't duplicate the same overview in header, source and group. Use the README
for project-wide front matter, not a module's API description.

## Build and doc-lint

```sh
./sr docs
# Explicit environment for scripts:
nix-shell shell.nix --run './sr docs'
```

The command builds HTML at `work/doxygen/html/index.html`, XML at
`work/doxygen/xml`, records diagnostics in `work/doxygen/doxygen.warn` and
`build.log`, and writes coverage.json. Any Doxygen failure or nonempty warning
log fails the command. Configured checks include malformed commands, unresolved
@ref targets, incorrect @param names, incomplete parameter documentation and
missing parameter documentation on documented functions. Tests exercise these
behaviors with real Doxygen fixture headers.

The Doxyfile searches all subsystem header folders and MSC's INCLUDE directory,
maps uppercase .C/.H to C and strips DOS qualifiers **only for Doxygen** through
PREDEFINED: far/near/huge, calling conventions and interrupt keywords. Actual
source and compiler preprocessing remain unchanged. EXPAND_ONLY_PREDEF keeps
unrelated macros intact. Do not hide a parsing problem by excluding a header.

EXTRACT_ALL=NO and WARN_IF_UNDOCUMENTED=NO allow gradual documentation without
making every existing undocumented declaration fail. This is not permission to
suppress warnings about new docs. With sparse coverage, add @file and document
the target before replacing a TBD-ref with @ref. If a target remains absent,
keep the explicit TBD-ref and reason. Never set EXTRACT_ALL=YES just to silence
refs or claim complete coverage. AUTOLINK_SUPPORT=NO prevents accidental links;
the @ref requirement is also a review rule, not an automatic prose checker.

Coverage reports documented game C function definitions versus the source
inventory, deduplicating XML declarations/definitions. It separately reports
files/headers with documentation versus all .C/.H files. Assembly is excluded.
Zero warnings is a validity baseline, not proof of documentation completeness.
Run `grep -R -n TBD-ref src` during review; the tool does not enforce semantic
truth, line wrapping or every bare-name mention.

Before finishing source changes run the normal headers, function, packed-build,
DGROUP and unittest gates in CLAUDE.md, plus `./sr docs`. Do not add source
comments unless the user requested them. Put new reconstruction findings in
`docs/RECONSTRUCTION.md` rather than sneaking them into Doxygen notes.
