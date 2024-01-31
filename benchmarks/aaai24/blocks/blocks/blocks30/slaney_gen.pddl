

(define (problem BW-30-1-1)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on b1 b26)
        (on b2 b16)
        (on-table b3)
        (on b4 b7)
        (on b5 b2)
        (on b6 b9)
        (on b7 b3)
        (on-table b8)
        (on-table b9)
        (on b10 b1)
        (on b11 b27)
        (on b12 b8)
        (on b13 b6)
        (on b14 b10)
        (on b15 b4)
        (on-table b16)
        (on-table b17)
        (on b18 b11)
        (on b19 b13)
        (on b20 b24)
        (on-table b21)
        (on b22 b17)
        (on b23 b21)
        (on b24 b19)
        (on b25 b22)
        (on b26 b28)
        (on b27 b20)
        (on b28 b29)
        (on-table b29)
        (on b30 b14)
        (clear b5)
        (clear b12)
        (clear b15)
        (clear b18)
        (clear b23)
        (clear b25)
        (clear b30)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b19)
            (on b3 b14)
            (on b4 b1)
            (on-table b5)
            (on b6 b30)
            (on b7 b15)
            (on-table b8)
            (on b9 b16)
            (on b10 b12)
            (on b11 b18)
            (on b12 b22)
            (on b13 b21)
            (on b14 b7)
            (on b15 b2)
            (on b16 b25)
            (on b17 b28)
            (on b18 b8)
            (on b19 b4)
            (on b20 b13)
            (on b21 b17)
            (on b22 b24)
            (on b23 b10)
            (on b24 b26)
            (on b25 b20)
            (on b26 b5)
            (on b27 b23)
            (on b28 b29)
            (on-table b29)
            (on b30 b11)
        )
    )
)


(define (problem BW-30-1-2)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on b1 b3)
        (on-table b2)
        (on b3 b29)
        (on b4 b24)
        (on b5 b26)
        (on-table b6)
        (on b7 b30)
        (on b8 b18)
        (on b9 b1)
        (on b10 b12)
        (on b11 b28)
        (on b12 b16)
        (on b13 b9)
        (on b14 b13)
        (on b15 b17)
        (on b16 b15)
        (on b17 b2)
        (on-table b18)
        (on b19 b25)
        (on b20 b21)
        (on b21 b23)
        (on b22 b4)
        (on b23 b7)
        (on b24 b27)
        (on b25 b10)
        (on b26 b20)
        (on b27 b5)
        (on b28 b22)
        (on b29 b8)
        (on b30 b6)
        (clear b11)
        (clear b14)
        (clear b19)
    )
    (:goal
        (and
            (on b1 b15)
            (on b2 b24)
            (on b3 b10)
            (on b4 b3)
            (on b5 b19)
            (on b6 b7)
            (on-table b7)
            (on b8 b22)
            (on b9 b23)
            (on b10 b2)
            (on b11 b8)
            (on b12 b20)
            (on b13 b29)
            (on b14 b6)
            (on b15 b30)
            (on b16 b17)
            (on b17 b27)
            (on b18 b26)
            (on b19 b12)
            (on b20 b18)
            (on b21 b11)
            (on b22 b5)
            (on b23 b16)
            (on-table b24)
            (on-table b25)
            (on-table b26)
            (on b27 b28)
            (on-table b28)
            (on b29 b14)
            (on-table b30)
        )
    )
)


(define (problem BW-30-1-3)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on b1 b25)
        (on b2 b24)
        (on b3 b1)
        (on b4 b7)
        (on b5 b13)
        (on b6 b28)
        (on b7 b30)
        (on b8 b11)
        (on b9 b3)
        (on b10 b8)
        (on b11 b12)
        (on b12 b18)
        (on b13 b9)
        (on b14 b5)
        (on b15 b20)
        (on b16 b2)
        (on b17 b4)
        (on b18 b14)
        (on b19 b6)
        (on b20 b29)
        (on b21 b23)
        (on b22 b17)
        (on b23 b22)
        (on b24 b10)
        (on b25 b21)
        (on b26 b19)
        (on b27 b16)
        (on-table b28)
        (on b29 b26)
        (on b30 b15)
        (clear b27)
    )
    (:goal
        (and
            (on b1 b24)
            (on b2 b18)
            (on b3 b21)
            (on b4 b7)
            (on b5 b27)
            (on-table b6)
            (on b7 b8)
            (on b8 b11)
            (on b9 b15)
            (on b10 b1)
            (on b11 b22)
            (on-table b12)
            (on b13 b2)
            (on-table b14)
            (on b15 b28)
            (on b16 b9)
            (on b17 b6)
            (on b18 b10)
            (on b19 b5)
            (on b20 b19)
            (on b21 b17)
            (on b22 b16)
            (on b23 b20)
            (on b24 b4)
            (on b25 b13)
            (on b26 b12)
            (on b27 b14)
            (on b28 b23)
            (on b29 b3)
            (on b30 b25)
        )
    )
)


