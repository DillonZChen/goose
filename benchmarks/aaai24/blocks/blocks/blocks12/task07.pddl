(define (problem BW-12-9546-7)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 - block)
    (:init
        (handempty)
        (on b1 b3)
        (on b2 b5)
        (on b3 b9)
        (on b4 b8)
        (on-table b5)
        (on b6 b1)
        (on b7 b6)
        (on-table b8)
        (on b9 b11)
        (on b10 b7)
        (on-table b11)
        (on b12 b2)
        (clear b4)
        (clear b10)
        (clear b12)
    )
    (:goal
        (and
            (on b1 b12)
            (on b2 b6)
            (on b3 b2)
            (on b4 b11)
            (on b5 b7)
            (on-table b6)
            (on b7 b3)
            (on-table b8)
            (on-table b9)
            (on-table b10)
            (on b11 b9)
            (on b12 b10)
        )
    )
)