(define (problem BW-10-9000-5)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block)
    (:init
        (handempty)
        (on-table b1)
        (on b2 b4)
        (on b3 b8)
        (on b4 b3)
        (on b5 b2)
        (on b6 b10)
        (on-table b7)
        (on-table b8)
        (on b9 b6)
        (on b10 b5)
        (clear b1)
        (clear b7)
        (clear b9)
    )
    (:goal
        (and
            (on b1 b6)
            (on b2 b5)
            (on b3 b1)
            (on b4 b3)
            (on b5 b7)
            (on-table b6)
            (on b7 b9)
            (on-table b8)
            (on b9 b10)
            (on b10 b4)
        )
    )
)