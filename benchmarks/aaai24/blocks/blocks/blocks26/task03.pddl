(define (problem BW-26-1-3)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 - block)
    (:init
        (handempty)
        (on b1 b15)
        (on b2 b12)
        (on-table b3)
        (on b4 b24)
        (on b5 b20)
        (on b6 b3)
        (on b7 b17)
        (on b8 b16)
        (on b9 b18)
        (on b10 b2)
        (on b11 b21)
        (on b12 b7)
        (on b13 b25)
        (on b14 b10)
        (on b15 b4)
        (on b16 b26)
        (on-table b17)
        (on b18 b13)
        (on b19 b14)
        (on b20 b6)
        (on b21 b22)
        (on b22 b9)
        (on b23 b5)
        (on-table b24)
        (on b25 b1)
        (on b26 b11)
        (clear b8)
        (clear b19)
        (clear b23)
    )
    (:goal
        (and
            (on b1 b23)
            (on b2 b15)
            (on b3 b17)
            (on b4 b2)
            (on b5 b26)
            (on b6 b9)
            (on b7 b21)
            (on b8 b1)
            (on b9 b4)
            (on b10 b24)
            (on b11 b25)
            (on-table b12)
            (on b13 b18)
            (on b14 b5)
            (on b15 b7)
            (on b16 b6)
            (on b17 b20)
            (on b18 b11)
            (on b19 b16)
            (on-table b20)
            (on b21 b13)
            (on b22 b14)
            (on b23 b19)
            (on b24 b22)
            (on b25 b3)
            (on b26 b12)
        )
    )
)