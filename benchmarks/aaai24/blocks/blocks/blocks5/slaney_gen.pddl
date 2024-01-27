

(define (problem BW-5-168-1)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b5)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b4)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-2)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b5)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (on b5 b2)
        (clear b1)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
            (on b4 b3)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-3)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on b2 b4)
        (on-table b3)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b3)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b1)
            (on b3 b2)
            (on b4 b5)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-4)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b5)
        (on-table b2)
        (on-table b3)
        (on b4 b2)
        (on b5 b3)
        (clear b1)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b5)
            (on b4 b2)
            (on b5 b4)
        )
    )
)


(define (problem BW-5-168-5)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b4)
        (on b4 b5)
        (on b5 b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b5)
            (on b2 b3)
            (on-table b3)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-6)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (on b4 b5)
        (on b5 b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
            (on b4 b5)
            (on b5 b3)
        )
    )
)


(define (problem BW-5-168-7)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on b2 b5)
        (on-table b3)
        (on-table b4)
        (on b5 b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on-table b3)
            (on b4 b2)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-8)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b5)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b4)
            (on b4 b5)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-9)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on-table b2)
        (on b3 b2)
        (on b4 b3)
        (on b5 b1)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b5)
            (on b4 b1)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-168-10)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b4)
        (on b3 b1)
        (on-table b4)
        (on b5 b2)
        (clear b3)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b1)
            (on b3 b5)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-11)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on b2 b5)
        (on b3 b4)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b5)
            (on b4 b3)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-12)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b4)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b3)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on b3 b2)
            (on b4 b3)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-13)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on-table b2)
        (on-table b3)
        (on b4 b5)
        (on b5 b2)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on b3 b5)
            (on-table b4)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-168-14)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b5)
        (on-table b2)
        (on-table b3)
        (on b4 b1)
        (on b5 b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b5)
            (on-table b3)
            (on-table b4)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-15)
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
            (on b1 b3)
            (on b2 b5)
            (on b3 b2)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-16)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on b4 b5)
        (on b5 b2)
        (clear b1)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b5)
            (on b3 b4)
            (on-table b4)
            (on b5 b3)
        )
    )
)


(define (problem BW-5-168-17)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (on b5 b3)
        (clear b1)
        (clear b2)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
            (on b4 b5)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-18)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on b2 b1)
        (on-table b3)
        (on b4 b5)
        (on-table b5)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on-table b3)
            (on-table b4)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-168-19)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on b2 b5)
        (on-table b3)
        (on b4 b2)
        (on b5 b3)
        (clear b1)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b5)
            (on b4 b1)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-20)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b1)
        (on-table b3)
        (on b4 b2)
        (on-table b5)
        (clear b3)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on b3 b1)
            (on b4 b5)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-21)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on b2 b3)
        (on-table b3)
        (on b4 b5)
        (on b5 b2)
        (clear b1)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b1)
            (on-table b3)
            (on-table b4)
            (on b5 b3)
        )
    )
)


(define (problem BW-5-168-22)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (on-table b5)
        (clear b1)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b5)
            (on-table b2)
            (on b3 b1)
            (on-table b4)
            (on b5 b4)
        )
    )
)


(define (problem BW-5-168-23)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b1)
        (on-table b3)
        (on b4 b3)
        (on-table b5)
        (clear b2)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b4)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-24)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (on b4 b3)
        (on-table b5)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b1)
            (on b3 b4)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-25)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on b4 b2)
        (on b5 b1)
        (clear b3)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b5)
            (on b3 b4)
            (on-table b4)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-26)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b5)
        (on-table b2)
        (on b3 b2)
        (on-table b4)
        (on b5 b3)
        (clear b1)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b5)
            (on-table b2)
            (on b3 b1)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-27)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on-table b2)
        (on b3 b2)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b3)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-28)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b4)
        (on b4 b5)
        (on b5 b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b5)
            (on b2 b1)
            (on-table b3)
            (on-table b4)
            (on b5 b3)
        )
    )
)


