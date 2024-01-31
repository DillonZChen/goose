

(define (problem BW-47-1-1)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on b1 b43)
        (on b2 b35)
        (on-table b3)
        (on b4 b14)
        (on b5 b17)
        (on b6 b42)
        (on b7 b31)
        (on-table b8)
        (on b9 b3)
        (on b10 b18)
        (on b11 b10)
        (on b12 b11)
        (on b13 b20)
        (on b14 b7)
        (on b15 b29)
        (on b16 b1)
        (on b17 b2)
        (on b18 b8)
        (on-table b19)
        (on b20 b36)
        (on b21 b16)
        (on b22 b41)
        (on b23 b30)
        (on-table b24)
        (on b25 b46)
        (on b26 b37)
        (on-table b27)
        (on b28 b13)
        (on b29 b32)
        (on-table b30)
        (on b31 b28)
        (on b32 b4)
        (on b33 b26)
        (on-table b34)
        (on b35 b25)
        (on b36 b12)
        (on b37 b5)
        (on b38 b23)
        (on b39 b27)
        (on b40 b38)
        (on b41 b19)
        (on b42 b33)
        (on b43 b45)
        (on b44 b9)
        (on b45 b24)
        (on b46 b15)
        (on b47 b21)
        (clear b6)
        (clear b22)
        (clear b34)
        (clear b39)
        (clear b40)
        (clear b44)
        (clear b47)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b35)
            (on b3 b43)
            (on-table b4)
            (on b5 b27)
            (on-table b6)
            (on b7 b14)
            (on b8 b3)
            (on b9 b41)
            (on b10 b36)
            (on b11 b30)
            (on b12 b31)
            (on b13 b29)
            (on b14 b2)
            (on b15 b16)
            (on b16 b7)
            (on b17 b39)
            (on b18 b4)
            (on b19 b44)
            (on b20 b25)
            (on b21 b34)
            (on b22 b47)
            (on b23 b37)
            (on b24 b20)
            (on b25 b8)
            (on b26 b23)
            (on b27 b46)
            (on b28 b33)
            (on b29 b22)
            (on b30 b28)
            (on b31 b24)
            (on b32 b21)
            (on b33 b26)
            (on-table b34)
            (on b35 b17)
            (on b36 b11)
            (on b37 b40)
            (on-table b38)
            (on-table b39)
            (on b40 b5)
            (on b41 b15)
            (on b42 b12)
            (on b43 b10)
            (on b44 b32)
            (on b45 b38)
            (on b46 b13)
            (on b47 b45)
        )
    )
)


(define (problem BW-47-1-2)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on b1 b29)
        (on b2 b21)
        (on b3 b38)
        (on b4 b24)
        (on b5 b34)
        (on b6 b16)
        (on b7 b11)
        (on b8 b40)
        (on b9 b37)
        (on b10 b19)
        (on-table b11)
        (on b12 b17)
        (on-table b13)
        (on b14 b4)
        (on b15 b25)
        (on b16 b35)
        (on b17 b43)
        (on b18 b10)
        (on b19 b47)
        (on b20 b1)
        (on-table b21)
        (on b22 b42)
        (on b23 b18)
        (on-table b24)
        (on b25 b20)
        (on b26 b44)
        (on-table b27)
        (on b28 b13)
        (on-table b29)
        (on b30 b28)
        (on b31 b15)
        (on b32 b6)
        (on b33 b36)
        (on b34 b14)
        (on b35 b9)
        (on b36 b26)
        (on b37 b45)
        (on b38 b27)
        (on b39 b7)
        (on b40 b41)
        (on b41 b5)
        (on b42 b33)
        (on-table b43)
        (on b44 b3)
        (on b45 b22)
        (on b46 b23)
        (on b47 b30)
        (clear b2)
        (clear b8)
        (clear b12)
        (clear b31)
        (clear b32)
        (clear b39)
        (clear b46)
    )
    (:goal
        (and
            (on b1 b20)
            (on b2 b15)
            (on b3 b12)
            (on b4 b5)
            (on b5 b33)
            (on-table b6)
            (on b7 b21)
            (on-table b8)
            (on b9 b36)
            (on b10 b8)
            (on b11 b2)
            (on b12 b30)
            (on b13 b39)
            (on b14 b10)
            (on b15 b9)
            (on-table b16)
            (on b17 b44)
            (on b18 b6)
            (on b19 b41)
            (on b20 b42)
            (on b21 b3)
            (on b22 b46)
            (on b23 b18)
            (on b24 b25)
            (on b25 b19)
            (on b26 b22)
            (on b27 b11)
            (on b28 b24)
            (on b29 b38)
            (on b30 b28)
            (on-table b31)
            (on b32 b17)
            (on b33 b43)
            (on b34 b16)
            (on b35 b7)
            (on b36 b40)
            (on b37 b27)
            (on b38 b34)
            (on b39 b14)
            (on b40 b1)
            (on b41 b45)
            (on b42 b4)
            (on b43 b47)
            (on b44 b13)
            (on b45 b32)
            (on b46 b37)
            (on b47 b35)
        )
    )
)