(define (problem BW-30-1-4)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on b1 b26)
        (on b2 b4)
        (on b3 b9)
        (on b4 b30)
        (on b5 b13)
        (on b6 b17)
        (on b7 b25)
        (on b8 b28)
        (on b9 b24)
        (on-table b10)
        (on b11 b1)
        (on b12 b14)
        (on b13 b20)
        (on-table b14)
        (on b15 b21)
        (on b16 b19)
        (on b17 b22)
        (on b18 b23)
        (on b19 b7)
        (on-table b20)
        (on b21 b5)
        (on-table b22)
        (on-table b23)
        (on b24 b15)
        (on b25 b6)
        (on-table b26)
        (on b27 b18)
        (on b28 b10)
        (on-table b29)
        (on b30 b8)
        (clear b2)
        (clear b3)
        (clear b11)
        (clear b12)
        (clear b16)
        (clear b27)
        (clear b29)
    )
    (:goal
        (and
            (on b1 b16)
            (on b2 b9)
            (on b3 b2)
            (on b4 b3)
            (on b5 b8)
            (on b6 b17)
            (on b7 b20)
            (on b8 b15)
            (on b9 b5)
            (on b10 b14)
            (on b11 b27)
            (on-table b12)
            (on b13 b25)
            (on-table b14)
            (on b15 b26)
            (on b16 b12)
            (on b17 b24)
            (on b18 b28)
            (on-table b19)
            (on b20 b18)
            (on b21 b10)
            (on-table b22)
            (on b23 b13)
            (on-table b24)
            (on b25 b11)
            (on b26 b30)
            (on b27 b6)
            (on-table b28)
            (on b29 b7)
            (on-table b30)
        )
    )
)


(define (problem BW-30-1-5)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on b1 b4)
        (on b2 b11)
        (on-table b3)
        (on-table b4)
        (on b5 b23)
        (on b6 b8)
        (on-table b7)
        (on b8 b1)
        (on b9 b17)
        (on b10 b2)
        (on b11 b24)
        (on b12 b5)
        (on b13 b22)
        (on-table b14)
        (on-table b15)
        (on b16 b28)
        (on b17 b10)
        (on b18 b26)
        (on b19 b7)
        (on b20 b15)
        (on b21 b14)
        (on b22 b27)
        (on-table b23)
        (on b24 b29)
        (on b25 b3)
        (on b26 b6)
        (on b27 b12)
        (on b28 b13)
        (on b29 b30)
        (on b30 b18)
        (clear b9)
        (clear b16)
        (clear b19)
        (clear b20)
        (clear b21)
        (clear b25)
    )
    (:goal
        (and
            (on b1 b6)
            (on-table b2)
            (on b3 b13)
            (on b4 b16)
            (on b5 b21)
            (on-table b6)
            (on b7 b25)
            (on b8 b4)
            (on b9 b12)
            (on b10 b27)
            (on b11 b20)
            (on b12 b11)
            (on b13 b28)
            (on-table b14)
            (on b15 b5)
            (on b16 b23)
            (on b17 b19)
            (on b18 b30)
            (on-table b19)
            (on b20 b3)
            (on b21 b22)
            (on b22 b18)
            (on b23 b7)
            (on b24 b8)
            (on-table b25)
            (on b26 b14)
            (on b27 b24)
            (on b28 b10)
            (on b29 b2)
            (on-table b30)
        )
    )
)


