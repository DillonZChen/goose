(define (problem BW-3-9804-35)
    (:domain blocksworld)
    (:objects b1 b2 b3 - block)
    (:init
        (handempty)
        (on-table b1)
        (on-table b2)
        (on b3 b2)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
        )
    )
)