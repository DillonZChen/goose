(define (problem BW-86-1-1)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 - block)
    (:init
        (handempty)
        (on b1 b82)
        (on-table b2)
        (on b3 b84)
        (on b4 b53)
        (on b5 b33)
        (on b6 b12)
        (on b7 b52)
        (on b8 b35)
        (on b9 b66)
        (on b10 b78)
        (on b11 b16)
        (on b12 b17)
        (on b13 b32)
        (on b14 b13)
        (on b15 b44)
        (on-table b16)
        (on b17 b11)
        (on b18 b49)
        (on b19 b72)
        (on b20 b42)
        (on b21 b23)
        (on b22 b69)
        (on b23 b7)
        (on b24 b40)
        (on b25 b5)
        (on b26 b41)
        (on b27 b36)
        (on b28 b22)
        (on b29 b1)
        (on b30 b46)
        (on b31 b77)
        (on b32 b25)
        (on b33 b31)
        (on b34 b2)
        (on b35 b38)
        (on b36 b45)
        (on b37 b59)
        (on b38 b74)
        (on b39 b29)
        (on b40 b48)
        (on b41 b61)
        (on b42 b21)
        (on b43 b27)
        (on b44 b67)
        (on b45 b47)
        (on b46 b6)
        (on b47 b58)
        (on-table b48)
        (on-table b49)
        (on b50 b54)
        (on b51 b50)
        (on b52 b56)
        (on b53 b43)
        (on b54 b83)
        (on b55 b80)
        (on b56 b79)
        (on b57 b10)
        (on b58 b20)
        (on b59 b14)
        (on b60 b68)
        (on b61 b62)
        (on b62 b71)
        (on b63 b55)
        (on-table b64)
        (on b65 b9)
        (on b66 b37)
        (on b67 b73)
        (on-table b68)
        (on b69 b4)
        (on b70 b18)
        (on b71 b65)
        (on b72 b57)
        (on b73 b34)
        (on b74 b70)
        (on b75 b15)
        (on b76 b28)
        (on b77 b24)
        (on b78 b75)
        (on b79 b60)
        (on b80 b85)
        (on b81 b26)
        (on b82 b30)
        (on b83 b3)
        (on b84 b8)
        (on b85 b81)
        (on b86 b39)
        (clear b19)
        (clear b51)
        (clear b63)
        (clear b64)
        (clear b76)
        (clear b86)
    )
    (:goal
        (and
            (on b1 b7)
            (on b2 b19)
            (on-table b3)
            (on b4 b44)
            (on b5 b53)
            (on b6 b1)
            (on b7 b20)
            (on b8 b36)
            (on b9 b69)
            (on-table b10)
            (on b11 b73)
            (on b12 b80)
            (on b13 b65)
            (on-table b14)
            (on b15 b71)
            (on b16 b55)
            (on b17 b33)
            (on b18 b68)
            (on-table b19)
            (on b20 b45)
            (on b21 b46)
            (on b22 b32)
            (on-table b23)
            (on b24 b25)
            (on b25 b26)
            (on b26 b35)
            (on b27 b2)
            (on b28 b76)
            (on b29 b3)
            (on b30 b22)
            (on b31 b17)
            (on b32 b78)
            (on b33 b52)
            (on b34 b79)
            (on b35 b6)
            (on b36 b59)
            (on b37 b29)
            (on b38 b64)
            (on b39 b31)
            (on b40 b37)
            (on b41 b85)
            (on b42 b43)
            (on b43 b57)
            (on b44 b62)
            (on b45 b86)
            (on b46 b56)
            (on b47 b42)
            (on b48 b47)
            (on b49 b23)
            (on b50 b51)
            (on b51 b70)
            (on b52 b49)
            (on-table b53)
            (on b54 b40)
            (on b55 b83)
            (on b56 b34)
            (on b57 b5)
            (on b58 b30)
            (on b59 b41)
            (on b60 b82)
            (on b61 b9)
            (on b62 b67)
            (on b63 b11)
            (on-table b64)
            (on-table b65)
            (on b66 b60)
            (on b67 b27)
            (on b68 b48)
            (on b69 b72)
            (on b70 b13)
            (on b71 b12)
            (on b72 b38)
            (on b73 b18)
            (on b74 b61)
            (on b75 b14)
            (on b76 b21)
            (on b77 b39)
            (on b78 b66)
            (on b79 b15)
            (on b80 b58)
            (on-table b81)
            (on b82 b84)
            (on b83 b54)
            (on b84 b8)
            (on b85 b50)
            (on b86 b4)
        )
    )
)