(define (problem BW-7-9000-2)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 - block)
    (:init
        (handempty)
        (on b1 b7)
        (on-table b2)
        (on b3 b5)
        (on-table b4)
        (on b5 b1)
        (on b6 b3)
        (on-table b7)
        (clear b2)
        (clear b4)
        (clear b6)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b3)
            (on-table b3)
            (on b4 b7)
            (on b5 b6)
            (on-table b6)
            (on-table b7)
        )
    )
)