(define (problem BW-5-168-29)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on b2 b3)
        (on b3 b5)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on b3 b4)
            (on-table b4)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-30)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b5)
        (on b2 b3)
        (on-table b3)
        (on b4 b1)
        (on-table b5)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on-table b3)
            (on b4 b5)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-31)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on b2 b4)
        (on-table b3)
        (on-table b4)
        (on b5 b2)
        (clear b1)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on b3 b2)
            (on b4 b1)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-32)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b5)
        (on b4 b1)
        (on-table b5)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b5)
            (on b2 b3)
            (on b3 b4)
            (on b4 b1)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-33)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (on b5 b2)
        (clear b1)
        (clear b3)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
            (on b4 b2)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-34)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on b2 b4)
        (on-table b3)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b2)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b5)
            (on-table b3)
            (on b4 b2)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-35)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b4)
        (on-table b3)
        (on b4 b1)
        (on-table b5)
        (clear b2)
        (clear b3)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on b3 b5)
            (on b4 b3)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-36)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on-table b2)
        (on-table b3)
        (on b4 b5)
        (on-table b5)
        (clear b1)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
            (on-table b4)
            (on b5 b4)
        )
    )
)


(define (problem BW-5-168-37)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b5)
        (on-table b4)
        (on b5 b4)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b5)
            (on b2 b3)
            (on b3 b1)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-38)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (on-table b4)
        (on b5 b4)
        (clear b2)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
            (on b4 b5)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-168-39)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (on-table b5)
        (clear b1)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b1)
            (on b3 b2)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-40)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b5)
        (on b3 b4)
        (on b4 b1)
        (on-table b5)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on b3 b5)
            (on-table b4)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-41)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on b4 b2)
        (on b5 b1)
        (clear b3)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b5)
            (on b2 b3)
            (on-table b3)
            (on b4 b1)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-42)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b5)
        (on b2 b3)
        (on-table b3)
        (on-table b4)
        (on b5 b2)
        (clear b1)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b4)
            (on-table b4)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-43)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on-table b2)
        (on b3 b2)
        (on-table b4)
        (on b5 b1)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
            (on b4 b3)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-44)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (on-table b4)
        (on b5 b2)
        (clear b1)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
            (on b4 b2)
            (on b5 b4)
        )
    )
)


(define (problem BW-5-168-45)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on b2 b4)
        (on b3 b5)
        (on b4 b3)
        (on-table b5)
        (clear b1)
    )
    (:goal
        (and
            (on b1 b5)
            (on-table b2)
            (on b3 b4)
            (on-table b4)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-168-46)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (on-table b5)
        (clear b1)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on b3 b5)
            (on b4 b1)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-168-47)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b5)
        (on b4 b1)
        (on-table b5)
        (clear b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b5)
            (on b3 b2)
            (on-table b4)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-48)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on b2 b1)
        (on b3 b2)
        (on-table b4)
        (on b5 b3)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b5)
            (on b2 b1)
            (on-table b3)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-49)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (on b4 b1)
        (on b5 b2)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on-table b3)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-50)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b4)
        (on b4 b1)
        (on-table b5)
        (clear b2)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b5)
            (on-table b3)
            (on-table b4)
            (on b5 b4)
        )
    )
)


(define (problem BW-5-168-51)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b5)
        (on b3 b4)
        (on b4 b1)
        (on b5 b3)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b5)
            (on b4 b2)
            (on b5 b4)
        )
    )
)


(define (problem BW-5-168-52)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on-table b2)
        (on b3 b4)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b2)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b5)
            (on b4 b2)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-53)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b4)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (on b5 b3)
        (clear b1)
        (clear b2)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
            (on-table b4)
            (on b5 b4)
        )
    )
)


(define (problem BW-5-168-54)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on b2 b1)
        (on-table b3)
        (on b4 b5)
        (on-table b5)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b5)
            (on b3 b2)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-55)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (on b5 b2)
        (clear b1)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b2)
            (on-table b4)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-56)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on-table b2)
        (on b3 b5)
        (on-table b4)
        (on-table b5)
        (clear b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b5)
            (on-table b4)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-168-57)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b3)
        (on b2 b1)
        (on-table b3)
        (on b4 b5)
        (on-table b5)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on b3 b5)
            (on b4 b3)
            (on b5 b1)
        )
    )
)


(define (problem BW-5-168-58)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on b4 b1)
        (on-table b5)
        (clear b2)
        (clear b3)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on b1 b5)
            (on b2 b3)
            (on-table b3)
            (on b4 b1)
            (on b5 b2)
        )
    )
)


(define (problem BW-5-168-59)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on b1 b2)
        (on b2 b4)
        (on b3 b5)
        (on-table b4)
        (on b5 b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b5)
            (on-table b2)
            (on-table b3)
            (on b4 b1)
            (on-table b5)
        )
    )
)


(define (problem BW-5-168-60)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b2)
        (on-table b4)
        (on-table b5)
        (clear b3)
        (clear b4)
        (clear b5)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on-table b3)
            (on b4 b5)
            (on-table b5)
        )
    )
)