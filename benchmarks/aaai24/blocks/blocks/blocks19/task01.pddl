(define (problem BW-19-1-1)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 - block)
    (:init
        (handempty)
        (on b1 b3)
        (on b2 b16)
        (on b3 b11)
        (on-table b4)
        (on b5 b6)
        (on b6 b18)
        (on b7 b8)
        (on b8 b4)
        (on-table b9)
        (on-table b10)
        (on-table b11)
        (on b12 b5)
        (on b13 b17)
        (on-table b14)
        (on-table b15)
        (on-table b16)
        (on b17 b12)
        (on b18 b15)
        (on-table b19)
        (clear b1)
        (clear b2)
        (clear b7)
        (clear b9)
        (clear b10)
        (clear b13)
        (clear b14)
        (clear b19)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b17)
            (on b4 b7)
            (on b5 b2)
            (on b6 b4)
            (on b7 b15)
            (on-table b8)
            (on b9 b19)
            (on-table b10)
            (on b11 b13)
            (on b12 b3)
            (on b13 b16)
            (on-table b14)
            (on b15 b12)
            (on b16 b18)
            (on b17 b14)
            (on b18 b10)
            (on b19 b8)
        )
    )
)