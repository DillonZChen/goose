(define (problem BW-91-1-5)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 b88 b89 b90 b91 - block)
    (:init
        (handempty)
        (on b1 b85)
        (on b2 b49)
        (on b3 b50)
        (on b4 b83)
        (on b5 b52)
        (on b6 b86)
        (on b7 b69)
        (on b8 b90)
        (on b9 b43)
        (on b10 b27)
        (on b11 b16)
        (on b12 b15)
        (on b13 b41)
        (on b14 b56)
        (on b15 b39)
        (on b16 b58)
        (on b17 b7)
        (on b18 b3)
        (on b19 b8)
        (on b20 b88)
        (on b21 b71)
        (on b22 b65)
        (on b23 b35)
        (on b24 b57)
        (on b25 b60)
        (on b26 b10)
        (on b27 b44)
        (on-table b28)
        (on b29 b19)
        (on b30 b70)
        (on-table b31)
        (on b32 b20)
        (on b33 b67)
        (on b34 b46)
        (on b35 b72)
        (on b36 b9)
        (on b37 b53)
        (on-table b38)
        (on b39 b78)
        (on-table b40)
        (on-table b41)
        (on-table b42)
        (on-table b43)
        (on b44 b51)
        (on b45 b31)
        (on b46 b87)
        (on b47 b66)
        (on b48 b80)
        (on-table b49)
        (on b50 b13)
        (on b51 b73)
        (on b52 b28)
        (on b53 b14)
        (on b54 b25)
        (on b55 b61)
        (on b56 b2)
        (on b57 b40)
        (on b58 b23)
        (on b59 b18)
        (on-table b60)
        (on b61 b84)
        (on b62 b81)
        (on b63 b24)
        (on b64 b6)
        (on b65 b17)
        (on b66 b79)
        (on b67 b68)
        (on b68 b22)
        (on b69 b64)
        (on b70 b91)
        (on b71 b29)
        (on b72 b38)
        (on b73 b76)
        (on b74 b45)
        (on b75 b30)
        (on b76 b75)
        (on b77 b89)
        (on b78 b26)
        (on b79 b82)
        (on b80 b32)
        (on b81 b21)
        (on-table b82)
        (on b83 b74)
        (on b84 b36)
        (on b85 b54)
        (on b86 b4)
        (on b87 b62)
        (on b88 b42)
        (on b89 b48)
        (on b90 b33)
        (on-table b91)
        (clear b1)
        (clear b5)
        (clear b11)
        (clear b12)
        (clear b34)
        (clear b37)
        (clear b47)
        (clear b55)
        (clear b59)
        (clear b63)
        (clear b77)
    )
    (:goal
        (and
            (on b1 b37)
            (on b2 b63)
            (on b3 b29)
            (on b4 b11)
            (on b5 b50)
            (on b6 b20)
            (on b7 b21)
            (on b8 b33)
            (on b9 b64)
            (on b10 b12)
            (on b11 b26)
            (on b12 b83)
            (on b13 b58)
            (on b14 b49)
            (on b15 b7)
            (on b16 b79)
            (on b17 b76)
            (on b18 b25)
            (on b19 b23)
            (on b20 b81)
            (on b21 b88)
            (on b22 b17)
            (on b23 b86)
            (on b24 b56)
            (on-table b25)
            (on-table b26)
            (on b27 b10)
            (on b28 b39)
            (on b29 b44)
            (on b30 b72)
            (on b31 b43)
            (on b32 b51)
            (on b33 b15)
            (on b34 b46)
            (on b35 b70)
            (on b36 b6)
            (on b37 b41)
            (on b38 b22)
            (on b39 b16)
            (on b40 b9)
            (on b41 b75)
            (on b42 b89)
            (on b43 b24)
            (on b44 b2)
            (on b45 b62)
            (on b46 b13)
            (on b47 b54)
            (on b48 b87)
            (on b49 b67)
            (on b50 b66)
            (on b51 b18)
            (on b52 b77)
            (on b53 b19)
            (on b54 b53)
            (on b55 b34)
            (on b56 b91)
            (on b57 b84)
            (on b58 b80)
            (on b59 b65)
            (on-table b60)
            (on b61 b27)
            (on b62 b8)
            (on b63 b36)
            (on b64 b73)
            (on b65 b60)
            (on b66 b90)
            (on b67 b28)
            (on b68 b32)
            (on b69 b61)
            (on-table b70)
            (on b71 b3)
            (on b72 b52)
            (on b73 b35)
            (on b74 b1)
            (on-table b75)
            (on b76 b4)
            (on b77 b47)
            (on b78 b42)
            (on b79 b82)
            (on b80 b69)
            (on b81 b45)
            (on b82 b5)
            (on b83 b68)
            (on b84 b31)
            (on b85 b59)
            (on b86 b74)
            (on-table b87)
            (on-table b88)
            (on b89 b48)
            (on b90 b38)
            (on b91 b30)
        )
    )
)