(define (problem BW-30-1-6)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on-table b1)
        (on b2 b7)
        (on-table b3)
        (on b4 b16)
        (on-table b5)
        (on b6 b22)
        (on b7 b3)
        (on b8 b24)
        (on b9 b23)
        (on-table b10)
        (on b11 b17)
        (on b12 b27)
        (on-table b13)
        (on b14 b30)
        (on b15 b26)
        (on b16 b29)
        (on b17 b18)
        (on b18 b2)
        (on-table b19)
        (on b20 b4)
        (on b21 b1)
        (on b22 b9)
        (on b23 b12)
        (on b24 b20)
        (on b25 b13)
        (on b26 b21)
        (on b27 b10)
        (on b28 b5)
        (on b29 b25)
        (on-table b30)
        (clear b6)
        (clear b8)
        (clear b11)
        (clear b14)
        (clear b15)
        (clear b19)
        (clear b28)
    )
    (:goal
        (and
            (on b1 b15)
            (on-table b2)
            (on b3 b25)
            (on b4 b19)
            (on b5 b1)
            (on b6 b5)
            (on-table b7)
            (on b8 b20)
            (on-table b9)
            (on b10 b21)
            (on b11 b16)
            (on b12 b17)
            (on b13 b30)
            (on b14 b26)
            (on b15 b10)
            (on b16 b28)
            (on b17 b4)
            (on-table b18)
            (on b19 b22)
            (on b20 b3)
            (on b21 b13)
            (on b22 b18)
            (on b23 b6)
            (on b24 b29)
            (on b25 b23)
            (on-table b26)
            (on b27 b8)
            (on-table b28)
            (on b29 b9)
            (on b30 b12)
        )
    )
)


(define (problem BW-30-1-7)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on b1 b22)
        (on b2 b4)
        (on-table b3)
        (on-table b4)
        (on b5 b14)
        (on b6 b12)
        (on b7 b20)
        (on b8 b30)
        (on b9 b8)
        (on b10 b17)
        (on b11 b5)
        (on b12 b18)
        (on-table b13)
        (on b14 b21)
        (on-table b15)
        (on b16 b24)
        (on b17 b27)
        (on b18 b13)
        (on b19 b2)
        (on b20 b25)
        (on-table b21)
        (on b22 b6)
        (on b23 b28)
        (on b24 b1)
        (on b25 b19)
        (on b26 b23)
        (on b27 b11)
        (on b28 b7)
        (on b29 b15)
        (on b30 b26)
        (clear b3)
        (clear b9)
        (clear b10)
        (clear b16)
        (clear b29)
    )
    (:goal
        (and
            (on b1 b11)
            (on b2 b29)
            (on b3 b28)
            (on-table b4)
            (on b5 b21)
            (on b6 b3)
            (on b7 b30)
            (on b8 b1)
            (on-table b9)
            (on-table b10)
            (on b11 b23)
            (on b12 b16)
            (on b13 b20)
            (on b14 b9)
            (on b15 b13)
            (on b16 b2)
            (on b17 b24)
            (on b18 b4)
            (on-table b19)
            (on b20 b19)
            (on b21 b26)
            (on b22 b17)
            (on-table b23)
            (on-table b24)
            (on-table b25)
            (on b26 b14)
            (on b27 b10)
            (on b28 b15)
            (on b29 b25)
            (on b30 b18)
        )
    )
)


