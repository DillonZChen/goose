(define (problem BW-92-1-8)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 b88 b89 b90 b91 b92 - block)
    (:init
        (handempty)
        (on b1 b72)
        (on b2 b59)
        (on b3 b41)
        (on b4 b3)
        (on b5 b8)
        (on b6 b27)
        (on b7 b55)
        (on b8 b76)
        (on b9 b88)
        (on b10 b13)
        (on b11 b57)
        (on b12 b61)
        (on b13 b60)
        (on b14 b89)
        (on b15 b24)
        (on b16 b82)
        (on b17 b1)
        (on b18 b86)
        (on b19 b36)
        (on-table b20)
        (on b21 b80)
        (on b22 b16)
        (on b23 b74)
        (on b24 b31)
        (on b25 b69)
        (on-table b26)
        (on b27 b78)
        (on b28 b19)
        (on b29 b58)
        (on b30 b81)
        (on b31 b44)
        (on b32 b17)
        (on b33 b68)
        (on b34 b52)
        (on b35 b62)
        (on-table b36)
        (on b37 b38)
        (on b38 b64)
        (on b39 b40)
        (on b40 b92)
        (on b41 b49)
        (on b42 b77)
        (on b43 b4)
        (on b44 b73)
        (on-table b45)
        (on b46 b6)
        (on b47 b37)
        (on b48 b28)
        (on b49 b75)
        (on b50 b5)
        (on b51 b87)
        (on b52 b26)
        (on-table b53)
        (on b54 b70)
        (on b55 b65)
        (on b56 b2)
        (on b57 b15)
        (on b58 b66)
        (on b59 b48)
        (on b60 b67)
        (on b61 b46)
        (on b62 b30)
        (on b63 b39)
        (on b64 b83)
        (on b65 b56)
        (on b66 b9)
        (on b67 b85)
        (on b68 b18)
        (on b69 b90)
        (on b70 b20)
        (on b71 b23)
        (on b72 b54)
        (on b73 b51)
        (on b74 b47)
        (on b75 b91)
        (on b76 b29)
        (on-table b77)
        (on b78 b45)
        (on b79 b11)
        (on b80 b84)
        (on b81 b7)
        (on b82 b14)
        (on-table b83)
        (on b84 b25)
        (on b85 b33)
        (on b86 b43)
        (on b87 b50)
        (on b88 b34)
        (on b89 b35)
        (on b90 b79)
        (on b91 b21)
        (on b92 b22)
        (clear b10)
        (clear b12)
        (clear b32)
        (clear b42)
        (clear b53)
        (clear b63)
        (clear b71)
    )
    (:goal
        (and
            (on b1 b58)
            (on-table b2)
            (on b3 b11)
            (on b4 b19)
            (on-table b5)
            (on b6 b23)
            (on b7 b25)
            (on-table b8)
            (on b9 b37)
            (on b10 b67)
            (on b11 b71)
            (on b12 b18)
            (on-table b13)
            (on b14 b63)
            (on b15 b24)
            (on b16 b72)
            (on b17 b14)
            (on b18 b51)
            (on b19 b74)
            (on b20 b41)
            (on b21 b92)
            (on b22 b20)
            (on b23 b46)
            (on b24 b22)
            (on b25 b16)
            (on b26 b52)
            (on b27 b89)
            (on b28 b36)
            (on b29 b64)
            (on b30 b54)
            (on b31 b61)
            (on b32 b39)
            (on b33 b38)
            (on b34 b48)
            (on b35 b80)
            (on b36 b90)
            (on b37 b68)
            (on b38 b7)
            (on b39 b81)
            (on b40 b66)
            (on b41 b62)
            (on b42 b27)
            (on b43 b49)
            (on b44 b31)
            (on b45 b47)
            (on b46 b1)
            (on b47 b28)
            (on b48 b79)
            (on b49 b70)
            (on b50 b3)
            (on b51 b82)
            (on b52 b59)
            (on b53 b9)
            (on b54 b32)
            (on b55 b8)
            (on b56 b35)
            (on-table b57)
            (on b58 b69)
            (on b59 b91)
            (on b60 b45)
            (on-table b61)
            (on b62 b73)
            (on-table b63)
            (on b64 b15)
            (on b65 b21)
            (on b66 b83)
            (on b67 b5)
            (on b68 b17)
            (on b69 b13)
            (on b70 b40)
            (on b71 b53)
            (on b72 b6)
            (on b73 b26)
            (on b74 b65)
            (on b75 b87)
            (on b76 b55)
            (on b77 b30)
            (on b78 b60)
            (on b79 b29)
            (on-table b80)
            (on b81 b50)
            (on b82 b84)
            (on b83 b88)
            (on b84 b2)
            (on b85 b75)
            (on b86 b56)
            (on b87 b57)
            (on b88 b44)
            (on b89 b43)
            (on-table b90)
            (on b91 b78)
            (on b92 b85)
        )
    )
)