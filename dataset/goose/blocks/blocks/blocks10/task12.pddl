(define (problem BW-10-7268-12)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block)
    (:init
        (handempty)
        (on-table b1)
        (on-table b2)
        (on b3 b4)
        (on b4 b9)
        (on b5 b10)
        (on-table b6)
        (on b7 b2)
        (on b8 b5)
        (on b9 b8)
        (on b10 b6)
        (clear b1)
        (clear b3)
        (clear b7)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b6)
            (on-table b3)
            (on b4 b5)
            (on-table b5)
            (on-table b6)
            (on-table b7)
            (on-table b8)
            (on b9 b7)
            (on b10 b3)
        )
    )
)