(define (problem BW-3-9804-56)
    (:domain blocksworld)
    (:objects b1 b2 b3 - block)
    (:init
        (handempty)
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b1)
            (on-table b3)
        )
    )
)