(define (problem BW-47-1-3)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on b1 b4)
        (on b2 b17)
        (on b3 b33)
        (on b4 b10)
        (on b5 b42)
        (on b6 b40)
        (on b7 b1)
        (on b8 b36)
        (on b9 b39)
        (on b10 b2)
        (on b11 b23)
        (on b12 b28)
        (on b13 b19)
        (on b14 b37)
        (on b15 b46)
        (on-table b16)
        (on b17 b30)
        (on-table b18)
        (on b19 b9)
        (on b20 b7)
        (on b21 b6)
        (on b22 b26)
        (on b23 b3)
        (on-table b24)
        (on b25 b44)
        (on-table b26)
        (on-table b27)
        (on b28 b15)
        (on-table b29)
        (on b30 b32)
        (on b31 b12)
        (on b32 b47)
        (on b33 b22)
        (on b34 b25)
        (on b35 b14)
        (on b36 b5)
        (on b37 b24)
        (on b38 b13)
        (on b39 b31)
        (on b40 b34)
        (on b41 b8)
        (on b42 b29)
        (on b43 b41)
        (on b44 b16)
        (on b45 b20)
        (on b46 b43)
        (on b47 b18)
        (clear b11)
        (clear b21)
        (clear b27)
        (clear b35)
        (clear b38)
        (clear b45)
    )
    (:goal
        (and
            (on-table b1)
            (on-table b2)
            (on b3 b22)
            (on b4 b34)
            (on b5 b4)
            (on b6 b42)
            (on b7 b1)
            (on b8 b13)
            (on b9 b14)
            (on b10 b19)
            (on b11 b26)
            (on b12 b38)
            (on b13 b15)
            (on b14 b45)
            (on-table b15)
            (on b16 b29)
            (on-table b17)
            (on b18 b20)
            (on b19 b32)
            (on b20 b17)
            (on b21 b2)
            (on-table b22)
            (on b23 b33)
            (on b24 b10)
            (on b25 b27)
            (on b26 b24)
            (on-table b27)
            (on b28 b18)
            (on b29 b12)
            (on b30 b44)
            (on b31 b5)
            (on b32 b40)
            (on-table b33)
            (on b34 b46)
            (on b35 b3)
            (on-table b36)
            (on b37 b21)
            (on b38 b25)
            (on b39 b7)
            (on b40 b36)
            (on b41 b16)
            (on b42 b11)
            (on b43 b41)
            (on b44 b37)
            (on b45 b30)
            (on b46 b39)
            (on-table b47)
        )
    )
)


