(define (problem BW-7-6874-6)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 - block)
    (:init
        (handempty)
        (on b1 b5)
        (on b2 b6)
        (on b3 b4)
        (on b4 b1)
        (on b5 b2)
        (on b6 b7)
        (on-table b7)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b7)
            (on-table b3)
            (on-table b4)
            (on b5 b4)
            (on-table b6)
            (on b7 b1)
        )
    )
)