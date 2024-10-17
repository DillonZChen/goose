

(define (problem BW-4-5774-1)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b2)
        (on-table b2)
        (on-table b3)
        (on b4 b1)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-2)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b4)
        (on b4 b2)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b1)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-3)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (on b4 b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b1)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-4)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b2)
        (on-table b4)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b1)
            (on-table b3)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-5)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on-table b2)
        (on b3 b2)
        (on-table b4)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on b3 b1)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-6)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (on b4 b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on b3 b1)
            (on b4 b3)
        )
    )
)


(define (problem BW-4-5774-7)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b4)
        (on-table b3)
        (on b4 b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b2)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-8)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on-table b2)
        (on b3 b2)
        (on b4 b1)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b4)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-9)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b4)
        (on-table b3)
        (on-table b4)
        (clear b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b4)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-10)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (on b4 b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-11)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b4)
        (on-table b3)
        (on b4 b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b1)
            (on-table b3)
            (on b4 b3)
        )
    )
)


(define (problem BW-4-5774-12)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b4)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-13)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (clear b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
            (on b4 b3)
        )
    )
)


(define (problem BW-4-5774-14)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b4)
        (on b3 b1)
        (on b4 b3)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on b3 b1)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-15)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b4)
        (on-table b4)
        (clear b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-16)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (on b4 b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on-table b3)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-17)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b2)
        (on b2 b4)
        (on b3 b1)
        (on-table b4)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b4)
            (on-table b3)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-18)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (clear b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on b3 b2)
            (on b4 b3)
        )
    )
)


(define (problem BW-4-5774-19)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on b2 b1)
        (on-table b3)
        (on-table b4)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-20)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on-table b2)
        (on b3 b1)
        (on b4 b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b3)
            (on-table b3)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-21)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b4)
        (on b3 b2)
        (on-table b4)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b4)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-22)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (clear b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on b3 b2)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-23)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b4)
        (on b3 b2)
        (on b4 b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b4)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-24)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (on b4 b2)
        (clear b1)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on-table b3)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-25)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b4)
        (on b3 b1)
        (on-table b4)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b4)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-26)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on-table b2)
        (on b3 b2)
        (on b4 b3)
        (clear b1)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on-table b3)
            (on b4 b3)
        )
    )
)


(define (problem BW-4-5774-27)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (clear b1)
        (clear b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on b3 b1)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-28)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b2)
        (on b2 b3)
        (on b3 b4)
        (on-table b4)
        (clear b1)
    )
    (:goal
        (and
            (on b1 b2)
            (on b2 b4)
            (on b3 b1)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-29)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on b2 b1)
        (on-table b3)
        (on-table b4)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b4)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-30)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on b4 b1)
        (clear b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-31)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b4)
        (on-table b4)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b4)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-32)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on b2 b1)
        (on b3 b2)
        (on-table b4)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on b3 b2)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-33)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b1)
        (on-table b4)
        (clear b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
            (on b4 b3)
        )
    )
)


(define (problem BW-4-5774-34)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (on b4 b2)
        (clear b1)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b4)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-35)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (on b4 b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b4)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-36)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-37)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on b2 b1)
        (on-table b3)
        (on b4 b2)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-38)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on-table b4)
        (clear b1)
        (clear b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on b3 b2)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-39)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (on-table b4)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on-table b3)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-40)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b4)
        (on b3 b1)
        (on b4 b3)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on b3 b2)
            (on b4 b3)
        )
    )
)


(define (problem BW-4-5774-41)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b4)
        (on b3 b2)
        (on b4 b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on-table b3)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-42)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b4)
        (on b4 b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on-table b3)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-43)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on b2 b1)
        (on-table b3)
        (on-table b4)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on-table b3)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-44)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (on b4 b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b4)
            (on-table b3)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-45)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on-table b2)
        (on b3 b2)
        (on-table b4)
        (clear b1)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b2)
            (on b2 b3)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-46)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (on b4 b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b2)
            (on b4 b3)
        )
    )
)


(define (problem BW-4-5774-47)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (on-table b4)
        (clear b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b1)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-48)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (clear b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on b3 b1)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-49)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on-table b2)
        (on-table b3)
        (on b4 b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on-table b3)
            (on b4 b2)
        )
    )
)


(define (problem BW-4-5774-50)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b4)
        (on-table b4)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b1)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-51)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (on-table b4)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b4)
            (on b3 b2)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-52)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on-table b2)
        (on b3 b4)
        (on-table b4)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b4)
            (on-table b2)
            (on b3 b2)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-53)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (on-table b4)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on b3 b2)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-54)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b4)
        (on b2 b1)
        (on-table b3)
        (on-table b4)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on-table b3)
            (on b4 b1)
        )
    )
)


(define (problem BW-4-5774-55)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (on b4 b1)
        (clear b2)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b2)
            (on b4 b3)
        )
    )
)


(define (problem BW-4-5774-56)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b1)
        (on-table b3)
        (on b4 b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on b3 b1)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-57)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b1)
        (on-table b4)
        (clear b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b3)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-58)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (on b4 b3)
        (clear b4)
    )
    (:goal
        (and
            (on b1 b3)
            (on b2 b4)
            (on b3 b2)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-59)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on b1 b3)
        (on b2 b4)
        (on-table b3)
        (on b4 b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b4)
            (on b2 b3)
            (on-table b3)
            (on-table b4)
        )
    )
)


(define (problem BW-4-5774-60)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4)
    (:init
        (on-table b1)
        (on b2 b1)
        (on-table b3)
        (on-table b4)
        (clear b2)
        (clear b3)
        (clear b4)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b4)
            (on b3 b1)
            (on b4 b3)
        )
    )
)