(define (problem BW-30-1-8)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on b1 b8)
        (on-table b2)
        (on b3 b23)
        (on-table b4)
        (on b5 b9)
        (on b6 b2)
        (on b7 b16)
        (on b8 b29)
        (on b9 b24)
        (on b10 b17)
        (on-table b11)
        (on b12 b20)
        (on-table b13)
        (on b14 b12)
        (on b15 b21)
        (on b16 b15)
        (on-table b17)
        (on b18 b5)
        (on b19 b13)
        (on b20 b10)
        (on b21 b11)
        (on-table b22)
        (on b23 b27)
        (on b24 b30)
        (on-table b25)
        (on-table b26)
        (on b27 b4)
        (on b28 b3)
        (on b29 b6)
        (on b30 b25)
        (clear b1)
        (clear b7)
        (clear b14)
        (clear b18)
        (clear b19)
        (clear b22)
        (clear b26)
        (clear b28)
    )
    (:goal
        (and
            (on b1 b14)
            (on b2 b7)
            (on-table b3)
            (on b4 b23)
            (on-table b5)
            (on b6 b26)
            (on b7 b6)
            (on-table b8)
            (on b9 b2)
            (on b10 b13)
            (on b11 b28)
            (on b12 b27)
            (on b13 b20)
            (on-table b14)
            (on b15 b16)
            (on b16 b11)
            (on b17 b10)
            (on b18 b29)
            (on b19 b15)
            (on b20 b5)
            (on b21 b22)
            (on-table b22)
            (on b23 b9)
            (on b24 b18)
            (on b25 b12)
            (on b26 b30)
            (on b27 b1)
            (on-table b28)
            (on-table b29)
            (on b30 b24)
        )
    )
)


(define (problem BW-30-1-9)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on b1 b2)
        (on b2 b27)
        (on b3 b15)
        (on b4 b6)
        (on-table b5)
        (on b6 b20)
        (on b7 b5)
        (on b8 b11)
        (on b9 b18)
        (on b10 b7)
        (on-table b11)
        (on b12 b29)
        (on b13 b10)
        (on b14 b22)
        (on b15 b24)
        (on b16 b17)
        (on b17 b19)
        (on b18 b1)
        (on-table b19)
        (on-table b20)
        (on b21 b26)
        (on-table b22)
        (on b23 b4)
        (on b24 b13)
        (on b25 b9)
        (on b26 b14)
        (on b27 b16)
        (on-table b28)
        (on b29 b3)
        (on b30 b12)
        (clear b8)
        (clear b21)
        (clear b23)
        (clear b25)
        (clear b28)
        (clear b30)
    )
    (:goal
        (and
            (on b1 b12)
            (on b2 b13)
            (on b3 b4)
            (on-table b4)
            (on b5 b25)
            (on b6 b23)
            (on b7 b24)
            (on b8 b7)
            (on-table b9)
            (on b10 b20)
            (on b11 b10)
            (on b12 b28)
            (on b13 b1)
            (on b14 b2)
            (on b15 b8)
            (on b16 b18)
            (on b17 b3)
            (on-table b18)
            (on b19 b27)
            (on b20 b9)
            (on b21 b26)
            (on-table b22)
            (on-table b23)
            (on b24 b11)
            (on b25 b6)
            (on b26 b29)
            (on-table b27)
            (on b28 b15)
            (on b29 b17)
            (on b30 b21)
        )
    )
)


(define (problem BW-30-1-10)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30)
    (:init
        (on b1 b3)
        (on b2 b4)
        (on b3 b25)
        (on b4 b21)
        (on-table b5)
        (on b6 b10)
        (on b7 b30)
        (on b8 b23)
        (on b9 b12)
        (on b10 b26)
        (on b11 b19)
        (on b12 b6)
        (on-table b13)
        (on b14 b18)
        (on b15 b28)
        (on b16 b9)
        (on b17 b13)
        (on b18 b24)
        (on b19 b22)
        (on b20 b7)
        (on b21 b15)
        (on b22 b1)
        (on b23 b5)
        (on b24 b11)
        (on b25 b2)
        (on b26 b14)
        (on b27 b8)
        (on b28 b29)
        (on b29 b17)
        (on b30 b27)
        (clear b16)
        (clear b20)
    )
    (:goal
        (and
            (on b1 b15)
            (on b2 b8)
            (on b3 b7)
            (on b4 b3)
            (on b5 b16)
            (on b6 b24)
            (on b7 b20)
            (on b8 b29)
            (on-table b9)
            (on-table b10)
            (on b11 b26)
            (on b12 b14)
            (on b13 b9)
            (on-table b14)
            (on b15 b21)
            (on b16 b4)
            (on b17 b13)
            (on b18 b19)
            (on b19 b5)
            (on b20 b2)
            (on b21 b10)
            (on b22 b30)
            (on-table b23)
            (on b24 b27)
            (on b25 b17)
            (on b26 b12)
            (on b27 b23)
            (on b28 b11)
            (on b29 b28)
            (on b30 b6)
        )
    )
)