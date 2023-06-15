

(define (problem BW-3-9804-1)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-2)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-3)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on-table b2)
        (on-table b3)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-4)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on b2 b3)
        (on-table b3)
        (clear b1)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-5)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on b2 b3)
        (on-table b3)
        (clear b1)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-6)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-7)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on-table b2)
        (on-table b3)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-8)
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
            (on b2 b1)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-9)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-10)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (clear b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-11)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-12)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
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


(define (problem BW-3-9804-13)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b2)
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


(define (problem BW-3-9804-14)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b2)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-15)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b2)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-16)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-17)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on b2 b3)
        (on-table b3)
        (clear b1)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-18)
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
            (on b1 b2)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-19)
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
            (on b1 b3)
            (on-table b2)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-20)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-21)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-22)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-23)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on b2 b3)
        (on-table b3)
        (clear b1)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-24)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on b2 b3)
        (on-table b3)
        (clear b1)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-25)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-26)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-27)
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
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-28)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b2)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-29)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b2)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-30)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-31)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-32)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-33)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-34)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-35)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
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


(define (problem BW-3-9804-36)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-37)
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
            (on b1 b2)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-38)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-39)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on-table b2)
        (on b3 b1)
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


(define (problem BW-3-9804-40)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b3)
        (on-table b2)
        (on-table b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-41)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
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


(define (problem BW-3-9804-42)
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
            (on b1 b3)
            (on b2 b1)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-43)
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


(define (problem BW-3-9804-44)
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
            (on-table b2)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-45)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-46)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b2)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b1)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-47)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-48)
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
            (on b1 b2)
            (on-table b2)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-49)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b2)
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


(define (problem BW-3-9804-50)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on-table b2)
        (on-table b3)
        (clear b1)
        (clear b2)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-51)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on-table b2)
        (on b3 b2)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-52)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b1)
        (on b3 b2)
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


(define (problem BW-3-9804-53)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-54)
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
            (on-table b1)
            (on-table b2)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-55)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on-table b2)
        (on-table b3)
        (clear b1)
        (clear b3)
    )
    (:goal
        (and
            (on b1 b2)
            (on-table b2)
            (on b3 b1)
        )
    )
)


(define (problem BW-3-9804-56)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
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


(define (problem BW-3-9804-57)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on-table b3)
        (clear b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-58)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on b1 b2)
        (on b2 b3)
        (on-table b3)
        (clear b1)
    )
    (:goal
        (and
            (on b1 b3)
            (on-table b2)
            (on b3 b2)
        )
    )
)


(define (problem BW-3-9804-59)
    (:domain blocksworld)
    (:objects b1 b2 b3)
    (:init
        (on-table b1)
        (on b2 b3)
        (on b3 b1)
        (clear b2)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b3)
            (on-table b3)
        )
    )
)


(define (problem BW-3-9804-60)
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