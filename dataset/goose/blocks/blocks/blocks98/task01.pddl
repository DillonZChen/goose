(define (problem BW-98-1-1)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 b88 b89 b90 b91 b92 b93 b94 b95 b96 b97 b98 - block)
    (:init
        (handempty)
        (on b1 b94)
        (on b2 b24)
        (on b3 b7)
        (on b4 b49)
        (on b5 b62)
        (on b6 b14)
        (on-table b7)
        (on b8 b4)
        (on b9 b75)
        (on-table b10)
        (on b11 b78)
        (on b12 b68)
        (on b13 b19)
        (on b14 b20)
        (on b15 b69)
        (on b16 b89)
        (on b17 b37)
        (on b18 b95)
        (on-table b19)
        (on b20 b13)
        (on b21 b17)
        (on b22 b84)
        (on b23 b47)
        (on-table b24)
        (on b25 b72)
        (on b26 b81)
        (on b27 b43)
        (on b28 b16)
        (on b29 b91)
        (on b30 b55)
        (on b31 b23)
        (on b32 b97)
        (on b33 b1)
        (on b34 b93)
        (on b35 b53)
        (on b36 b26)
        (on b37 b67)
        (on b38 b66)
        (on b39 b30)
        (on b40 b71)
        (on b41 b57)
        (on-table b42)
        (on b43 b36)
        (on b44 b64)
        (on b45 b33)
        (on b46 b58)
        (on b47 b54)
        (on b48 b21)
        (on b49 b15)
        (on b50 b42)
        (on b51 b11)
        (on b52 b79)
        (on b53 b6)
        (on b54 b73)
        (on-table b55)
        (on b56 b39)
        (on b57 b5)
        (on b58 b25)
        (on b59 b38)
        (on b60 b34)
        (on b61 b56)
        (on b62 b61)
        (on b63 b12)
        (on b64 b51)
        (on b65 b52)
        (on b66 b87)
        (on b67 b50)
        (on b68 b90)
        (on b69 b18)
        (on b70 b27)
        (on b71 b80)
        (on b72 b83)
        (on b73 b77)
        (on-table b74)
        (on b75 b22)
        (on b76 b9)
        (on b77 b46)
        (on b78 b85)
        (on b79 b48)
        (on b80 b92)
        (on b81 b28)
        (on b82 b3)
        (on b83 b60)
        (on b84 b31)
        (on b85 b59)
        (on b86 b82)
        (on b87 b10)
        (on-table b88)
        (on b89 b63)
        (on b90 b40)
        (on b91 b44)
        (on b92 b96)
        (on b93 b88)
        (on b94 b35)
        (on b95 b41)
        (on b96 b32)
        (on b97 b86)
        (on b98 b45)
        (clear b2)
        (clear b8)
        (clear b29)
        (clear b65)
        (clear b70)
        (clear b74)
        (clear b76)
        (clear b98)
    )
    (:goal
        (and
            (on b1 b65)
            (on-table b2)
            (on b3 b74)
            (on b4 b35)
            (on b5 b16)
            (on b6 b15)
            (on b7 b3)
            (on b8 b7)
            (on b9 b49)
            (on-table b10)
            (on b11 b55)
            (on-table b12)
            (on b13 b36)
            (on b14 b52)
            (on b15 b77)
            (on b16 b72)
            (on b17 b92)
            (on b18 b26)
            (on b19 b89)
            (on b20 b85)
            (on b21 b98)
            (on b22 b66)
            (on b23 b95)
            (on b24 b86)
            (on b25 b90)
            (on b26 b84)
            (on b27 b11)
            (on b28 b57)
            (on b29 b58)
            (on-table b30)
            (on b31 b39)
            (on b32 b6)
            (on-table b33)
            (on b34 b31)
            (on b35 b46)
            (on b36 b71)
            (on-table b37)
            (on-table b38)
            (on b39 b24)
            (on b40 b17)
            (on b41 b88)
            (on b42 b9)
            (on b43 b81)
            (on b44 b38)
            (on b45 b64)
            (on b46 b8)
            (on b47 b70)
            (on b48 b51)
            (on b49 b20)
            (on b50 b23)
            (on b51 b82)
            (on b52 b41)
            (on b53 b25)
            (on b54 b34)
            (on b55 b54)
            (on b56 b96)
            (on-table b57)
            (on b58 b63)
            (on b59 b19)
            (on b60 b40)
            (on-table b61)
            (on b62 b97)
            (on b63 b69)
            (on b64 b59)
            (on b65 b67)
            (on b66 b60)
            (on b67 b32)
            (on b68 b56)
            (on b69 b68)
            (on b70 b76)
            (on b71 b73)
            (on-table b72)
            (on b73 b42)
            (on-table b74)
            (on b75 b10)
            (on-table b76)
            (on b77 b29)
            (on b78 b45)
            (on b79 b80)
            (on b80 b22)
            (on b81 b50)
            (on-table b82)
            (on b83 b62)
            (on b84 b48)
            (on b85 b93)
            (on b86 b30)
            (on b87 b27)
            (on b88 b75)
            (on b89 b37)
            (on b90 b13)
            (on b91 b78)
            (on b92 b18)
            (on b93 b12)
            (on b94 b21)
            (on b95 b14)
            (on b96 b61)
            (on b97 b53)
            (on b98 b87)
        )
    )
)