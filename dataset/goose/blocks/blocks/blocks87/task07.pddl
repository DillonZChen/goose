(define (problem BW-87-1-7)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 - block)
    (:init
        (handempty)
        (on b1 b43)
        (on b2 b10)
        (on b3 b8)
        (on-table b4)
        (on b5 b75)
        (on b6 b86)
        (on-table b7)
        (on b8 b19)
        (on b9 b12)
        (on b10 b31)
        (on b11 b9)
        (on b12 b67)
        (on b13 b41)
        (on b14 b39)
        (on-table b15)
        (on b16 b63)
        (on b17 b54)
        (on b18 b27)
        (on b19 b15)
        (on b20 b21)
        (on-table b21)
        (on-table b22)
        (on b23 b79)
        (on b24 b42)
        (on b25 b52)
        (on b26 b3)
        (on b27 b11)
        (on b28 b46)
        (on b29 b24)
        (on b30 b1)
        (on b31 b80)
        (on b32 b6)
        (on b33 b44)
        (on b34 b49)
        (on b35 b51)
        (on b36 b13)
        (on b37 b59)
        (on b38 b37)
        (on b39 b53)
        (on b40 b87)
        (on b41 b55)
        (on b42 b83)
        (on b43 b25)
        (on b44 b38)
        (on b45 b48)
        (on b46 b32)
        (on b47 b18)
        (on b48 b72)
        (on b49 b33)
        (on b50 b30)
        (on b51 b2)
        (on b52 b20)
        (on-table b53)
        (on b54 b69)
        (on b55 b71)
        (on b56 b62)
        (on b57 b60)
        (on b58 b76)
        (on b59 b81)
        (on b60 b84)
        (on-table b61)
        (on b62 b7)
        (on b63 b26)
        (on b64 b82)
        (on b65 b36)
        (on b66 b61)
        (on b67 b45)
        (on b68 b4)
        (on b69 b34)
        (on b70 b50)
        (on b71 b22)
        (on b72 b5)
        (on b73 b28)
        (on b74 b85)
        (on b75 b56)
        (on b76 b64)
        (on b77 b35)
        (on b78 b40)
        (on b79 b29)
        (on b80 b14)
        (on b81 b23)
        (on b82 b17)
        (on b83 b65)
        (on b84 b74)
        (on b85 b70)
        (on-table b86)
        (on-table b87)
        (clear b16)
        (clear b47)
        (clear b57)
        (clear b58)
        (clear b66)
        (clear b68)
        (clear b73)
        (clear b77)
        (clear b78)
    )
    (:goal
        (and
            (on b1 b28)
            (on b2 b77)
            (on b3 b48)
            (on b4 b53)
            (on b5 b4)
            (on b6 b40)
            (on b7 b43)
            (on b8 b20)
            (on b9 b56)
            (on b10 b9)
            (on b11 b39)
            (on b12 b74)
            (on b13 b6)
            (on b14 b60)
            (on b15 b62)
            (on b16 b23)
            (on b17 b51)
            (on b18 b63)
            (on b19 b44)
            (on b20 b80)
            (on b21 b47)
            (on b22 b29)
            (on b23 b64)
            (on b24 b26)
            (on b25 b45)
            (on b26 b38)
            (on-table b27)
            (on b28 b31)
            (on b29 b66)
            (on b30 b16)
            (on b31 b5)
            (on b32 b85)
            (on b33 b8)
            (on-table b34)
            (on b35 b78)
            (on b36 b46)
            (on b37 b67)
            (on b38 b30)
            (on b39 b75)
            (on b40 b3)
            (on b41 b49)
            (on b42 b52)
            (on b43 b19)
            (on b44 b13)
            (on b45 b37)
            (on b46 b27)
            (on b47 b72)
            (on b48 b15)
            (on b49 b12)
            (on b50 b32)
            (on b51 b1)
            (on b52 b14)
            (on-table b53)
            (on b54 b33)
            (on b55 b50)
            (on b56 b2)
            (on b57 b59)
            (on b58 b21)
            (on b59 b24)
            (on b60 b25)
            (on-table b61)
            (on-table b62)
            (on b63 b36)
            (on b64 b82)
            (on b65 b7)
            (on b66 b84)
            (on b67 b35)
            (on b68 b65)
            (on b69 b54)
            (on b70 b42)
            (on b71 b70)
            (on b72 b22)
            (on b73 b76)
            (on-table b74)
            (on b75 b86)
            (on b76 b79)
            (on b77 b41)
            (on-table b78)
            (on b79 b58)
            (on b80 b17)
            (on b81 b10)
            (on b82 b68)
            (on b83 b69)
            (on b84 b87)
            (on b85 b73)
            (on b86 b61)
            (on b87 b34)
        )
    )
)