(define (problem BW-47-1-4)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on-table b1)
        (on b2 b7)
        (on-table b3)
        (on b4 b15)
        (on-table b5)
        (on b6 b21)
        (on b7 b39)
        (on b8 b37)
        (on b9 b35)
        (on b10 b13)
        (on b11 b16)
        (on b12 b45)
        (on b13 b18)
        (on b14 b42)
        (on b15 b12)
        (on b16 b24)
        (on-table b17)
        (on b18 b5)
        (on b19 b3)
        (on b20 b1)
        (on b21 b4)
        (on b22 b23)
        (on b23 b30)
        (on b24 b34)
        (on b25 b20)
        (on b26 b44)
        (on-table b27)
        (on-table b28)
        (on b29 b2)
        (on b30 b10)
        (on b31 b19)
        (on b32 b8)
        (on b33 b22)
        (on b34 b47)
        (on b35 b36)
        (on b36 b11)
        (on b37 b33)
        (on b38 b40)
        (on b39 b31)
        (on b40 b43)
        (on b41 b17)
        (on b42 b25)
        (on b43 b32)
        (on b44 b46)
        (on b45 b28)
        (on b46 b6)
        (on-table b47)
        (clear b9)
        (clear b14)
        (clear b26)
        (clear b27)
        (clear b29)
        (clear b38)
        (clear b41)
    )
    (:goal
        (and
            (on b1 b39)
            (on b2 b21)
            (on b3 b38)
            (on b4 b28)
            (on b5 b35)
            (on b6 b9)
            (on b7 b44)
            (on b8 b26)
            (on b9 b3)
            (on-table b10)
            (on-table b11)
            (on b12 b5)
            (on b13 b30)
            (on b14 b13)
            (on-table b15)
            (on b16 b36)
            (on-table b17)
            (on b18 b20)
            (on b19 b25)
            (on b20 b27)
            (on b21 b16)
            (on b22 b41)
            (on b23 b15)
            (on b24 b29)
            (on b25 b14)
            (on b26 b10)
            (on-table b27)
            (on b28 b1)
            (on-table b29)
            (on b30 b4)
            (on b31 b42)
            (on b32 b2)
            (on b33 b23)
            (on b34 b33)
            (on b35 b31)
            (on b36 b43)
            (on b37 b11)
            (on b38 b8)
            (on b39 b37)
            (on b40 b46)
            (on b41 b32)
            (on b42 b22)
            (on b43 b6)
            (on b44 b45)
            (on b45 b18)
            (on b46 b34)
            (on b47 b24)
        )
    )
)


(define (problem BW-47-1-5)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on b1 b21)
        (on b2 b44)
        (on b3 b35)
        (on b4 b34)
        (on b5 b26)
        (on-table b6)
        (on b7 b30)
        (on-table b8)
        (on b9 b27)
        (on-table b10)
        (on b11 b1)
        (on b12 b37)
        (on b13 b33)
        (on-table b14)
        (on b15 b7)
        (on b16 b9)
        (on b17 b3)
        (on b18 b41)
        (on b19 b5)
        (on b20 b31)
        (on b21 b25)
        (on b22 b6)
        (on b23 b24)
        (on b24 b20)
        (on b25 b10)
        (on b26 b39)
        (on b27 b8)
        (on b28 b18)
        (on b29 b15)
        (on b30 b4)
        (on b31 b46)
        (on-table b32)
        (on-table b33)
        (on b34 b36)
        (on b35 b11)
        (on b36 b40)
        (on b37 b32)
        (on-table b38)
        (on b39 b16)
        (on b40 b47)
        (on b41 b14)
        (on b42 b45)
        (on b43 b19)
        (on b44 b42)
        (on b45 b43)
        (on b46 b22)
        (on b47 b2)
        (clear b12)
        (clear b13)
        (clear b17)
        (clear b23)
        (clear b28)
        (clear b29)
        (clear b38)
    )
    (:goal
        (and
            (on b1 b35)
            (on b2 b4)
            (on b3 b43)
            (on b4 b45)
            (on b5 b14)
            (on b6 b12)
            (on b7 b18)
            (on b8 b26)
            (on b9 b3)
            (on b10 b6)
            (on b11 b32)
            (on b12 b30)
            (on b13 b9)
            (on b14 b21)
            (on b15 b23)
            (on b16 b37)
            (on b17 b27)
            (on b18 b17)
            (on-table b19)
            (on b20 b41)
            (on b21 b40)
            (on-table b22)
            (on b23 b7)
            (on b24 b34)
            (on b25 b11)
            (on b26 b42)
            (on b27 b20)
            (on b28 b44)
            (on b29 b1)
            (on b30 b46)
            (on b31 b29)
            (on b32 b38)
            (on b33 b16)
            (on b34 b47)
            (on-table b35)
            (on b36 b19)
            (on b37 b22)
            (on-table b38)
            (on b39 b33)
            (on-table b40)
            (on b41 b31)
            (on-table b42)
            (on b43 b39)
            (on b44 b36)
            (on b45 b5)
            (on b46 b28)
            (on-table b47)
        )
    )
)


