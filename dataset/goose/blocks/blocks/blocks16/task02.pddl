(define (problem BW-16-1-2)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 - block)
    (:init
        (handempty)
        (on b1 b15)
        (on b2 b10)
        (on b3 b13)
        (on b4 b12)
        (on b5 b9)
        (on b6 b1)
        (on b7 b14)
        (on b8 b7)
        (on b9 b4)
        (on-table b10)
        (on b11 b16)
        (on b12 b8)
        (on-table b13)
        (on b14 b6)
        (on b15 b11)
        (on b16 b2)
        (clear b3)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b15)
            (on-table b2)
            (on-table b3)
            (on-table b4)
            (on b5 b16)
            (on b6 b5)
            (on b7 b12)
            (on b8 b4)
            (on b9 b6)
            (on b10 b8)
            (on-table b11)
            (on b12 b14)
            (on b13 b7)
            (on b14 b3)
            (on b15 b11)
            (on b16 b10)
        )
    )
)