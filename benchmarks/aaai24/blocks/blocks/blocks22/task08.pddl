(define (problem BW-22-1-8)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 - block)
    (:init
        (handempty)
        (on-table b1)
        (on b2 b7)
        (on b3 b5)
        (on b4 b14)
        (on b5 b20)
        (on-table b6)
        (on b7 b22)
        (on b8 b10)
        (on-table b9)
        (on b10 b11)
        (on b11 b1)
        (on b12 b15)
        (on b13 b16)
        (on b14 b3)
        (on b15 b8)
        (on b16 b4)
        (on b17 b18)
        (on b18 b12)
        (on b19 b6)
        (on b20 b17)
        (on-table b21)
        (on-table b22)
        (clear b2)
        (clear b9)
        (clear b13)
        (clear b19)
        (clear b21)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b7)
            (on-table b3)
            (on b4 b15)
            (on b5 b6)
            (on b6 b21)
            (on b7 b19)
            (on b8 b22)
            (on b9 b8)
            (on b10 b2)
            (on b11 b16)
            (on b12 b13)
            (on-table b13)
            (on b14 b20)
            (on b15 b12)
            (on b16 b17)
            (on-table b17)
            (on-table b18)
            (on b19 b3)
            (on b20 b1)
            (on b21 b4)
            (on b22 b11)
        )
    )
)