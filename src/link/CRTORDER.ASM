; The shipped runtime _TEXT sits between game code regions 1C11 and 2196.
; Declare it in that order; its real code/data comes from untouched MSC objects
; later in the link input. PUBLIC segment combination supplies the contents.
; This anchor allocates no bytes. The resulting game addresses are all exact.
_TEXT SEGMENT WORD PUBLIC 'CODE'
_TEXT ENDS
END
