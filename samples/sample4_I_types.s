        .data
array:  .word   3
        .text
main:
        addiu   $2, $0, 1024
        addu    $3, $2, $2
        or      $4, $3, $2
        addiu   $5, $0, 1234
