

(define (problem BW-5-9000-1)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (on b5 b1)
        (clear b2)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b4)
            (on b4 b1)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-9000-2)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b5)
        (on b2 b4)
        (on b3 b2)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b5)
            (on-table b2)
            (on b3 b2)
            (on b4 b3)
            (on b5 b4)
        )
    )
)


(define (problem BW-5-9000-3)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b2)
        (on b4 b3)
        (on b5 b1)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b5)
            (on b2 b4)
            (on-table b3)
            (on-table b4)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-9000-4)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on b2 b5)
        (on-table b3)
        (on b4 b2)
        (on-table b5)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b5)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-9000-5)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b4)
        (on b3 b1)
        (on b4 b5)
        (on-table b5)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b5)
            (on-table b2)
            (on-table b3)
            (on-table b4)
            (on-table b5)
        )
    )
)
