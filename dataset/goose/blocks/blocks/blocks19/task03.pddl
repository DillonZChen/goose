(define (problem BW-19-1-3)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 - block)
    (:init
        (handempty)
        (on b1 b11)
        (on b2 b4)
        (on-table b3)
        (on b4 b6)
        (on b5 b1)
        (on b6 b10)
        (on b7 b8)
        (on b8 b2)
        (on-table b9)
        (on b10 b18)
        (on b11 b13)
        (on-table b12)
        (on b13 b12)
        (on b14 b19)
        (on b15 b14)
        (on b16 b5)
        (on b17 b3)
        (on-table b18)
        (on b19 b9)
        (clear b7)
        (clear b15)
        (clear b16)
        (clear b17)
    )
    (:goal
        (and
            (on b1 b17)
            (on b2 b9)
            (on b3 b12)
            (on-table b4)
            (on b5 b15)
            (on b6 b16)
            (on b7 b5)
            (on b8 b2)
            (on-table b9)
            (on-table b10)
            (on-table b11)
            (on b12 b13)
            (on b13 b19)
            (on b14 b7)
            (on b15 b3)
            (on b16 b11)
            (on b17 b4)
            (on-table b18)
            (on b19 b6)
        )
    )
)