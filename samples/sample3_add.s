        .data
array:  .word   3
        .text
main:
        and     $1, $2, $3
		and     $2, $3, $4
		and     $3, $4, $5
