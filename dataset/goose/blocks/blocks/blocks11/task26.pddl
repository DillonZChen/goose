(define (problem BW-11-6452-26)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 - block)
    (:init
        (handempty)
        (on b1 b4)
        (on b2 b3)
        (on-table b3)
        (on-table b4)
        (on b5 b9)
        (on-table b6)
        (on b7 b2)
        (on b8 b1)
        (on b9 b11)
        (on b10 b6)
        (on b11 b10)
        (clear b5)
        (clear b7)
        (clear b8)
    )
    (:goal
        (and
            (on b1 b5)
            (on b2 b7)
            (on b3 b9)
            (on b4 b3)
            (on b5 b11)
            (on-table b6)
            (on b7 b1)
            (on b8 b2)
            (on b9 b10)
            (on-table b10)
            (on b11 b4)
        )
    )
)