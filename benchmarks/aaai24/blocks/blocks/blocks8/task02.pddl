(define (problem BW-8-3326-2)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 - block)
    (:init
        (handempty)
        (on b1 b4)
        (on b2 b7)
        (on-table b3)
        (on b4 b3)
        (on b5 b8)
        (on-table b6)
        (on b7 b5)
        (on b8 b1)
        (clear b2)
        (clear b6)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b5)
            (on b4 b2)
            (on b5 b4)
            (on b6 b7)
            (on-table b7)
            (on b8 b6)
        )
    )
)