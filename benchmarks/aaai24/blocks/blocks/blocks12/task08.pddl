(define (problem BW-12-9546-8)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 - block)
    (:init
        (handempty)
        (on b1 b9)
        (on-table b2)
        (on b3 b12)
        (on b4 b6)
        (on b5 b7)
        (on b6 b1)
        (on b7 b3)
        (on-table b8)
        (on-table b9)
        (on b10 b2)
        (on-table b11)
        (on b12 b11)
        (clear b4)
        (clear b5)
        (clear b8)
        (clear b10)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on b3 b11)
            (on b4 b12)
            (on b5 b8)
            (on b6 b1)
            (on-table b7)
            (on b8 b6)
            (on b9 b5)
            (on-table b10)
            (on-table b11)
            (on b12 b7)
        )
    )
)