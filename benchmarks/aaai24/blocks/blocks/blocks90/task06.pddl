(define (problem BW-90-1-6)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 b88 b89 b90 - block)
    (:init
        (handempty)
        (on b1 b67)
        (on b2 b23)
        (on b3 b41)
        (on b4 b82)
        (on b5 b87)
        (on b6 b80)
        (on b7 b59)
        (on b8 b9)
        (on b9 b85)
        (on b10 b20)
        (on b11 b31)
        (on-table b12)
        (on b13 b39)
        (on b14 b83)
        (on-table b15)
        (on b16 b73)
        (on b17 b58)
        (on b18 b86)
        (on b19 b26)
        (on b20 b2)
        (on b21 b29)
        (on b22 b30)
        (on b23 b6)
        (on b24 b54)
        (on b25 b46)
        (on b26 b35)
        (on b27 b33)
        (on b28 b18)
        (on b29 b3)
        (on b30 b61)
        (on b31 b81)
        (on b32 b55)
        (on b33 b16)
        (on b34 b40)
        (on b35 b56)
        (on b36 b24)
        (on-table b37)
        (on b38 b10)
        (on b39 b60)
        (on b40 b68)
        (on b41 b38)
        (on b42 b63)
        (on b43 b12)
        (on b44 b19)
        (on b45 b70)
        (on-table b46)
        (on b47 b21)
        (on b48 b84)
        (on b49 b8)
        (on b50 b42)
        (on b51 b48)
        (on b52 b1)
        (on b53 b65)
        (on b54 b88)
        (on b55 b72)
        (on b56 b51)
        (on b57 b34)
        (on b58 b13)
        (on b59 b15)
        (on b60 b78)
        (on-table b61)
        (on b62 b47)
        (on b63 b66)
        (on b64 b28)
        (on b65 b75)
        (on b66 b4)
        (on b67 b49)
        (on b68 b89)
        (on b69 b32)
        (on b70 b79)
        (on-table b71)
        (on b72 b62)
        (on b73 b36)
        (on b74 b27)
        (on b75 b14)
        (on b76 b25)
        (on-table b77)
        (on b78 b11)
        (on b79 b37)
        (on b80 b53)
        (on b81 b45)
        (on b82 b69)
        (on b83 b43)
        (on b84 b7)
        (on b85 b64)
        (on b86 b90)
        (on b87 b71)
        (on b88 b17)
        (on b89 b22)
        (on b90 b74)
        (clear b5)
        (clear b44)
        (clear b50)
        (clear b52)
        (clear b57)
        (clear b76)
        (clear b77)
    )
    (:goal
        (and
            (on b1 b71)
            (on b2 b53)
            (on b3 b40)
            (on b4 b80)
            (on b5 b81)
            (on b6 b75)
            (on b7 b66)
            (on-table b8)
            (on b9 b45)
            (on b10 b70)
            (on-table b11)
            (on-table b12)
            (on b13 b48)
            (on b14 b42)
            (on b15 b35)
            (on b16 b7)
            (on b17 b58)
            (on b18 b9)
            (on b19 b25)
            (on-table b20)
            (on b21 b39)
            (on b22 b82)
            (on b23 b11)
            (on b24 b21)
            (on b25 b44)
            (on-table b26)
            (on b27 b19)
            (on b28 b56)
            (on-table b29)
            (on b30 b87)
            (on b31 b84)
            (on b32 b89)
            (on b33 b52)
            (on b34 b50)
            (on b35 b28)
            (on b36 b26)
            (on b37 b13)
            (on b38 b32)
            (on b39 b37)
            (on b40 b88)
            (on b41 b30)
            (on-table b42)
            (on b43 b62)
            (on b44 b12)
            (on b45 b63)
            (on b46 b73)
            (on b47 b57)
            (on b48 b43)
            (on b49 b64)
            (on b50 b17)
            (on b51 b76)
            (on b52 b38)
            (on b53 b49)
            (on b54 b36)
            (on b55 b67)
            (on b56 b33)
            (on b57 b54)
            (on b58 b72)
            (on b59 b65)
            (on b60 b27)
            (on-table b61)
            (on b62 b8)
            (on b63 b10)
            (on-table b64)
            (on b65 b77)
            (on b66 b41)
            (on-table b67)
            (on b68 b29)
            (on b69 b68)
            (on b70 b15)
            (on b71 b16)
            (on-table b72)
            (on b73 b85)
            (on b74 b34)
            (on b75 b47)
            (on b76 b1)
            (on b77 b2)
            (on b78 b86)
            (on b79 b90)
            (on b80 b55)
            (on-table b81)
            (on b82 b46)
            (on b83 b23)
            (on b84 b74)
            (on b85 b51)
            (on b86 b14)
            (on b87 b60)
            (on b88 b83)
            (on b89 b22)
            (on b90 b18)
        )
    )
)