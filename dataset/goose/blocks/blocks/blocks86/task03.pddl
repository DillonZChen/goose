(define (problem BW-86-1-3)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 - block)
    (:init
        (handempty)
        (on b1 b44)
        (on b2 b63)
        (on b3 b71)
        (on b4 b52)
        (on-table b5)
        (on b6 b57)
        (on b7 b43)
        (on b8 b9)
        (on b9 b41)
        (on b10 b46)
        (on b11 b78)
        (on-table b12)
        (on b13 b26)
        (on b14 b3)
        (on b15 b75)
        (on b16 b82)
        (on b17 b25)
        (on b18 b2)
        (on b19 b70)
        (on b20 b28)
        (on b21 b61)
        (on b22 b55)
        (on b23 b54)
        (on b24 b83)
        (on-table b25)
        (on b26 b49)
        (on b27 b20)
        (on b28 b64)
        (on-table b29)
        (on b30 b80)
        (on b31 b18)
        (on-table b32)
        (on-table b33)
        (on b34 b68)
        (on b35 b8)
        (on b36 b47)
        (on-table b37)
        (on b38 b40)
        (on b39 b56)
        (on b40 b84)
        (on b41 b62)
        (on b42 b59)
        (on b43 b29)
        (on b44 b85)
        (on b45 b37)
        (on b46 b39)
        (on b47 b32)
        (on b48 b12)
        (on b49 b69)
        (on b50 b60)
        (on b51 b15)
        (on-table b52)
        (on b53 b51)
        (on b54 b31)
        (on b55 b77)
        (on b56 b72)
        (on-table b57)
        (on b58 b34)
        (on b59 b14)
        (on b60 b24)
        (on b61 b86)
        (on b62 b1)
        (on b63 b81)
        (on b64 b13)
        (on b65 b6)
        (on b66 b38)
        (on b67 b17)
        (on b68 b35)
        (on b69 b74)
        (on-table b70)
        (on b71 b33)
        (on b72 b30)
        (on b73 b53)
        (on b74 b36)
        (on b75 b4)
        (on-table b76)
        (on b77 b76)
        (on b78 b19)
        (on b79 b5)
        (on b80 b22)
        (on b81 b79)
        (on b82 b7)
        (on b83 b58)
        (on-table b84)
        (on b85 b11)
        (on b86 b65)
        (clear b10)
        (clear b16)
        (clear b21)
        (clear b23)
        (clear b27)
        (clear b42)
        (clear b45)
        (clear b48)
        (clear b50)
        (clear b66)
        (clear b67)
        (clear b73)
    )
    (:goal
        (and
            (on b1 b34)
            (on-table b2)
            (on b3 b24)
            (on-table b4)
            (on b5 b43)
            (on b6 b77)
            (on b7 b5)
            (on b8 b6)
            (on b9 b85)
            (on b10 b81)
            (on b11 b75)
            (on b12 b30)
            (on b13 b10)
            (on b14 b9)
            (on b15 b41)
            (on b16 b25)
            (on b17 b58)
            (on b18 b50)
            (on-table b19)
            (on b20 b4)
            (on b21 b15)
            (on b22 b27)
            (on b23 b35)
            (on b24 b11)
            (on b25 b68)
            (on b26 b44)
            (on b27 b60)
            (on b28 b59)
            (on b29 b74)
            (on b30 b38)
            (on-table b31)
            (on b32 b72)
            (on b33 b70)
            (on-table b34)
            (on b35 b8)
            (on b36 b45)
            (on b37 b79)
            (on b38 b16)
            (on b39 b3)
            (on b40 b61)
            (on-table b41)
            (on b42 b20)
            (on b43 b83)
            (on b44 b49)
            (on b45 b82)
            (on b46 b69)
            (on b47 b42)
            (on b48 b73)
            (on b49 b64)
            (on b50 b47)
            (on b51 b71)
            (on b52 b7)
            (on b53 b54)
            (on b54 b84)
            (on b55 b56)
            (on b56 b32)
            (on b57 b86)
            (on b58 b18)
            (on b59 b1)
            (on b60 b23)
            (on b61 b46)
            (on b62 b78)
            (on-table b63)
            (on b64 b66)
            (on b65 b2)
            (on-table b66)
            (on b67 b29)
            (on b68 b13)
            (on b69 b80)
            (on b70 b52)
            (on b71 b40)
            (on b72 b17)
            (on b73 b12)
            (on-table b74)
            (on-table b75)
            (on b76 b62)
            (on b77 b21)
            (on-table b78)
            (on b79 b19)
            (on b80 b31)
            (on b81 b33)
            (on b82 b65)
            (on b83 b63)
            (on b84 b76)
            (on b85 b55)
            (on b86 b28)
        )
    )
)