(define (problem BW-47-1-6)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on b1 b39)
        (on b2 b7)
        (on b3 b24)
        (on b4 b16)
        (on b5 b44)
        (on b6 b22)
        (on b7 b9)
        (on b8 b5)
        (on-table b9)
        (on b10 b34)
        (on b11 b8)
        (on-table b12)
        (on b13 b31)
        (on b14 b11)
        (on b15 b10)
        (on b16 b42)
        (on b17 b28)
        (on b18 b41)
        (on b19 b21)
        (on b20 b1)
        (on b21 b29)
        (on b22 b17)
        (on-table b23)
        (on b24 b12)
        (on-table b25)
        (on b26 b14)
        (on b27 b45)
        (on-table b28)
        (on b29 b33)
        (on b30 b23)
        (on b31 b4)
        (on b32 b13)
        (on-table b33)
        (on b34 b35)
        (on b35 b20)
        (on b36 b25)
        (on b37 b46)
        (on b38 b36)
        (on b39 b18)
        (on b40 b3)
        (on b41 b43)
        (on b42 b26)
        (on b43 b19)
        (on-table b44)
        (on b45 b47)
        (on-table b46)
        (on b47 b2)
        (clear b6)
        (clear b15)
        (clear b27)
        (clear b30)
        (clear b32)
        (clear b37)
        (clear b38)
        (clear b40)
    )
    (:goal
        (and
            (on b1 b18)
            (on b2 b38)
            (on b3 b30)
            (on b4 b6)
            (on b5 b24)
            (on b6 b29)
            (on b7 b15)
            (on b8 b17)
            (on b9 b1)
            (on-table b10)
            (on b11 b20)
            (on b12 b26)
            (on b13 b2)
            (on b14 b41)
            (on-table b15)
            (on-table b16)
            (on b17 b4)
            (on b18 b23)
            (on b19 b8)
            (on b20 b25)
            (on b21 b12)
            (on-table b22)
            (on b23 b7)
            (on b24 b42)
            (on b25 b36)
            (on b26 b39)
            (on-table b27)
            (on-table b28)
            (on b29 b21)
            (on b30 b16)
            (on b31 b45)
            (on b32 b44)
            (on b33 b47)
            (on b34 b3)
            (on b35 b10)
            (on b36 b27)
            (on b37 b11)
            (on b38 b22)
            (on b39 b5)
            (on b40 b46)
            (on-table b41)
            (on b42 b34)
            (on b43 b28)
            (on b44 b35)
            (on b45 b14)
            (on b46 b13)
            (on b47 b43)
        )
    )
)


(define (problem BW-47-1-7)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on b1 b7)
        (on-table b2)
        (on b3 b30)
        (on b4 b17)
        (on b5 b15)
        (on b6 b24)
        (on b7 b23)
        (on b8 b19)
        (on b9 b8)
        (on b10 b12)
        (on b11 b4)
        (on-table b12)
        (on b13 b26)
        (on b14 b22)
        (on-table b15)
        (on b16 b27)
        (on b17 b46)
        (on-table b18)
        (on b19 b28)
        (on b20 b31)
        (on b21 b41)
        (on b22 b37)
        (on b23 b11)
        (on-table b24)
        (on b25 b36)
        (on b26 b45)
        (on b27 b32)
        (on b28 b2)
        (on b29 b43)
        (on b30 b40)
        (on b31 b10)
        (on b32 b33)
        (on b33 b44)
        (on b34 b35)
        (on b35 b42)
        (on b36 b20)
        (on b37 b29)
        (on b38 b18)
        (on b39 b6)
        (on b40 b39)
        (on b41 b47)
        (on b42 b14)
        (on b43 b38)
        (on-table b44)
        (on b45 b21)
        (on b46 b3)
        (on b47 b5)
        (clear b1)
        (clear b9)
        (clear b13)
        (clear b16)
        (clear b25)
        (clear b34)
    )
    (:goal
        (and
            (on b1 b6)
            (on b2 b20)
            (on-table b3)
            (on b4 b27)
            (on b5 b9)
            (on b6 b4)
            (on b7 b29)
            (on b8 b13)
            (on b9 b46)
            (on b10 b8)
            (on b11 b16)
            (on-table b12)
            (on b13 b18)
            (on b14 b1)
            (on b15 b35)
            (on-table b16)
            (on b17 b38)
            (on b18 b5)
            (on b19 b26)
            (on b20 b25)
            (on b21 b47)
            (on-table b22)
            (on b23 b31)
            (on-table b24)
            (on b25 b34)
            (on b26 b43)
            (on-table b27)
            (on b28 b24)
            (on b29 b44)
            (on b30 b2)
            (on b31 b17)
            (on b32 b33)
            (on b33 b39)
            (on b34 b42)
            (on-table b35)
            (on b36 b28)
            (on b37 b30)
            (on b38 b22)
            (on b39 b40)
            (on b40 b19)
            (on b41 b12)
            (on b42 b15)
            (on b43 b11)
            (on b44 b23)
            (on b45 b36)
            (on-table b46)
            (on b47 b7)
        )
    )
)


