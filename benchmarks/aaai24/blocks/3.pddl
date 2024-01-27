

(define (problem BW-3-9000-1)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9000-2)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b1)
        (on-table b3)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9000-3)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b3)
        (on b2 b1)
        (on-table b3)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9000-4)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9000-5)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b3)
        (on-table b2)
        (on b3 b2)
        (clear b1)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
        )
    )
)
