(define (problem BW-11-6452-29)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 - block)
    (:init
        (handempty)
        (on b1 b4)
        (on-table b2)
        (on b3 b2)
        (on-table b4)
        (on b5 b9)
        (on-table b6)
        (on b7 b1)
        (on-table b8)
        (on b9 b8)
        (on b10 b11)
        (on b11 b5)
        (clear b3)
        (clear b6)
        (clear b7)
        (clear b10)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on b3 b5)
            (on b4 b3)
            (on b5 b6)
            (on b6 b9)
            (on b7 b11)
            (on-table b8)
            (on-table b9)
            (on b10 b1)
            (on b11 b8)
        )
    )
)