(define (problem BW-47-1-8)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on-table b1)
        (on b2 b41)
        (on b3 b21)
        (on b4 b36)
        (on b5 b11)
        (on b6 b16)
        (on b7 b32)
        (on b8 b27)
        (on b9 b20)
        (on-table b10)
        (on b11 b10)
        (on b12 b38)
        (on b13 b35)
        (on b14 b15)
        (on-table b15)
        (on b16 b22)
        (on b17 b18)
        (on b18 b14)
        (on b19 b46)
        (on b20 b5)
        (on b21 b25)
        (on b22 b29)
        (on b23 b39)
        (on b24 b45)
        (on b25 b24)
        (on b26 b19)
        (on b27 b12)
        (on b28 b40)
        (on b29 b23)
        (on-table b30)
        (on b31 b30)
        (on b32 b4)
        (on b33 b28)
        (on-table b34)
        (on b35 b8)
        (on-table b36)
        (on b37 b6)
        (on b38 b7)
        (on b39 b26)
        (on b40 b17)
        (on b41 b3)
        (on b42 b1)
        (on-table b43)
        (on b44 b13)
        (on b45 b42)
        (on b46 b33)
        (on b47 b44)
        (clear b2)
        (clear b9)
        (clear b31)
        (clear b34)
        (clear b37)
        (clear b43)
        (clear b47)
    )
    (:goal
        (and
            (on b1 b15)
            (on-table b2)
            (on b3 b34)
            (on-table b4)
            (on b5 b38)
            (on b6 b39)
            (on b7 b37)
            (on b8 b41)
            (on-table b9)
            (on b10 b4)
            (on b11 b40)
            (on-table b12)
            (on b13 b16)
            (on b14 b12)
            (on b15 b44)
            (on b16 b2)
            (on b17 b31)
            (on b18 b36)
            (on b19 b33)
            (on-table b20)
            (on b21 b45)
            (on b22 b14)
            (on b23 b35)
            (on b24 b29)
            (on b25 b17)
            (on b26 b47)
            (on b27 b10)
            (on b28 b21)
            (on b29 b6)
            (on b30 b1)
            (on b31 b13)
            (on b32 b24)
            (on b33 b3)
            (on b34 b42)
            (on b35 b27)
            (on b36 b30)
            (on b37 b25)
            (on b38 b8)
            (on b39 b5)
            (on b40 b23)
            (on-table b41)
            (on b42 b9)
            (on b43 b22)
            (on b44 b19)
            (on b45 b18)
            (on b46 b11)
            (on b47 b7)
        )
    )
)


