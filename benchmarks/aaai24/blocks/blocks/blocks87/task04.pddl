(define (problem BW-87-1-4)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 - block)
    (:init
        (handempty)
        (on b1 b58)
        (on b2 b26)
        (on-table b3)
        (on b4 b64)
        (on-table b5)
        (on b6 b37)
        (on-table b7)
        (on b8 b21)
        (on b9 b50)
        (on b10 b45)
        (on b11 b30)
        (on b12 b82)
        (on b13 b34)
        (on b14 b73)
        (on-table b15)
        (on b16 b9)
        (on-table b17)
        (on-table b18)
        (on b19 b6)
        (on b20 b3)
        (on-table b21)
        (on b22 b48)
        (on b23 b14)
        (on b24 b42)
        (on b25 b41)
        (on b26 b57)
        (on b27 b66)
        (on b28 b68)
        (on b29 b40)
        (on b30 b70)
        (on b31 b10)
        (on b32 b27)
        (on b33 b53)
        (on b34 b23)
        (on b35 b56)
        (on b36 b80)
        (on b37 b72)
        (on b38 b2)
        (on b39 b16)
        (on b40 b61)
        (on b41 b62)
        (on b42 b36)
        (on b43 b81)
        (on b44 b63)
        (on b45 b24)
        (on b46 b59)
        (on b47 b79)
        (on b48 b25)
        (on b49 b8)
        (on b50 b19)
        (on b51 b15)
        (on b52 b11)
        (on-table b53)
        (on b54 b65)
        (on b55 b87)
        (on b56 b75)
        (on b57 b32)
        (on b58 b77)
        (on b59 b52)
        (on-table b60)
        (on-table b61)
        (on b62 b49)
        (on b63 b86)
        (on b64 b44)
        (on b65 b18)
        (on b66 b20)
        (on b67 b83)
        (on b68 b54)
        (on-table b69)
        (on b70 b13)
        (on b71 b28)
        (on b72 b33)
        (on b73 b71)
        (on b74 b29)
        (on b75 b46)
        (on-table b76)
        (on b77 b7)
        (on b78 b5)
        (on b79 b38)
        (on b80 b60)
        (on b81 b76)
        (on b82 b69)
        (on b83 b85)
        (on b84 b35)
        (on b85 b1)
        (on b86 b39)
        (on b87 b43)
        (clear b4)
        (clear b12)
        (clear b17)
        (clear b22)
        (clear b31)
        (clear b47)
        (clear b51)
        (clear b55)
        (clear b67)
        (clear b74)
        (clear b78)
        (clear b84)
    )
    (:goal
        (and
            (on b1 b67)
            (on b2 b1)
            (on b3 b36)
            (on b4 b28)
            (on b5 b81)
            (on b6 b20)
            (on b7 b48)
            (on b8 b10)
            (on b9 b50)
            (on b10 b13)
            (on b11 b9)
            (on b12 b62)
            (on b13 b29)
            (on b14 b44)
            (on b15 b42)
            (on b16 b84)
            (on b17 b54)
            (on b18 b85)
            (on-table b19)
            (on b20 b31)
            (on b21 b86)
            (on b22 b73)
            (on b23 b49)
            (on b24 b5)
            (on b25 b74)
            (on b26 b60)
            (on b27 b52)
            (on b28 b8)
            (on b29 b58)
            (on b30 b61)
            (on b31 b4)
            (on b32 b51)
            (on b33 b35)
            (on b34 b68)
            (on b35 b7)
            (on b36 b30)
            (on b37 b19)
            (on b38 b46)
            (on b39 b37)
            (on b40 b82)
            (on b41 b47)
            (on b42 b16)
            (on b43 b24)
            (on b44 b32)
            (on b45 b6)
            (on b46 b25)
            (on b47 b33)
            (on b48 b63)
            (on b49 b69)
            (on b50 b87)
            (on-table b51)
            (on b52 b43)
            (on b53 b11)
            (on b54 b56)
            (on b55 b76)
            (on b56 b78)
            (on b57 b39)
            (on b58 b65)
            (on b59 b80)
            (on b60 b57)
            (on b61 b55)
            (on b62 b64)
            (on b63 b79)
            (on b64 b75)
            (on b65 b59)
            (on b66 b53)
            (on-table b67)
            (on-table b68)
            (on-table b69)
            (on b70 b2)
            (on b71 b72)
            (on-table b72)
            (on-table b73)
            (on b74 b3)
            (on b75 b21)
            (on b76 b83)
            (on b77 b70)
            (on b78 b66)
            (on b79 b22)
            (on b80 b23)
            (on b81 b41)
            (on-table b82)
            (on b83 b12)
            (on b84 b40)
            (on b85 b17)
            (on b86 b14)
            (on-table b87)
        )
    )
)