(define (problem BW-47-1-9)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on b1 b21)
        (on b2 b42)
        (on b3 b13)
        (on b4 b3)
        (on b5 b4)
        (on b6 b17)
        (on b7 b38)
        (on b8 b30)
        (on b9 b47)
        (on-table b10)
        (on b11 b39)
        (on b12 b5)
        (on b13 b20)
        (on b14 b6)
        (on b15 b34)
        (on b16 b36)
        (on b17 b24)
        (on b18 b16)
        (on b19 b11)
        (on-table b20)
        (on b21 b46)
        (on b22 b40)
        (on b23 b25)
        (on b24 b12)
        (on b25 b8)
        (on b26 b18)
        (on b27 b37)
        (on b28 b41)
        (on-table b29)
        (on b30 b7)
        (on b31 b14)
        (on b32 b44)
        (on-table b33)
        (on-table b34)
        (on b35 b29)
        (on b36 b31)
        (on-table b37)
        (on b38 b43)
        (on b39 b1)
        (on b40 b9)
        (on-table b41)
        (on b42 b15)
        (on b43 b10)
        (on b44 b35)
        (on b45 b22)
        (on b46 b33)
        (on b47 b2)
        (clear b19)
        (clear b23)
        (clear b26)
        (clear b27)
        (clear b28)
        (clear b32)
        (clear b45)
    )
    (:goal
        (and
            (on b1 b11)
            (on b2 b15)
            (on b3 b25)
            (on b4 b45)
            (on b5 b28)
            (on-table b6)
            (on b7 b22)
            (on b8 b12)
            (on b9 b20)
            (on b10 b16)
            (on b11 b14)
            (on b12 b46)
            (on b13 b27)
            (on-table b14)
            (on b15 b1)
            (on b16 b47)
            (on b17 b24)
            (on b18 b31)
            (on b19 b38)
            (on-table b20)
            (on b21 b23)
            (on b22 b32)
            (on b23 b4)
            (on b24 b3)
            (on b25 b2)
            (on b26 b21)
            (on b27 b33)
            (on b28 b30)
            (on b29 b7)
            (on b30 b40)
            (on b31 b5)
            (on b32 b19)
            (on-table b33)
            (on-table b34)
            (on-table b35)
            (on b36 b13)
            (on b37 b43)
            (on b38 b36)
            (on b39 b8)
            (on-table b40)
            (on b41 b37)
            (on-table b42)
            (on b43 b26)
            (on b44 b42)
            (on b45 b17)
            (on b46 b35)
            (on b47 b34)
        )
    )
)


(define (problem BW-47-1-10)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47)
    (:init
        (on b1 b17)
        (on b2 b37)
        (on-table b3)
        (on b4 b35)
        (on b5 b38)
        (on b6 b47)
        (on b7 b26)
        (on b8 b45)
        (on b9 b42)
        (on-table b10)
        (on b11 b1)
        (on b12 b23)
        (on-table b13)
        (on b14 b8)
        (on b15 b40)
        (on b16 b4)
        (on b17 b13)
        (on b18 b5)
        (on b19 b39)
        (on b20 b2)
        (on b21 b43)
        (on b22 b16)
        (on-table b23)
        (on b24 b33)
        (on b25 b24)
        (on b26 b34)
        (on b27 b7)
        (on b28 b27)
        (on b29 b12)
        (on b30 b44)
        (on b31 b28)
        (on b32 b29)
        (on b33 b3)
        (on b34 b18)
        (on b35 b6)
        (on b36 b41)
        (on b37 b46)
        (on b38 b11)
        (on b39 b32)
        (on b40 b19)
        (on b41 b25)
        (on b42 b10)
        (on-table b43)
        (on b44 b31)
        (on b45 b36)
        (on b46 b14)
        (on b47 b30)
        (clear b9)
        (clear b15)
        (clear b20)
        (clear b21)
        (clear b22)
    )
    (:goal
        (and
            (on-table b1)
            (on b2 b33)
            (on-table b3)
            (on-table b4)
            (on-table b5)
            (on b6 b15)
            (on b7 b38)
            (on b8 b25)
            (on b9 b7)
            (on-table b10)
            (on b11 b16)
            (on b12 b5)
            (on b13 b46)
            (on-table b14)
            (on b15 b41)
            (on b16 b47)
            (on b17 b4)
            (on-table b18)
            (on b19 b35)
            (on b20 b45)
            (on b21 b42)
            (on b22 b40)
            (on b23 b43)
            (on b24 b30)
            (on b25 b26)
            (on b26 b24)
            (on b27 b39)
            (on b28 b17)
            (on b29 b21)
            (on-table b30)
            (on b31 b29)
            (on b32 b19)
            (on b33 b36)
            (on b34 b32)
            (on b35 b22)
            (on b36 b23)
            (on b37 b18)
            (on b38 b37)
            (on b39 b28)
            (on-table b40)
            (on b41 b27)
            (on b42 b34)
            (on b43 b12)
            (on b44 b6)
            (on b45 b31)
            (on b46 b11)
            (on b47 b8)
        )
    )
)